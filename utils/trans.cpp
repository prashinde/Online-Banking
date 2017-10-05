#include "trans.h"
#include "logger.h"
void print_trans(c_trans_t *trans, char *f)
{
	cr_log << "---------------------------------------------------------------" << endl;
	cr_log << "Transaction id: " << trans->ct_id << endl;
	cr_log << "Timestamp: " << trans->ct_timestamp << endl;
	cr_log << "Account num:" << trans->ct_acc_num << endl;
	cr_log << "Op type: " << trans->ct_op << endl;
	cr_log << "Amount :" << trans->ct_amount << endl;
	cr_log << "---------------------------------------------------------------" << endl;
}

string op_str(enum operation op)
{
	string ret;
	switch(op) {
	case WITHDRAW:
		ret = string("Withdraw");
		break;
	case DEPOSIT:
		ret = string("Deposite");
		break;
	case INTEREST:
		ret = string("Interest");
		break;
	}
	return ret;
}

