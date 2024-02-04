#include "CaseInsensitiveCompare.hpp"

static unsigned char helper_tolower(unsigned char c) {
	if ('A' <= c && c <= 'Z') { return (c - 'A' + 'a'); }
	return (c);
}

bool CaseInsensitiveCompare::operator()(std::string const &s1, std::string const &s2) const {
	std::string str1(s1.length(), ' ');
	std::string str2(s2.length(), ' ');
	std::transform(s1.begin(), s1.end(), str1.begin(), helper_tolower);
	std::transform(s2.begin(), s2.end(), str2.begin(), helper_tolower);
	return (str1 < str2);
}