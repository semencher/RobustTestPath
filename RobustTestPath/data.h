#pragma once

#include <string>
#include <Windows.h>

#include "kiss2.h"
#include "pla.h"

class Data
{
private:
	Kiss2 kiss2_;
	std::vector<std::string> testFileNames_;
	Pla pla_;

public:
	Data();

	bool readKiss2(std::string fileName);
	bool readPla(std::string fileName);
	bool readTestFileNames(std::string directoryName);

	Kiss2 kiss2();
	std::vector<std::string> testFileNames();
	Pla pla();

};	// class Data
