#include "SyntaxError.h"

SyntaxError::SyntaxError(std::string errorMessage, int line, int position)
{
	// Someone is peace of ...
	int temp = line;
	line = position;
	position = temp;

	SyntaxError::errorMessage = errorMessage;
	SyntaxError::errorLine = line;
	SyntaxError::errorPosition = position;
}

void SyntaxError::printMessage()
{
	std::cout << "Error occured at line " << errorLine << " lexem number " << errorPosition << " : " 
																	   << errorMessage << "." << std::endl;
	return ;
}
