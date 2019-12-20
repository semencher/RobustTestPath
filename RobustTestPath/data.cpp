#include "data.h"

#include "utils.h"

#include <math.h>

Data::Data() {
	numTestFile_ = -1;
}

bool Data::readKiss2(std::string fileName) {
	Kiss2 kiss2;
	if (kiss2.parse(fileName)) {
		kiss2_ = kiss2;
		return true;
	}
	return false;
}

bool Data::readPla(std::string fileName) {
	Pla pla;
	if (pla.parse(fileName)) {
		pla_ = pla;
		return true;
	}
	return false;
}

bool Data::readTestFileNames(std::string directoryName) {
	WIN32_FIND_DATAA findData;
	HANDLE hf;

	hf = FindFirstFileA(directoryName.c_str(), &findData);

	if (hf == INVALID_HANDLE_VALUE) {
		return false;
	}
	do {
		if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			testFileNames_.push_back(findData.cFileName);
		}
	} while (FindNextFileA(hf, &findData));

	FindClose(hf);
	return true;
}

Kiss2 Data::kiss2() {
	return kiss2_;
}

std::vector<std::string> Data::testFileNames() {
	return testFileNames_;
}

Pla Data::pla() {
	return pla_;
}

std::string Data::testFolder() {
	return testFolder_;
}

void Data::setTestFolder(std::string folder) {
	testFolder_ = folder;
}

std::vector<TestPath> Data::currentTestPaths() {
	return testPaths_;
}

int Data::numTestFile() {
	return numTestFile_;
}

int Data::nextTestPaths() {
	if (testFileNames_.size() <= numTestFile_ + 1) {
		testPaths_.clear();
		return -2;
	}
	std::string fileName = testFolder_ + testFileNames_[numTestFile_ + 1];

	std::ifstream file;
	file.open(fileName);
	if (!file.is_open()) {
		testPaths_.clear();
		return -2;
	}
	std::string word;

	if (file.eof()) {
		file.close();
		testPaths_.clear();
		return -2;
	}

	file >> word;
	while (!file.eof()) {
		TestPath testPath;
		testPath.path_ = word;
		std::vector<std::string> elements = Utils::instance().split(testPath.path_, '<');
		testPath.output_ = std::abs(std::stoi(elements[0]));
		testPath.input_ = std::abs(std::stoi(elements[elements.size() - 1]));

		file >> word;
		if (word == "No") {
			file >> word;
			file >> word;
			continue; 
		}

		while (Utils::instance().validStringCube(word) && !file.eof()) {
			testPath.tests_.push_back(word);
			file >> word;
		}
		testPaths_.push_back(testPath);
	}
	numTestFile_++;
	return numTestFile_;
}