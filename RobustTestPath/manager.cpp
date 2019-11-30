#include "manager.h"

Manager::Manager() {

}

bool Manager::readInputData(std::string fileKiss2, std::string directoryTest, std::string filePla) {
	Data data;
	bool readStatus = data.readKiss2(fileKiss2) && data.readTestFileNames(directoryTest) && data.readPla(filePla);
	if (readStatus) {
		data_ = data;
		return true;
	}
	return false;
}