#include "ProgramReader.h"

std::string ProgramReader(const char* programFileName)
{
	std::string programStr; // строка для записи в нее текста программы
	std::ifstream programFile; // сам файл с текстом программы
	programFile.open(programFileName);
	std::getline(programFile, programStr, '\0'); // считываем в строку все символы до '\0'
	programFile.close();

	return programStr;
}