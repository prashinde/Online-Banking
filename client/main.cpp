#include <stdlib.h>
#include <string.h>

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
	Q = new c_queue(maxq);
	if(Q == nullptr) {
		cr_log << "Client ran out of memory!" << endl;
		return -ENOMEM;
	}

	thread t1(parse_file, argv[1], Q);

	/* Client will generate requests at the rate. */
	rate = atoi(argv[5]);
	thread t2(connector, Q, argv[3], atoi(argv[4]), rate);
	t1.join();
	t2.join();
	delete Q;
	return 0;
}
