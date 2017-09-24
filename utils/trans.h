#ifndef __TRANS_H_
#define __TRANS_H_
#include <iostream>
#include <string>

enum operation {
	WITHDRAW = 1,
	DEPOSIT,
};

typedef struct client_trans {
	unsigned long  ct_id;
	unsigned long  ct_timestamp;
	unsigned long  ct_acc_num;
	enum operation ct_op;
	unsigned long  ct_amount;
} c_trans_t;

void print_trans(c_trans_t *trans, char *f);
#endif
