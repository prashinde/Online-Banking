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

#define LOGNAME_FORMAT "log/%Y%m%d_%H%M%S"
#define LOGNAME_SIZE 20

int start_transactions(c_queue *q, c_sock *ns, int rate)
{
	int wait = 0;
	time_t t = time(0);
	char perf_f[128];
	strftime(perf_f, sizeof(perf_f), LOGNAME_FORMAT, localtime(&t));

	ofstream per_rec_f;

	per_rec_f.open(perf_f);

	while(1) {
		c_trans_t *t = q->remove_trans();

		//cr_log << "Transaction read" << endl;
		//cr_log << "Rate " << rate << endl;
		if(rate == 0)
			wait = t->ct_timestamp;
		else
			wait = rate;
		//cr_log << "Wait " << wait << endl;

		if(wait != (0UL-1UL))
			sleep(wait);
		//cr_log << "Wait over sendindg to server " << endl;
		ns->c_sock_write((void *)t, sizeof(c_trans_t));
		
		/*TODO: */
		if(wait == (0UL-1UL) || t->ct_timestamp == (0UL-1UL))
			break;

		//cr_log << "Waiting from server" << endl;
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
		per_rec_f <<"Transaction:" << op_str(t->ct_op) << " AccountNo:"<< t->ct_acc_num << " Status:" << t->ct_status \
		<<" Old Balance:" << t->ct_o_balance <<" Amount:"<< t->ct_amount << " New Balace:" << t->ct_n_balance << endl;
	}
	per_rec_f.close();
	return 0;
}

void * connector(void *arg)
{
	int rc;
	writer_ctx_t *w_ctx = (writer_ctx_t *)arg;

	c_queue *q = w_ctx->Q;
	char *ip = w_ctx->ip_addr;
	int port = w_ctx->port;
	int rate = w_ctx->rate;

	c_sock *ns = new c_sock;
	if(ns == NULL) {
		cr_log<<"Unable to open a socket:" << endl;
		return NULL;	
	}

	rc = ns->c_sock_addr(ip, port);
	if(rc != 0) {
		cr_log << "Invalid Addresses" << endl;
		return NULL;
	}

	rc = ns->c_sock_connect();
	if(rc != 0) {
		cr_log << "Socket not connected:" << errno << endl;
		return NULL;
	}

	start_transactions(q, ns, rate);
	ns->c_sock_close();
	delete ns;
	return NULL;
}
