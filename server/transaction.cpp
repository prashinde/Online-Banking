#include "server.h"

void CT(c_trans_t *t, cr_rec_t *r)
{
	unique_lock<mutex> lck(r->cr_mx);
	if(t->ct_op == WITHDRAW) {
		r->cr_balance -= t->ct_amount;
	} else if(t->ct_op == DEPOSIT) {
		r->cr_balance += t->ct_amount;
	} else {
		cr_log << "Impossible!!";
		return ;
	}
	lck.unlock();
	cr_log << "Operation: " << t->ct_op << " Accound balance" << r->cr_balance;
	fflush(stdout);
}


void transaction(c_sock *ns, int fd, unordered_map<unsigned long, cr_rec_t *> map)
{
	while(1) {
		c_trans_t *t = new c_trans_t;
		ns->c_sock_read(fd, (void *)t, sizeof(t));

		unordered_map<unsigned long, cr_rec_t *>::const_iterator got = map.find(t->ct_acc_num);
		if(got == map.end()) {
			cr_log << "Record does not exist.." << endl;
			return ;
		}

		CT(t, got->second);
		delete t;
	}
}
