#ifndef __CL_H_
#define __CL_H_

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

#include "sock.h"
#include "logger.h"
#include "trans.h"
using namespace std;

class c_queue {
	queue <c_trans_t *> Q;
	unsigned int q_max_size;
	mutex mtx;
	condition_variable pcv;	
	condition_variable ccv;
public:
	c_queue(unsigned int q_max_len);	
	int insert_trans(c_trans_t *item);
	c_trans_t *remove_trans();
};


void parse_file(char *file, c_queue *q);
void connector(c_queue *q, char *ip, int port, int rate);
#endif
