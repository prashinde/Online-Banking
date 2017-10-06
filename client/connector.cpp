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

int start_transactions(c_queue *q, c_sock *ns, int rate, int id, int mult)
{
	int wait = 0;
	time_t t = time(0);
	char perf_f[128] = "client_log";
	char unique_id[4];
	ofstream per_rec_f;

	snprintf(unique_id, 4, "%d", id);
	strncat(perf_f, unique_id, 4);

	cr_log << "File Name:" << perf_f << endl;
	per_rec_f.open(perf_f);

	if(!per_rec_f.is_open()) {
		cr_log<< "Log file open failed:" << errno << endl;
	}

	int nr_trans = 0;
	double total_time = 0;
	unsigned long ticks = 0;
	while(1) {
		c_trans_t *t = q->remove_trans();

		//cr_log << "Transaction read" << endl;
		//cr_log << "Rate " << rate << endl;
		if(rate == -1) {
			wait = t->ct_timestamp - ticks;
		} else
			wait = rate;
		//cr_log << "Wait " << wait << endl;

		if(wait != (0UL-1UL)) {
			usleep(wait*mult);
			ticks += wait;
		}
		struct timespec start, finish;
		double elapsed;
		clock_gettime(CLOCK_MONOTONIC, &start);
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

		clock_gettime(CLOCK_MONOTONIC, &finish);
		elapsed = (finish.tv_sec - start.tv_sec);
		elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
		total_time += elapsed;
		nr_trans++;
		per_rec_f <<"Transaction:" << op_str(t->ct_op) << " AccountNo:"<< t->ct_acc_num << " Status:" << t->ct_status \
		<<" Old Balance:" << t->ct_o_balance <<" Amount:"<< t->ct_amount << " New Balace:" << t->ct_n_balance << endl;
		cr_log << "Trans " << nr_trans << " complete " << "Elapsed: "<< elapsed << endl;
	}

	per_rec_f << "Average time per transaction:" << (total_time/nr_trans) << endl;
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

	start_transactions(q, ns, rate, w_ctx->id, w_ctx->mult);
	ns->c_sock_close();
	delete ns;
	return NULL;
}
