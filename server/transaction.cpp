#include "server.h"

void CT(c_trans_t *t, cr_rec_t *r)
{
	unique_lock<mutex> lck(r->cr_mx);
	if(t->ct_op == WITHDRAW) {
		if((r->cr_balance - t->ct_amount) <= MIN_FUND) {
			t->ct_status = INS_FUND;
			cr_log << "Insufficient funds" << endl;
			lck.unlock();
			return ;
		}
		r->cr_balance -= t->ct_amount;

	} else if(t->ct_op == DEPOSIT) {
		r->cr_balance += t->ct_amount;
	} else {
		lck.unlock();
		cr_log << "Impossible!!";
		return ;
	}
	t->ct_status = 0;
	lck.unlock();
	cr_log << "Operation: " << t->ct_op << " Accound balance: " << r->cr_balance << endl;
}

void transaction(c_sock *ns, int fd, unordered_map<unsigned long, cr_rec_t *> map)
{
	while(1) {
		c_trans_t *t = new c_trans_t;

		ssize_t exp = sizeof(c_trans_t);
		ssize_t ret = ns->c_sock_read(fd, (void *)t, sizeof(c_trans_t));
		if(ret == -1) {
			cr_log << "Error on socket read:" << errno << endl;
			delete t;
			continue;
		}

		cout << "RET is: " << ret << endl;
		if(ret != sizeof(c_trans_t)) {
			cr_log << "Partial read. Do something:" << endl;
			delete t;
			continue;
		}

		if(t->ct_timestamp == (0UL-1UL)) {
			cr_log << "Connection closing" << endl;
			break;
		}

		print_trans(t, nullptr);
		unordered_map<unsigned long, cr_rec_t *>::const_iterator got = map.find(t->ct_acc_num);
		if(got == map.end()) {
			t->ct_status = NOT_CUST;
			cr_log << "Record does not exist..:" <<  t->ct_acc_num << endl;
			delete t;
			return ;
		}

		CT(t, got->second);
		delete t;
	}
}
