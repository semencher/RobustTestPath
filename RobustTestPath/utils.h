#pragma once

#include <string>
#include <vector>

class Utils
{
public:
	static Utils & instance()
	{
		static Utils theSingleInstance;
		return theSingleInstance;
	}
private:
	Utils() {}
	Utils(const Utils& root) = delete;
	Utils& operator=(const Utils&) = delete;

private:

public:
	bool validStringCube(const std::string &interval);

	std::vector<std::string> split(std::string s, char delim);

};	// class Utils