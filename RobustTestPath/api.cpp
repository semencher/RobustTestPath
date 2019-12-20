#include "api.h"

API::API() {

}

bool API::readInputData(std::string testFolder, std::string fileKiss2, std::string filePla) {
	return manager_.readInputData(testFolder, fileKiss2, filePla);
}

bool API::run() {
	return manager_.runMainAlgorithm();
}