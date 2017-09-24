#include <errno.h>
#include "util.h"
#include "logger.h"

bool is_string_num(string s)
{
	return find_if_not(s.begin(), s.end(), ::isdigit) != s.end();
}
int parse_file(char *file, int (*parse_line)(stringstream &ss, char *f))
{
	int      rc;
	ifstream fs;
	string   line;

	fs.open(file);
	if(fs.fail()) {
		rc = errno;
		cr_log << "Unable to open file:" << rc <<endl; 
		return rc;
	}

	while(getline(fs, line)) {
		if(fs.bad()) {
			rc = errno;
			fs.close();
			cr_log << "Error reading from a file: " << errno << endl;
			return rc;
		}
		stringstream ss(line);
		parse_line(ss, file);
	}
	fs.close();
	return 0;
}
