
#include <iostream>
#include <string>
#include <exception>

class MultipleDefinitionError : std::exception {
public:
	MultipleDefinitionError(std::string errorMessage, int line, int position);
	void printMessage();
private:
	std::string errorMessage;
	int errorLine = -1;
	int errorPosition = -1;
};