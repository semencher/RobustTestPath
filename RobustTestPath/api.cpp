#include "api.h"

API::API() {

}

bool API::readInputData(std::string fileKiss2, std::string directoryTest, std::string filePla) {
	return manager_.readInputData(fileKiss2, directoryTest, filePla);
}