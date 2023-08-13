#ifndef CONF_HPP
#define CONF_HPP

#include <map>
#include <string>
#include <vector>

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

#endif
