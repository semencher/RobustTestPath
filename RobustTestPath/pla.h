#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <ostream>

struct ElementP
{
	std::string input;
	std::string output;
};

class Pla
{
private:
	int inputs_;
	int outputs_;
	int states_;
	int products_;
	std::vector<ElementP> elements_;

private:
	bool validStringCube_(const std::string &interval);

public:
	Pla();
	bool parse(std::string fileName);
	void show(std::ostream &stream);

	int inputs();
	int outputs();
	int products();
	int states();
	std::vector<ElementP> elements();

}; // class Pla