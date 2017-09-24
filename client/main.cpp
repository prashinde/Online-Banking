#include <stdlib.h>
#include <string.h>

#include <string>
#include <iostream>

#include "util.h"
#include "logger.h"
#include "trans.h"

using namespace std;

static unsigned long g_trans = 0;

int cli_parse(stringstream &ss, char *f)
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
	print_trans(trans, f);
	g_trans++;
	return 0;
}

int main(int argc, char *argv[])
{
	int rc;
	rc = parse_file(argv[1], cli_parse);
	if(rc != 0) {
		cout << "Error parsing a file" << endl;
		return rc;
	}

	return 0;
}
