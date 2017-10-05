#ifndef __TRANS_H_
#define __TRANS_H_
#include <iostream>
#include <string>

using namespace std;
#define MIN_FUND 0
enum operation {
	WITHDRAW = 1,
	DEPOSIT,
	INTEREST,
};

enum evt_type {
	SERVER = 1,
	CLIENT,
};

enum trans_st {
	ERR      = -1,
	INS_FUND = -2,
	NOT_CUST = -3,
};

/*
 * Hanldes client side events.
 * withdrawl, deposit etc.
 */
typedef struct client_trans {
	unsigned long  ct_id;
	unsigned long  ct_timestamp;
	unsigned long  ct_acc_num;
	enum operation ct_op;
	unsigned long  ct_amount;
	/* Output parameters */
	double         ct_o_balance;
	double         ct_n_balance;
	int            ct_status; /* Server sets the flag. */
} c_trans_t;

/*
 * Handles server side events,
 * interest rate, late fees, overdraft fees.
 */
typedef struct server_trans {
	int            st_rate;
	enum operation st_op;
	/* Output parameters */
	double         st_o_balance;
	double         st_n_balance;
	int            st_status;
} s_trans_t;

typedef struct transaction {
	enum evt_type e_t;
	union {
		c_trans_t *t_u_ct;
		s_trans_t *t_u_st;
	} u;
} trans_t;

void print_trans(c_trans_t *trans, char *f);
string op_str(enum operation op);
#endif
