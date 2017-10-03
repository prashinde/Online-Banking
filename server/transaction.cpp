#include "server.h"

int CT_server(s_trans_t *t, cr_rec_t *cr)
{
	double interest = 0;
	unique_lock<mutex> lck(cr->cr_mx);
	switch(t->st_op) {
	case INTEREST:
		interest = (t->st_rate * cr->cr_balance)/100;
		cr->cr_balance += interest;
		cr_log << fixed << "Updated cr:" << cr->cr_id << "new balance: " << cr->cr_balance << endl;
		break;
	default:
		cr_log << "Operation not supported:" << endl;
		break;
	}
	lck.unlock();
}

int CT_client(c_trans_t *t, cr_rec_t *cr)
{
	unique_lock<mutex> lck(cr->cr_mx);
	switch(t->ct_op) {
	case WITHDRAW:
		if((cr->cr_balance - t->ct_amount) <= MIN_FUND) {
			t->ct_status = INS_FUND;
			cr_log << "Insufficient funds" << endl;
		} else {
			cr->cr_balance -= t->ct_amount;
			t->ct_status = 0;
		}
		break;
	case DEPOSIT:
		cr->cr_balance += t->ct_amount;
		t->ct_status = 0;
		break;
	default:
		t->ct_status = ERR;
		cr_log << "Impossible!!";
	}

	lck.unlock();
	cr_log << "Operation: " << t->ct_op << " Accound balance: " << cr->cr_balance << endl;
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

void transaction(c_sock *ns, int fd, unordered_map<unsigned long, cr_rec_t *> map)
{
	while(1) {
		c_trans_t *t = new c_trans_t;
		if(t == NULL) {
			/* How to handle response */
		}

		ssize_t exp = sizeof(c_trans_t);

		cr_log << "About to read from socket" << endl;
		ssize_t ret = ns->c_sock_read(fd, (void *)t, sizeof(c_trans_t));
		if(ret == -1) {
			cr_log << "Error on socket read:" << errno << endl;
			delete t;
			continue;
		}

		cr_log << "Read from socket success" << endl;
		if(ret != sizeof(c_trans_t)) {
			cr_log << "Partial read. Do something:" << endl;
			delete t;
			continue;
		}

		if(t->ct_timestamp == (0UL-1UL)) {
			cr_log << "Connection closing" << endl;
			delete t;
			break;
		}

		cr_log << "Not last recoed" << endl;
		print_trans(t, nullptr);
		unordered_map<unsigned long, cr_rec_t *>::const_iterator got = map.find(t->ct_acc_num);
		if(got == map.end()) {
			t->ct_status = NOT_CUST;
			cr_log << "Record does not exist..:" <<  t->ct_acc_num << endl;
			delete t;
			continue;
		}
		cr_log << "Record exist" << endl;

		trans_t *ct = new trans_t;
		if(ct == NULL) {
			/* Handle the failure. */
		}

		ct->e_t = CLIENT;
		ct->u.t_u_ct = t;
		CT(ct, got->second);
		cr_log << "Transaction done writing to socket" << endl;
		ret = ns->c_sock_write(fd, (void *)t, sizeof(c_trans_t));
		if(ret == -1) {
			cr_log << "Error on socket read:" << errno << endl;
			delete ct;
			delete t;
			continue;
		}
		delete ct;
		delete t;
	}
}
