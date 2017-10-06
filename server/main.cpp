#include <stdlib.h>
#include <string.h>

#include <utility>
#include <unordered_map>
#include <string>
#include <iostream>

#include "server.h"
#include "util.h"
#include "logger.h"
#include "trans.h"

using namespace std;

void print_map(unordered_map<unsigned long, cr_rec_t *> *m)
{
	for(auto it = m->begin(); it != m->end(); ++it ) {
		cr_rec_t *rec = it->second;
		cr_log << "---------------------------------------------------------------" << endl;
		cr_log << "Customer id: " << rec->cr_id << endl;
		cr_log << "Account num:" << rec->cr_account_nr << endl;
		cr_log << "Name :" << rec->cr_name << endl;
		cr_log << "Balance :" << rec->cr_balance << endl;
		cr_log << "---------------------------------------------------------------" << endl;
	}
}


c_sock *listener(char *ip, int port)
{
	int rc;

	c_sock *ns = new c_sock;
	if(ns == NULL) {
		cr_log<<"Unable to open a socket:" << endl;
		return ns;
	}

	rc = ns->c_sock_addr(ip, port);
	if(rc != 0) {
		delete ns;
		cr_log << "Invalid Addresses" << endl;
		return NULL;
	}

	rc = ns->c_sock_bind();
	if(rc < 0) {
		delete ns;
		cr_log << "Unable to bind" << endl;
		return NULL;
	}

	ns->c_sock_listen();

	return ns;
}

int main(int argc, char *argv[])
{

	int rc;
	int rate, sec;

	unordered_map<unsigned long, cr_rec_t*> *map;
    	ios_base::sync_with_stdio(false); 
	setiosflags(ios::fixed);
	setprecision(2);

	map = new unordered_map<unsigned long, cr_rec_t*>;
	if(map == NULL) {
		cr_log << "Database built error" << endl;
		return -1;
	}

	parse_file(argv[1], map);
	if(map->empty()) {
		cr_log << "Database built error" << endl;
		return -1;
	}

	/* While t1 is building DB, let's complete bookkeeping for socket */
	c_sock *ns = listener(argv[2], atoi(argv[3]));
	if(ns == NULL) {
		cr_log << "unable to listen on a socket" << endl;
		return rc;
	}
	
	rate = atoi(argv[4]);
	sec = atoi(argv[5]);
	/* Interest calculator thread */
	int_thread_ctx_t *ctx = new int_thread_ctx_t;
	if(ctx == NULL) {
		cr_log << "Unable to allocate memory" << endl;
		delete ns;
	}

	ctx->rate = rate;
	ctx->sec = sec;
	ctx->map = map;

	if(rate != 0) {
		thread int_thread(interest_calc, ctx);
		int_thread.detach();
	}
	else
		delete ctx;
	/*
	 * Now we have at least few records to process.
	 * Accept connections from socket.
	 */
	while(1) {
		rc = ns->c_sock_accept();
		if(rc != 0) {
			cr_log << "Socket not connected:" << errno << endl;
			return rc;
		}
		int fd = ns->get_cl_sock();
		con_thread_ctx_t *ctx = new con_thread_ctx_t;
		if(ctx == NULL) {
			/* Handle, retry */
		}

		ctx->ns = ns;
		ctx->fd = fd;
		ctx->map = map;
		thread t1(transaction, ctx);
		t1.detach();
	}

	ns->c_sock_close();
	delete ns;
	delete map;
	return 0;
}
