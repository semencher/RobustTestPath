#pragma once

#include <string>

#include "manager.h"

class API
{
private:
	Manager manager_;

public:
	API();

	bool readInputData(std::string fileKiss2, std::string directoryTest, std::string filePla);

};	// class API