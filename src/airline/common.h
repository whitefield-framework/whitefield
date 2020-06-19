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

#ifndef _COMMON_H_
#define _COMMON_H_

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <iterator>
#include <string.h>
#include <unistd.h>
#include <signal.h>
using namespace std;

#define SUCCESS 0
#define FAILURE -1

#define INFO  cout << "INFO  "
#define ERROR cerr << "ERROR "
#define WARN  cerr << "WARN  "

string &ltrim(string &s, const char *t = " \t\n\r\f\v");
string &rtrim(string &s, const char *t = " \t\n\r\f\v");
string &trim(string &s, const char *t = " \t\n\r\f\v");

vector<string> split(const string &s, char delim);
struct ci_less
{
    // case-independent (ci) compare_less binary function
    struct nocase_compare
    {
        bool operator() (const unsigned char& c1, const unsigned char& c2) const {
            return tolower (c1) < tolower (c2);
        }
    };
    bool operator() (const std::string & s1, const std::string & s2) const {
        return std::lexicographical_compare
            (s1.begin (), s1.end (),   // source range
             s2.begin (), s2.end (),   // dest range
             nocase_compare ());  // comparison
    }
};
map<string, string, ci_less> splitKV(string & s);

//#include <Config.h>
namespace wf {
class Config;
};
extern wf::Config WF_config;

#define CFG(...)                WF_config.get(__VA_ARGS__)
#define CFG_INT(STR, DEF_VAL)   stoi(CFG(STR, #DEF_VAL), nullptr, 0)
#define CFG_PANID               stoi(CFG("panID", "0xface"), nullptr, 0)
#define SPAWN_STACKLINE(NODEID) WF_config.spawnStackline(NODEID)
#define WF_STOP                 raise(SIGINT)

static inline int stricmp(string s1, string s2)
{
    return strcasecmp(s1.c_str(), s2.c_str());
}

#endif //_COMMON_H_
