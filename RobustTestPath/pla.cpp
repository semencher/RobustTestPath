#include "pla.h"

Pla::Pla() {
	inputs_ = -1;
	outputs_ = -1;
	products_ = -1;
	states_ = -1;
}

bool Pla::validStringCube_(const std::string &interval) {
	size_t size = interval.size();
	for (size_t i = 0; i < size; ++i) {
		if (interval[i] != '0' && interval[i] != '1' && interval[i] != '-' && interval[i] != '_')
			return false;
	}
	return true;
}

bool Pla::parse(std::string fileName) {
	std::ifstream file;
	file.open(fileName);
	if (!file.is_open()) {
		return false;
	}
	std::string word;

	if (file.eof()) {
		file.close();
		return false;
	}
	file >> word;
	while (word[0] == '.') {
		if (word == ".i") {
			file >> inputs_;
			file >> word;
		}
		if (word == ".s") {
			file >> states_;
			file >> word;
		}
		if (word == ".o") {
			file >> outputs_;
			file >> word;
		}
		if (word == ".p") {
			file >> products_;
			file >> word;
		}
		if (word == ".ilb") {
			file >> word;
			while (word[0] != '.' && !validStringCube_(word)) {
				file >> word;
			}
		}
		if (word == ".ob") {
			file >> word;
			while (word[0] != '.' && !validStringCube_(word)) {
				file >> word;
			}
		}
	}
	size_t i = 0;
	while (word != ".e" && i < products_&& !file.eof()) {
		ElementP element;
		element.input = word;
		file >> word;
		element.output = word;
		file >> word;
		elements_.push_back(element);
		++i;
	}
	file.close();
	return true;
}

void Pla::show(std::ostream &stream) {
	stream << "Start output pla...\n";
	stream << ".i " << inputs_ << "\n";
	stream << ".o " << outputs_ << "\n";
	stream << ".s " << states_ << "\n";
	stream << ".p " << products_ << "\n";

	for (auto iter = elements_.begin(); iter < elements_.end(); ++iter) {
		stream << iter->input << " " << iter->output << "\n";
	}
	stream << "Finish output pla...\n";
}

int Pla::inputs() {
	return inputs_;
}

int Pla::outputs() {
	return outputs_;
}

int Pla::products() {
	return products_;
}

int Pla::states() {
	return states_;
}

std::vector<ElementP> Pla::elements() {
	return elements_;
}