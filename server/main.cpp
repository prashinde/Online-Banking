#include <stdlib.h>
#include <string.h>

#include <string>
#include <iostream>

#include "server.h"
#include "util.h"
#include "logger.h"
#include "trans.h"

using namespace std;

static unsigned long g_cust = 0;

static void print_cust_rec(cr_rec_t *rec, char *f)
{
	cr_log << "---------------------------------------------------------------" << endl;
	cr_log << "Customer details:" << f << endl;
	cr_log << "Customer id: " << rec->cr_id << endl;
	cr_log << "Account num:" << rec->cr_account_nr << endl;
	cr_log << "Name :" << rec->cr_name << endl;
	cr_log << "Balance :" << rec->cr_balance << endl;
	cr_log << "---------------------------------------------------------------" << endl;
}

int server_parse(stringstream &ss, char *f)
{
	string s;
	int i = 0;

	cr_rec_t *rec = new cr_rec_t;
	if(rec == NULL) {
		cr_log << "Out of memory!" << endl;
		return -ENOMEM;
	}

	while(getline(ss, s, ' ')) {
		i++;
		switch(i) {
			case 1:
			if(is_string_num(s)) {
				cr_log << "Record corrupted" << endl;
				delete rec;
				return -EINVAL;
			}

			rec->cr_account_nr = stol(s, NULL);
			break;

			case 2:
			rec->cr_name = s;	
			break;

			case 3:
			if(is_string_num(s)) {
				cr_log << "Record corrupted" << endl;
				delete rec;
				return -EINVAL;
			}
			rec->cr_balance = stol(s, NULL);
			break;

			default:
			cr_log << "Record corrupted" << endl;
			delete rec;
			return -EINVAL;
			break;
		}
	}

	rec->cr_id = g_cust; 
	print_cust_rec(rec, f);
	g_cust++;
	return 0;
}

int main(int argc, char *argv[])
{
	int rc;
	rc = parse_file(argv[1], server_parse);
	if(rc != 0) {
		cout << "Error parsing a file" << endl;
		return rc;
	}

	return 0;
}
