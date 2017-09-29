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

int start_transactions(c_queue *q, c_sock *ns, int rate)
{
	char buf[256];
	int wait = 0;
	while(1) {
		c_trans_t *t = q->remove_trans();

		cr_log << "Transaction read" << endl;
		cr_log << "Rate " << rate << endl;
		if(rate == 0)
			wait = t->ct_timestamp;
		else
			wait = rate;
		cr_log << "Wait " << wait << endl;

		if(wait != (0UL-1UL))
			sleep(wait);
		cr_log << "Wait over sendindg to server " << endl;
		ns->c_sock_write((void *)t, sizeof(c_trans_t));
		
		/*TODO: */
		if(wait == (0UL-1UL) || t->ct_timestamp == (0UL-1UL))
			break;

		cr_log << "Waiting from server" << endl;
		/* Let us wait of server to respond. */
		ssize_t ret = ns->c_sock_read((void *)t, sizeof(c_trans_t));
		if(ret == -1) {
			cr_log << "Error on socket read:" << errno << endl;
			delete t;
			continue;
		}

		if(ret != sizeof(c_trans_t)) {
			cr_log << "Partial read. Do something:" << endl;
			delete t;
			continue;
		}
		cr_log<<"Transaction: "<< t->ct_id << " Status" << t->ct_status << endl;
	}
	return 0;
}

void connector(c_queue *q, char *ip, int port, int rate)
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

	start_transactions(q, ns, rate);
	ns->c_sock_close();
	delete ns;
}
