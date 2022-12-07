#include "DividerReader.h"

std::set<std::string> DividerReader(const char* dividersFileName)
{
	std::ifstream dividersFile; // файл с разделителями
	std::string str; // строка для считывания из файла
	std::set<std::string> dividersSet; // сет разделителей 
 	
	dividersFile.open(dividersFileName);
	
	while (std::getline(dividersFile, str))
		dividersSet.insert(str); // заполнение сета
	dividersFile.close();

	return dividersSet;
}