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

#define INFO cout
#define ERROR cerr
#define WARN cerr

string& ltrim(string& s, const char* t = " \t\n\r\f\v");
string& rtrim(string& s, const char* t = " \t\n\r\f\v");
string& trim(string& s, const char* t = " \t\n\r\f\v");
vector<string> split(const string &s, char delim);

//#include <Config.h>
namespace wf
{
	class Config;
};
extern wf::Config WF_config;
#define	CFG(...)	WF_config.get(__VA_ARGS__)
#define	CFG_PANID	stoi(WF_config.get("panID"), nullptr, 0)
#define	SPAWN_STACKLINE(NODEID)	WF_config.spawnStackline(NODEID)
#define WF_STOP	raise(SIGINT)

#endif //_COMMON_H_
