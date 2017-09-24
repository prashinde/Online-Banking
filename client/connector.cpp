#include "client.h"
/*class c_sock {
	struct sockaddr_in server_addr;
	int sock;
public:
	int c_sock_addr(char *ip, int port);
	int c_sock_connect();
	ssize_t c_sock_read(char *buffer, size_t len);
	ssize_t c_sock_write(char *buffer, size_t len);
	void c_sock_close();
};*/

int start_transactions(c_queue *q, c_sock *ns)
{
	char buf[256];
	while(1) {
		c_trans_t *t = q->remove_trans();
		ns->c_sock_write((void *)t, sizeof(c_trans_t));
		//ns->c_sock_read((void *)buf, 256);
		cr_log<<"Transaction:" << t->ct_id << " " << buf << endl;
	}
	return 0;
}

void connector(c_queue *q, char *ip, int port)
{
	int rc;

	c_sock *ns = new c_sock;
	if(ns == NULL) {
		cr_log<<"Unable to open a socket:" << endl;
		return;	
	}

	rc = ns->c_sock_addr(ip, port);
	if(rc != 0) {
		cr_log << "Invalid Addresses" << endl;
		return ;
	}

	rc = ns->c_sock_connect();
	if(rc != 0) {
		cr_log << "Socket not connected:" << errno << endl;
		return ;
	}

	start_transactions(q, ns);
	ns->c_sock_close();
	delete ns;
}
