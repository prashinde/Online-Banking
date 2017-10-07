#include "server.h"

/*
 * Handle the transactions coming from server e.g.
 * interest rate.
 */
int CT_server(s_trans_t *t, cr_rec_t *cr)
{
	double interest = 0;
	double o_balance;
	unique_lock<mutex> lck(cr->cr_mx);
	switch(t->st_op) {
	case INTEREST:
		interest = (t->st_rate * cr->cr_balance)/100;
		o_balance = cr->cr_balance;
		cr->cr_balance += interest;
		t->st_status = 0;
		t->st_o_balance = o_balance;
		t->st_n_balance = cr->cr_balance;
		break;
	default:
		cr_log << "Operation not supported:" << endl;
		break;
	}
	lck.unlock();
}

/*
 * Handles the transaction coming from client.
 * Withdraw and Deposit
 */
int CT_client(c_trans_t *t, cr_rec_t *cr)
{
	double o_balance;
	unique_lock<mutex> lck(cr->cr_mx);
	switch(t->ct_op) {
	case WITHDRAW:
		o_balance = cr->cr_balance;
		if((cr->cr_balance - t->ct_amount) < MIN_FUND) {
			t->ct_status = INS_FUND;
			cr_log << "Insufficient funds" << endl;
		} else {
			cr->cr_balance -= t->ct_amount;
			t->ct_status = 0;
		}

		t->ct_o_balance = o_balance;
		t->ct_n_balance = cr->cr_balance;
		break;
	case DEPOSIT:
		o_balance = cr->cr_balance;
		cr->cr_balance += t->ct_amount;
		t->ct_status = 0;
		t->ct_o_balance = o_balance;
		t->ct_n_balance = cr->cr_balance;
		break;
	default:
		t->ct_status = ERR;
		t->ct_o_balance = o_balance;
		t->ct_n_balance = cr->cr_balance;
		cr_log << "Impossible!!";
	}

	lck.unlock();
}

void CT(trans_t *t, cr_rec_t *r)
{
	if(t->e_t == SERVER) {
		CT_server(t->u.t_u_st, r);
	} else if(t->e_t == CLIENT) {
		CT_client(t->u.t_u_ct, r);
	} else {
		/* Impossible */
	}
}

void *transaction(void *ctx)
{
	con_thread_ctx_t *context = (con_thread_ctx_t *)ctx;
	c_sock *ns = context->ns;
	int fd = context->fd;
	unsigned long cnt = 0;

	unordered_map<unsigned long, cr_rec_t*> *map = context->map;
	while(1) {
		/* Read transaction from client */
		c_trans_t *t = new c_trans_t;
		if(t == NULL) {
			/* How to handle response */
		}

		ssize_t exp = sizeof(c_trans_t);
		ssize_t ret = ns->c_sock_read(fd, (void *)t, sizeof(c_trans_t));
		if(ret == -1) {
			cr_log << "Error on socket read:" << errno << endl;
			delete t;
			continue;
		}

		if(ret == 0) {
			cr_log << "Client closed the connection:" << endl;
			delete t;
			break;
		}

		/* If dummy record, break the loop, it is bye bye */
		if(t->ct_timestamp == (0UL-1UL)) {
			cr_log << "Connection closing fd:" << fd << endl;
			delete t;
			break;
		}

		cr_log << "Data recieved from client:" << context->c_id << endl;
		/* Lookup  the account number in the hashmap */
		unordered_map<unsigned long, cr_rec_t *>::const_iterator got = map->find(t->ct_acc_num);
		if(got == map->end()) {
			t->ct_status = NOT_CUST;
			cr_log << "Record does not exist..:" <<  t->ct_acc_num << endl;
			delete t;
			continue;
		}

		/* Create a transaction parent object */
		trans_t *ct = new trans_t;
		if(ct == NULL) {
			/* Handle the failure. */
			delete t;
			continue;
		}

		/* Assign child object */
		ct->e_t = CLIENT;
		ct->u.t_u_ct = t;
		CT(ct, got->second);
		
		cnt++;
		cr_log << "Operation:" << op_str(t->ct_op) << " cr:" << t->ct_acc_num << " Old Balance:" << t->ct_o_balance \
	 	<< " Amount:" << t->ct_amount << " New Balance:" << t->ct_n_balance << " status:" << t->ct_status << endl;

		/* Send reply back to the client */
		ret = ns->c_sock_write(fd, (void *)t, sizeof(c_trans_t));
		if(ret == -1) {
			cr_log << "Error on socket write:" << errno << endl;
			delete ct;
			delete t;
			continue;
		}
		delete ct;
		delete t;
	}

	print_map(map);
	close(fd);
	return NULL;
}
