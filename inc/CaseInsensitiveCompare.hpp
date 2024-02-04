#ifndef CASEINSENSITIVECOMPARE_HPP
# define CASEINSENSITIVECOMPARE_HPP

#include <string>
#include <algorithm>

class CaseInsensitiveCompare {
	private:
	public:
		bool operator()(std::string const &s1, std::string const &s2) const;
};

#endif