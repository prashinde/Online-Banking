#ifndef __TRANS_H_
#define __TRANS_H_
#include <iostream>
#include <string>

#define MIN_FUND 0
enum operation {
	WITHDRAW = 1,
	DEPOSIT,
};

enum trans_st {
	ERR      = -1,
	INS_FUND = -2,
	NOT_CUST = -3,
};

typedef struct client_trans {
	unsigned long  ct_id;
	unsigned long  ct_timestamp;
	unsigned long  ct_acc_num;
	enum operation ct_op;
	unsigned long  ct_amount;
	int            ct_status; /* Server sets the flag. */
} c_trans_t;

void print_trans(c_trans_t *trans, char *f);
#endif
