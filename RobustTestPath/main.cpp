#include <iostream>

#include "api.h"
#include "pla.h"

int main()
{
	std::cout << "RobustTestPath running...\n";
	 
	API api;
	bool readStatus = api.readInputData("../InputData/s27/", "../InputData/s27.kiss2", "../InputData/s27.pla");
	
	bool runStatus = -1;
	if (readStatus) {
		runStatus = api.run();
	}

	int a = 0;
	std::cin >> a;

	return 0;
}