#ifndef __CUSTOMER_H_
#define __CUSTOMER_H_

#include <string>
#include <mutex>
#include <sock.h>

#include <future>
#include <utility>
#include <unordered_map>
#include <string>
#include <iomanip>
#include <iostream>

#include "server.h"
#include "util.h"
#include "logger.h"
#include "trans.h"


using namespace std;
typedef struct customer_record {
	unsigned long cr_id;
	unsigned long cr_account_nr;
	double        cr_balance;
	string        cr_name;
	mutex         cr_mx;
} cr_rec_t;

void parse_file(char *file, promise<unordered_map<unsigned long, cr_rec_t *>> && map);
int parse_line(stringstream &ss, char *f, unordered_map<unsigned long, cr_rec_t *> &map);
void transaction(c_sock *ns, int fd, unordered_map<unsigned long, cr_rec_t *> map);
int interest_calc(int rate, int sec, unordered_map<unsigned long, cr_rec_t*> m);
#endif
