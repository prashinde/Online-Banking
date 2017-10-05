#include "server.h"
#include "trans.h"
#include "unistd.h"

/* Rate per seconds */
void *interest_calc(void *arg)
{
	int_thread_ctx_t *ctx = (int_thread_ctx_t *)arg;
	int rate = ctx->rate;
	int sec = ctx->sec;
	unordered_map<unsigned long, cr_rec_t *> *m = ctx->map;
	int t = 0;
	s_trans_t *st = new s_trans_t;
	if(st == NULL) {
		/* Interest thread stall. Retry */
	}

	while(1) {
		sleep(sec);
		for(auto it = m->begin(); it!= m->end(); ++it) {
			st->st_op = INTEREST;
			st->st_rate = rate;

			trans_t t;
			t.e_t = SERVER;
			t.u.t_u_st = st;
			CT(&t, it->second);
			double interest = st->st_n_balance-st->st_o_balance;
			cr_log << fixed << "Operation:" << op_str(st->st_op) << " cr:" << it->second->cr_account_nr << " Old Balance:" << \
			st->st_o_balance << " Amount:" << interest << " new balance:" << st->st_n_balance << endl;
		}
	}

	delete st;
	delete ctx;
	return NULL;
}
