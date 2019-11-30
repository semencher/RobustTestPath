#include "kiss2.h"

Kiss2::Kiss2() {
	inputs_ = -1;
	outputs_ = -1;
	products_ = -1;
	states_ = -1;
	initialState_ = "";
}

bool Kiss2::parse(std::string fileName) {
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
	size_t size = 0;
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
		if (word == ".r") {
			file >> initialState_;
			file >> word;
		}
		if (word == ".p") {
			file >> products_;
			file >> word;
		}
		if (file.eof()) {
			file.close();
			return false;
		}
	}
	size_t i = 0;
	while (!file.eof() && i < products_) {
		ElementK element;
		element.input.in = word;
		file >> word;
		element.input.state = word;
		file >> word;
		element.output.state = word;
		file >> word;
		element.output.out = word;
		file >> word;
		elements_.push_back(element);
		++i;
	}
	file.close();
	return true;
}

void Kiss2::show(std::ostream &stream) {
	stream << "Start output kiss2...\n";
	stream << ".i " << inputs_ << "\n";
	stream << ".o " << outputs_ << "\n";
	stream << ".s " << states_ << "\n";
	stream << ".p " << products_ << "\n";
	stream << ".r " << initialState_ << "\n";

	for (auto iter = elements_.begin(); iter < elements_.end(); ++iter) {
		stream << iter->input.in << " " << iter->input.state << " " << iter->output.state << " " << iter->output.out << "\n";
	}
	stream << "Finish output kiss2...\n";
}

int Kiss2::inputs() {
	return inputs_;
}

int Kiss2::outputs() {
	return outputs_;
}

int Kiss2::products() {
	return products_;
}

int Kiss2::states() {
	return states_;
}

std::string Kiss2::initialState() {
	return initialState_;
}

std::vector<ElementK> Kiss2::elements() {
	return elements_;
}