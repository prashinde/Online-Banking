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
	c_queue      *Q; /* Transaction Queue */
	int rate;

	/* Some parameters for floating point and log */
    	ios_base::sync_with_stdio(false); 
	setiosflags(ios::fixed);
	setprecision(15);

	if(argc != 7) {
		cout << "usage ./client <trans.txt> <queue-len> <ip-addr> <portno> <request-rate> <mult> <id>" << endl;
		cout << "\
			 1. trans.txt -> Transactions file \n \
			 2. queue-len -> Length transaction queue. If queue is full client will stall\n \
			 3. ip-addr -> server's ip address\n \
			 4. portno -> server port number\n \
			 5. requer-rate -> time between two requests.\n \
			 		   If this is not -1, client will ignore timestamp of a transaction\n \
					   and will send request every <request-rate> time\n\
			                   If this is -1, client will issue request as per transaction timestamp\n.\
			 6. mult -> It converts the timestamp to other units\n.\
			 	    By default, timestamps are micro-second. This parameter is used to convert it ot other\n\
				    Timestamp is multuplied by this parameter. so if you want your timestamp\n \
				    ms then mult is 1000\n\
				    s then mult is 1000000\n\
			7. id -> Identifier for this client\n";
			return -EINVAL;
	}

	maxq = atoi(argv[2]);
	Q = new c_queue(maxq);
	if(Q == nullptr) {
		cr_log << "Client ran out of memory!" << endl;
		return -ENOMEM;
	}

	/* Create a context for parser thread. */
	read_ctx_t *r_ctx = new read_ctx_t;
	if(r_ctx == NULL) {
		cr_log << "Unable to creat reader context." << endl;
		delete Q;
		return -ENOMEM;
	}

	r_ctx->filename = argv[1];
	r_ctx->Q = Q;

	/* Spwan parser thread */
	thread t1(parse_file, r_ctx);

	/* Client will generate requests at the rate. */
	rate = atoi(argv[5]);

	/* Connector thread context */
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

	/* Spawn connector thread */
	thread t2(connector, w_ctx);
	t1.join();
	t2.join();
	delete r_ctx;
	delete w_ctx;
	delete Q;
	return 0;
}
