#ifndef _COMMON_H_
#define _COMMON_H_

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

using namespace std;

#define SUCCESS 0
#define FAILURE -1

#define INFO std::cout
#define LOG std::cout
#define ERROR std::cerr
#define WARN std::cerr

string& ltrim(std::string& s, const char* t = " \t\n\r\f\v");
string& rtrim(std::string& s, const char* t = " \t\n\r\f\v");
string& trim(std::string& s, const char* t = " \t\n\r\f\v");

#endif //_COMMON_H_
