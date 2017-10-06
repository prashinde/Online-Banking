#include <stdlib.h>
#include <string.h>
#include <iomanip>
#include <string>
#include <iostream>

#include "client.h"
#include "util.h"
#include "logger.h"
#include "trans.h"

using namespace std;

int main(int argc, char *argv[])
{
	int           rc;
	unsigned int  maxq;
	c_queue      *Q;
	int rate;

	maxq = atoi(argv[2]);

    	ios_base::sync_with_stdio(false); 
	setiosflags(ios::fixed);
	setprecision(15);

	Q = new c_queue(maxq);
	if(Q == nullptr) {
		cr_log << "Client ran out of memory!" << endl;
		return -ENOMEM;
	}

	read_ctx_t *r_ctx = new read_ctx_t;
	if(r_ctx == NULL) {
		cr_log << "Unable to creat reader context." << endl;
		delete Q;
		return -ENOMEM;
	}

	r_ctx->filename = argv[1];
	r_ctx->Q = Q;
	thread t1(parse_file, r_ctx);

	/* Client will generate requests at the rate. */
	rate = atoi(argv[5]);

	writer_ctx_t *w_ctx = new writer_ctx_t;
	if(w_ctx == NULL) {
		cr_log << "Unable to creat writer context." << endl;
		t1.join();
		delete r_ctx;
		delete Q;
		return -ENOMEM;
	}

	w_ctx->Q = Q;
	w_ctx->rate = rate;
	w_ctx->mult = atoi(argv[6]);
	w_ctx->ip_addr = argv[3];
	w_ctx->port = atoi(argv[4]);
	w_ctx->id = atoi(argv[7]);
	thread t2(connector, w_ctx);
	t1.join();
	t2.join();
	delete r_ctx;
	delete w_ctx;
	delete Q;
	return 0;
}
