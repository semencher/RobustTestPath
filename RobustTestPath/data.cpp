#include "data.h"

Data::Data() {

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