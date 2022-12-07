#include <iostream>
#include <string>
#include <exception>

class SyntaxError : std::exception {
public:
	SyntaxError(std::string errorMessage, int line, int position);
	void printMessage();
private:
	std::string errorMessage;
	int errorLine = -1;
	int errorPosition = -1;
};