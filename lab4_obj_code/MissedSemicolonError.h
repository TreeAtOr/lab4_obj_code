#include <iostream>
#include <string>
#include <exception>

class MissedSemicolonError : std::exception {
public:
	MissedSemicolonError(int line, int position);
	void printMessage();
private:
	std::string errorMessage;
	int errorLine = -1;
	int errorPosition = -1;
};