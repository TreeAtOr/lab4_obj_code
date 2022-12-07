#pragma once
#include <iostream>
#include <string>
using namespace std;
struct Token
{
	string type;	// DIVIDER KEYWORD IDENTIFIER CONSTANT типы токенов
	string value;	// значения токенов
	int pos[2];		// строка на которой расположен токен
};