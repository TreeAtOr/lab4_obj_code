#include "MultipleDefinitionError.h"

MultipleDefinitionError::MultipleDefinitionError(std::string errorMessage, int line, int position)
{
	MultipleDefinitionError::errorMessage = errorMessage;
	MultipleDefinitionError::errorLine = line;
	MultipleDefinitionError::errorPosition = position;

	// Someone is peace of ...
	int temp = errorLine;
	errorLine = errorPosition;
	errorPosition = temp;
}

void MultipleDefinitionError::printMessage()
{
	std::cout << "Error occured at line " << errorLine << " lexem number " << errorPosition << " : "
		<< errorMessage << "." << std::endl;
	return;
}
