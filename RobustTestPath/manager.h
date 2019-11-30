#pragma once

#include <string>

#include "data.h"

class Manager
{
private:
	Data data_;

public:
	Manager();

	bool readInputData(std::string fileKiss2, std::string directoryTest, std::string filePla);

};	// class Manager