#pragma once

#include <string>

#include "data.h"
#include "algorithm.h"

class Manager
{
private:
	Data data_;

	Algorithm algorithm_;

public:
	Manager();

	bool readInputData(std::string testFolder, std::string fileKiss2, std::string filePla);

	bool runMainAlgorithm();

};	// class Manager