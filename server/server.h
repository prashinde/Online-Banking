#ifndef __CUSTOMER_H_
#define __CUSTOMER_H_

#include <string>
#include <mutex>

using namespace std;
typedef struct customer_record {
	unsigned long cr_id;
	unsigned long cr_account_nr;
	unsigned long cr_balance;
	string        cr_name;
	mutex         cr_mx;
} cr_rec_t;


#endif
