#pragma once

#include <string>

#include "manager.h"

class API
{
private:
	Manager manager_;

public:
	API();

	bool readInputData(std::string testFolder, std::string fileKiss2, std::string filePla);

	bool run();

};	// class API