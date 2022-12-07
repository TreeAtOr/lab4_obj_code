#include "MissedSemicolonError.h"

MissedSemicolonError::MissedSemicolonError(int line, int position)
{
	MissedSemicolonError::errorMessage = errorMessage;
	MissedSemicolonError::errorLine = line;
	MissedSemicolonError::errorPosition = position;
}

void MissedSemicolonError::printMessage()
{
	std::cout << "Error occured at line " << errorLine << " lexem number " << errorPosition << " : missed semicolon." << std::endl;
	return;
}