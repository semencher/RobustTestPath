#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <ostream>

struct Input
{
	std::string in;
	std::string state;
};

struct Output
{
	std::string out;
	std::string state;
};

struct Element
{
	Input input;
	Output output;
};

class Kiss2
{
private:
	int inputs_;
	int outputs_;
	int products_;
	int states_;
	std::string initialState_;

	std::vector<Element> elements_;

public:
	Kiss2();
	bool parse(std::string fileName);
	void show(std::ostream &stream);

	int inputs();
	int outputs();
	int products();
	int states();
	std::string initialState();
	std::vector<Element> elements();
}; // class Kiss2