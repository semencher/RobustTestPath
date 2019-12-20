#pragma once

#include <string>

#include "data.h"

class Manager
{
private:
	Data data_;

public:
	Manager();

	bool readInputData(std::string testFolder, std::string fileKiss2, std::string filePla);

	bool runMainAlgorithm();

};	// class Manager