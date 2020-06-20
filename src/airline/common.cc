/*
 * Copyright (C) 2017 Rahul Jadhav <nyrahul@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU
 * General Public License v2. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     airline
 * @{
 *
 * @file
 * @brief       Common/utility APIs for Airline
 *
 * @author      Rahul Jadhav <nyrahul@gmail.com>
 *
 * @}
 */

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

map<string, string, ci_less> splitKV(string & s)
{
    map<string, string, ci_less> m;

    string key, val;
    istringstream iss(s);

    while(getline(getline(iss, key, '=') >> ws, val, ','))
        m[key] = val;
    return m;
}

/* Gets the string for the corr key and removes that map entry */
string getMapCfg(map<string, string, ci_less> & m, string key)
{
    string val;

    if (m.find(key) == m.end()) {
        return val;
    }
    val = m[key];
    m.erase(key);
    INFO << "param: " << key << "=" << val << "\n";
    return val;
}

