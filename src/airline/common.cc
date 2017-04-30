#define _COMMON_CC_

#include <common.h>

// trim from left
string& ltrim(string& s, const char* t)
{
  s.erase(0, s.find_first_not_of(t));
  return s;
}

// trim from right
string& rtrim(string& s, const char* t)
{
  s.erase(s.find_last_not_of(t) + 1);
  return s;
}

// trim from left & right
string& trim(string& s, const char* t)
{
  return ltrim(rtrim(s, t), t);
}

template<typename Out>
void split(const string &s, char delim, Out result) {
	stringstream ss;
	ss.str(s);
	string item;
	while (getline(ss, item, delim)) {
		*(result++) = item;
	}
}

vector<string> split(const string &s, char delim) {
	vector<string> elems;
	split(s, delim, back_inserter(elems));
	return elems;
}

