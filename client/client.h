#ifndef __CL_H_
#define __CL_H_

#include <thread>
#include <queue>
#include <mutex>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <condition_variable>

#include "sock.h"
#include "logger.h"
#include "trans.h"
using namespace std;

/* Transaction queue */
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

/* Parse thread context */
typedef struct reader_ctx {
	char    *filename;
	c_queue *Q;
} read_ctx_t;

/* Connector thread context */
typedef struct writer_ctx {
	c_queue *Q;
	int rate;
	char *ip_addr;
	int port;
	int id;
	int mult;
} writer_ctx_t;
void *parse_file(void *arg);
void *connector(void *arg);
#endif
