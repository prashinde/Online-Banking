#include <stdlib.h>
#include <string.h>

#include <string>
#include <iostream>

#include "client.h"
#include "util.h"
#include "logger.h"
#include "trans.h"

using namespace std;

static unsigned long g_trans = 0;

int parse_line(stringstream &ss, char *f, c_queue *q)
{
	string s;
	int i = 0;

	c_trans_t *trans = new c_trans_t;
	if(trans == NULL) {
		cr_log << "Out of memory!" << endl;
		return -ENOMEM;
	}

	while(getline(ss, s, ' ')) {
		i++;
		switch(i) {
			case 1:
			if(is_string_num(s)) {
				cr_log << "Transaction corrupted" << endl;
				delete trans;
				return -EINVAL;
			}

			trans->ct_timestamp = stol(s, NULL);
			break;

			case 2:
			if(is_string_num(s)) {
				cr_log << "Transaction corrupted" << endl;
				delete trans;
				return -EINVAL;
			}

			trans->ct_acc_num = stol(s, NULL);
			break;

			case 3:
			if(s.length() != 1) {
				cr_log << "Transaction corrupted" << endl;
				delete trans;
				return -EINVAL;
			}
			if((s.at(0) != 'w') && (s.at(0) != 'd')) {
				cr_log << "Transaction corrupted" << endl;
				delete trans;
				return -EINVAL;
			}

			trans->ct_op = (s.at(0) == 'w' ? WITHDRAW : DEPOSIT);
			break;

			case 4:
			if(is_string_num(s)) {
				cr_log << "Transaction corrupted" << endl;
				delete trans;
				return -EINVAL;
			}
			trans->ct_amount = stol(s, NULL);
			break;

			default:
			cr_log << "Transaction corrupted" << endl;
			delete trans;
			return -EINVAL;
			break;
		}
	}

	trans->ct_id = g_trans;
	q->insert_trans(trans);
	g_trans++;
	return 0;
}

void parse_file(char *file, c_queue *q)
{
	int      rc;
	ifstream fs;
	string   line;

	fs.open(file);
	if(fs.fail()) {
		rc = errno;
		cr_log << "Unable to open file:" << rc <<endl; 
		return ;
	}

	while(getline(fs, line)) {
		if(fs.bad()) {
			rc = errno;
			fs.close();
			cr_log << "Error reading from a file: " << errno << endl;
			return ;
		}
		stringstream ss(line);
		parse_line(ss, file, q);
	}
	fs.close();
	return ;
}
