#ifndef __UTIL_H_
#define __UTIL_H_
#include <string>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>

using namespace std;

int parse_file(char *file, int (*)(stringstream &, char *));
bool is_string_num(string s);
#endif
