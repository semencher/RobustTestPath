#include "utils.h"

#include <sstream>

bool Utils::validStringCube(const std::string &interval)
{
	size_t size = interval.size();
	for (size_t i = 0; i < size; ++i) {
		if (interval[i] != '0' && interval[i] != '1' && interval[i] != '-' && interval[i] != '_')
			return false;
	}
	return true;
}

std::vector<std::string> Utils::split(std::string s, char delim)
{
	std::vector<std::string> strings;
	for (std::istringstream ss(s); getline(ss, s, delim); strings.push_back(move(s)));
	return strings;
}