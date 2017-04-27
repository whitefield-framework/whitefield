#ifndef _COMMON_H_
#define _COMMON_H_

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <map>
#include <string.h>
#include <unistd.h>
using namespace std;

#define SUCCESS 0
#define FAILURE -1

#define INFO cout
#define ERROR cerr
#define WARN cerr

string& ltrim(string& s, const char* t = " \t\n\r\f\v");
string& rtrim(string& s, const char* t = " \t\n\r\f\v");
string& trim(string& s, const char* t = " \t\n\r\f\v");

//#include <Config.h>
namespace wf
{
	class Config;
};
extern wf::Config WF_config;
#define	CFG(STR)	WF_config.get(STR)
#define	CFG_PANID	stoi(WF_config.get("panID"), nullptr, 0)
#define	SPAWN_STACKLINE(NODEID)	WF_config.spawnStackline(NODEID)

#endif //_COMMON_H_
