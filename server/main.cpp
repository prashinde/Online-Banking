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

void print_map(unordered_map<unsigned long, cr_rec_t *> &m)
{
	for(auto it = m.begin(); it != m.end(); ++it ) {
		cout << " " << it->first << ":" << it->second;
		cout << endl;
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

	/* Spawn a thread to build database. */
	promise<unordered_map<unsigned long, cr_rec_t *>> p;
	auto f = p.get_future();

	thread t1(parse_file, argv[1], move(p));
	/* While t1 is building DB, let's complete bookkeeping for socket */
	c_sock *ns = listener(argv[2], atoi(argv[3]));
	if(ns == NULL) {
		cr_log << "unable to listen on a socket" << endl;
		t1.join();
		return rc;
	}

	/* Before falling through a dreadful blackhole, make sure
	 * databse is built succefully.
	 */
	t1.join();
	unordered_map<unsigned long, cr_rec_t*> m = f.get();
	if(m.empty()) {
		cr_log << "Database built error" << endl;
		ns->c_sock_close();
		delete ns;
		return -1;
	}

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
		thread(transaction, ns, fd, m).detach();
	}

	ns->c_sock_close();
	delete ns;
	return 0;
}
