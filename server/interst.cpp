#include "server.h"
#include "trans.h"
#include "unistd.h"

/* Rate per seconds */
int interest_calc(int rate, int sec, unordered_map<unsigned long, cr_rec_t*> m)
{
	int t = 0;
	while(1) {
		sleep(sec);
		for(auto it = m.begin(); it!= m.end(); ++it) {
			/*cr_rec_t *cr = (it->second);
			unique_lock<mutex> lck(cr->cr_mx);
			double interest = (rate * cr->cr_balance)/100;
			cr->cr_balance += interest;
			t = cr->cr_balance;
			lck.unlock();
			cr_log << fixed << "Updated cr:" << cr->cr_id << "new balance: " << cr->cr_balance << endl;*/
			s_trans_t *st = new s_trans_t;
			if(st == NULL) {
				/* Interest thread stall. Retry */
			}
			st->st_rate = rate;
		
			trans_t t;
			t.e_t = SERVER;
			t.u.t_u_st = st;
			CT(&t, it->second);
		}
	}
	return 0;
}
