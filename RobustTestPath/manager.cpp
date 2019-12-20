#include "manager.h"

Manager::Manager() {

}

bool Manager::readInputData(std::string testFolder, std::string fileKiss2, std::string filePla) {
	Data data;
	data.setTestFolder(testFolder);
	bool readStatus = data.readKiss2(fileKiss2) && data.readTestFileNames(testFolder + "*") && data.readPla(filePla);
	if (readStatus) {
		data_ = data;
		return true;
	}
	return false;
}

bool Manager::runMainAlgorithm() {
	data_.nextTestPaths();
	return true;
}