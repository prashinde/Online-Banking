#include "client.h"

/*
 * Read a single transaction from a queue and send it to the server.
 * Wait for the server's response. 
 * Once the server responds, log it and issue another transaction. 
 */
int start_transactions(c_queue *q, c_sock *ns, int rate, int id, int mult)
{
	int wait = 0;
	time_t t = time(0);
	char perf_f[128] = "client_log";
	char unique_id[4];
	ofstream per_rec_f;

	snprintf(unique_id, 4, "%d", id);
	strncat(perf_f, unique_id, 4);

	/* Create a log file to record performance readings. */
	per_rec_f.open(perf_f);

	if(!per_rec_f.is_open()) {
		cr_log<< "Log file open failed:" << errno << endl;
	}

	int nr_trans = 0;
	double total_time = 0;
	unsigned long ticks = 0;
	while(1) {

		c_trans_t *t = q->remove_trans();

		/* If rate parameter is -1, use timestamp to generate requests. */
		if(rate == -1) {
			wait = t->ct_timestamp - ticks;
		} else {
			wait = rate;
		}

		/* Wait for the timestamp. */
		if(wait != (0UL-1UL)) {
			usleep(wait*mult);
			ticks += wait;
		}

		/* Instrumentation: measure the time required for the transaction. */
		struct timespec start, finish;
		double elapsed;
		clock_gettime(CLOCK_MONOTONIC, &start);
		ssize_t ret = ns->c_sock_write((void *)t, sizeof(c_trans_t));
		if(ret == -1) {
			cr_log << "Cannot write to the socket:" << errno << endl;
			delete t;
			continue;
		}
		
		/* If dummy transaction, do not wait for the response. It is bye-bye */
		if(wait == (0UL-1UL) || t->ct_timestamp == (0UL-1UL)) {
			delete t;
			break;
		}

		/* Read the response */
		ret = ns->c_sock_read((void *)t, sizeof(c_trans_t));
		if(ret == -1) {
			cr_log << "Error on socket read:" << errno << endl;
			delete t;
			continue;
		}

		if(ret == 0) {
			cr_log << "Server ended the connection " << endl;
			delete t;
			break;
		}

		/* Instrimentation. Measure the time. */
		clock_gettime(CLOCK_MONOTONIC, &finish);
		elapsed = (finish.tv_sec - start.tv_sec);
		elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
		total_time += elapsed;
		nr_trans++;
		/* Log the status of the transaction. */
		per_rec_f <<fixed<<"Transaction:" << op_str(t->ct_op) << " AccountNo:"<< t->ct_acc_num << " Status:" << t->ct_status \
		<<" Old Balance:" << t->ct_o_balance <<" Amount:"<< t->ct_amount << " New Balace:" << t->ct_n_balance << endl;
		cr_log << "Trans " << nr_trans << " complete " << "Elapsed: "<< elapsed << endl;
	}

	per_rec_f << "Average time per transaction:" << (total_time/nr_trans) << endl;
	per_rec_f.close();
	return 0;
}

/*
 * It establishes a connection with the remote server over the socket.
 * If connection is not established, it will return an Error. 
 */
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
		delete ns;
		return NULL;
	}

	rc = ns->c_sock_connect();
	if(rc != 0) {
		cr_log << "Socket not connected:" << errno << endl;
		delete ns;
		return NULL;
	}

	start_transactions(q, ns, rate, w_ctx->id, w_ctx->mult);
	ns->c_sock_close();
	delete ns;
	return NULL;
}
