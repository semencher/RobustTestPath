#pragma once

#include <string>
#include <Windows.h>

#include "kiss2.h"
#include "pla.h"

struct TestPath
{
	int input_;
	int output_;
	std::string path_;
	std::vector<std::string> tests_;
};

class Data
{
private:
	Kiss2 kiss2_;
	std::vector<std::string> testFileNames_;
	Pla pla_;
	std::string testFolder_;

	std::vector<TestPath> testPaths_;
	int numTestFile_;		// -1 - считывание не начато. -2 - считывание завершено.

public:
	Data();

	bool readKiss2(std::string fileName);
	bool readPla(std::string fileName);
	bool readTestFileNames(std::string directoryName);

	Kiss2 kiss2();
	std::vector<std::string> testFileNames();
	Pla pla();
	std::string testFolder();
	void setTestFolder(std::string testFolder);

	std::vector<TestPath> currentTestPaths();
	int numTestFile();
	int nextTestPaths();

};	// class Data
