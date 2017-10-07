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

c_trans_t *cr_dummy()
{
	c_trans_t *trans = new c_trans_t;
	if(trans == NULL) {
		cr_log << "Unevitable occurs" << endl;
		return NULL;
	}

	trans->ct_timestamp = (0UL-1UL);
	return trans;
}

/*
 * Parse a space seperate line.
 */
int parse_line(stringstream &ss, char *f, c_queue *q)
{
	string s;
	int i = 0;

	c_trans_t *trans = new c_trans_t;
	if(trans == NULL) {
		cr_log << "Out of memory!" << endl;
		return -ENOMEM;
	}

	/* Parse a single line */
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
	/* Transaction is valid. Push it in a queue. */
	q->insert_trans(trans);
	g_trans++;
	return 0;
}

/*
 * Parse a file line by line.
 * Each valid transaction is put in a queue.
 */
void *parse_file(void *arg)
{
	int      rc;
	ifstream fs;
	string   line;

	read_ctx_t *ctx = (read_ctx_t *)arg;
	char *file = ctx->filename;
	c_queue *q = ctx->Q;

	fs.open(file);
	if(fs.fail()) {
		rc = errno;
		cr_log << "Unable to open file:" << file << ":" << rc <<endl; 
		return NULL;
	}

	/* Read line */
	while(getline(fs, line)) {
		if(fs.bad()) {
			rc = errno;
			fs.close();
			cr_log << "Error reading from a file: " << errno << endl;
			return NULL;
		}
		stringstream ss(line);
		parse_line(ss, file, q);
	}
	
	/* Dummy transaction to signal everyone to stop */
	if(fs.eof()) {
		q->insert_trans(cr_dummy());
	}

	fs.close();
	return NULL;
}
