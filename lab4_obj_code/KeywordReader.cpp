#include "KeywordReader.h"

std::set<std::string> KeywordReader(const char* keywordsFileName)
{
	std::ifstream keywordsFile; // файл с ключевыми словами
	std::string str; // строка для считывания из файла
	std::set<std::string> keywordsSet; // сет ключевых слов

	keywordsFile.open(keywordsFileName);

	while (std::getline(keywordsFile, str))
		keywordsSet.insert(str); // заполнение сета
	keywordsFile.close();

	return keywordsSet;
}