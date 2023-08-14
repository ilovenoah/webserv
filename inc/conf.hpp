#ifndef CONF_HPP
#define CONF_HPP

#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <cstdlib>

enum ValueType
{
	INT,
	STRING,
	MAP,
	VECTOR
};

struct Value
{
	ValueType type;
	union {
		int intVal;
		std::string *strVal;
		std::map<std::string, int> *mapVal;
		std::vector<Value> *vctVal;
	} value;
};

std::map <std::string, Value> *parseConfig(int argc, const char *argv[]);

#endif
