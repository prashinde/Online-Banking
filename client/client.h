#ifndef __CL_H_
#define __CL_H_

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

/* Socket */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> 
#include <arpa/inet.h>

#include <unistd.h>

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

class c_sock {
	struct sockaddr_in server_addr;
	int sock;
public:
	int c_sock_addr(char *ip, int port);
	int c_sock_connect();
	ssize_t c_sock_read(void *buffer, size_t len);
	ssize_t c_sock_write(void *buffer, size_t len);
	void c_sock_close();
};

void parse_file(char *file, c_queue *q);
void connector(c_queue *q, char *ip, int port);
#endif
