#include "server.h"

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

/*
 * Parse one line
 */
int parse_line(stringstream &ss, char *f, unordered_map<unsigned long, cr_rec_t *> *map)
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
			rec->cr_balance = stod(s, NULL);
			break;

			default:
			cr_log << "Record corrupted" << endl;
			delete rec;
			return -EINVAL;
			break;
		}
	}

	rec->cr_id = g_cust;
	map->insert(make_pair(rec->cr_account_nr, rec));
	print_cust_rec(rec, f);
	g_cust++;
	return 0;
}

/*
 * Parse a records file and put records in a map
 */
void parse_file(char *file, unordered_map<unsigned long, cr_rec_t *> *map)
{
	int      rc;
	ifstream fs;
	string   line;

	fs.open(file);
	if(fs.fail()) {
		rc = errno;
		cr_log << "Unable to open file:" << rc << ": " << file <<endl;
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
		parse_line(ss, file, map);
	}
	fs.close();
	return ;
}
