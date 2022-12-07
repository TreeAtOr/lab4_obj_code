#include "Parser.h"
#include <iomanip>

void Parser::Parser_prog(string prog_text, set<string> dividers, set<string> keywords) {

	string symb = " ";	// текущий символ (не char потому что должен работать с set<string>)
	string lexem;	// слово лексема: кейворд, идентификатор, константа 
	int n_str = 1;	// счетчик номера строки
	int n_lex = 1;
	bool flag_const = false;//флаг константы  
	bool flag_id = false;//флаг идентификатора 
	bool flag_comm = false;//флаг комментариев 

	/**/


	int i = 0;	// счетчик символов
	while (symb[0] != '\0') {
		symb = prog_text[i];

		//_____________ снятие статуса комментария
		if (symb[0] == '\n' && lexem[0] == '/' && lexem[1] == '/') {
			flag_comm = false;
			symb = prog_text[i + 1];
			lexem = "";
			i++;
		}

		if (symb[0] == '*' && prog_text[i + 1] == '/' && lexem[0] == '/' && lexem[1] == '*') {
			flag_comm = false;
			symb = prog_text[i + 2];
			lexem = "";
			i++;
		}
		//_____________

		if ((dividers.find(lexem) != dividers.end() || dividers.find(symb) != dividers.end() || symb[0] == '\n' || symb[0] == '\t' || symb == " ") && !flag_comm) {	// делается проверка на разделитель
			flag_id = true;

			// Проверка на "двойной" divider когда == += <= и тд
			if (dividers.find(lexem) != dividers.end() && dividers.find(symb) != dividers.end() && dividers.find(lexem+symb) != dividers.end()) {
				flag_id = false;
				Push_back("DIVIDER", lexem+symb, n_lex, n_str);
				if (prog_text[i + 1] != ' ') lexem = prog_text[i + 1];
				else lexem = "";
				i+=2;
				continue;
			}
			// проверка на divider

			if (dividers.find(lexem) != dividers.end() && !flag_const) {
				flag_id = false;
				Push_back("DIVIDER", lexem, n_lex, n_str);
			}

			// делается проверка на keyword

			if (keywords.find(lexem) != keywords.end() && !flag_const) {
				flag_id = false;
				Push_back("KEYWORD", lexem, n_lex, n_str);
			}
			
			if (flag_const) {
				flag_id = false;
				Push_back("CONSTANT", lexem, n_lex, n_str);
				flag_const = false;

				n_lex++;
				Push_back("DIVIDER", symb, n_lex, n_str);
				lexem = "";
				i++;
				n_lex++;
				continue;
			} 

			if (lexem == "'" || lexem[0] == '\"' && !flag_const) {
				flag_id = false;
				flag_const = true;
			}

			if (Is_digit(lexem)) {
				flag_id = false;
				Push_back("CONSTANT", lexem, n_lex, n_str);
			}
			

			// проверка на идентификатор

			if (flag_id && lexem != "") {
				Push_back("IDENTIFIER", lexem, n_lex, n_str);
			}

			if (lexem != "")
				n_lex++;

			if (symb[0] != '\n' && symb[0] != '\t' && symb != " ") {
				lexem = symb;
			}
			else 
				lexem = "";
		}
		else 
			lexem += symb;

		//_________Наложение статуса комментария
		if (!flag_const && symb == "/" && (prog_text[i + 1] == '*' || prog_text[i + 1] == '/')) {
			flag_comm = true;
		}
		//_________

		if (symb[0] == '\n') {
			n_str++;	// проверка на новую строку
			n_lex = 1;
		}
		i++;

	}

}

void Parser::Push_back(string type, string volume, int a1, int a2) {
	Token t;
	t.type = type;
	t.value = volume;
	t.pos[0] = a1;
	t.pos[1] = a2;

	if (type == "IDENTIFIER") {
		Set_id(t);
		Find_Error(t);
	}

	if (type == "CONSTANT") Set_co(t);

	token.push_back(t);

}

void Parser::Find_Error(Token t) {
	if (Is_digit(t.value[0]))
	{
		system("cls");
		cout << "Identifier " << t.value << " has incorrect name!" << endl;
		system("pause");
		exit(1);
	}
}

vector<Token> Parser::getIdentifierTable()
{
	return tab_id;
}

vector<Token> Parser::getConstantTable()
{
	return tab_co;
}

vector<Token> Parser::getListOfTokens()
{
	return token;
}

void Parser::Set_id(Token t) {
	for (int i = 0; i < tab_id.size(); i++) {
		if (tab_id[i].value == t.value) return;
	}
	tab_id.push_back(t);
}

void Parser::Set_co(Token t) {
	for (int i = 0; i < tab_co.size(); i++) {
		if (tab_co[i].value == t.value) return;
	}
	tab_co.push_back(t);
}

bool Parser::Is_digit(char c) {
	if (c != '0' && c != '1' && c != '2' && c != '3' && c != '4' && c != '5' && c != '6' && c != '7' && c != '8' && c != '9') return false;
	return true;
}

bool Parser::Is_digit(string s) {
	int i = 0;
	if (s == "") return false;
	while (s[i] != '\0') {
		if (!Is_digit(s[i])) return false;
		i++;
	}
	return true;
}

void Parser::Write() {
	cout << "==============Lexems=============" << endl << endl;
	cout << "TYPE" << setw(8) << "|" << setw(11) << "NAME/VALUE" << " | " << setw(7) << "POS" << endl;
	cout << setw(12) << "|" << setw(13) << "|" << endl;
	for (int i = 0; i < token.size(); i++) {
		if (token[i].type == "IDENTIFIER")
			cout << token[i].type << setw(2) << "|" << setw(11) << token[i].value << " | " << "[" << setw(2) << token[i].pos[0] << ";" << setw(2) << token[i].pos[1] << "]";
		else if (token[i].type == "CONSTANT")
			cout << token[i].type << setw(4) << "|" << setw(11) << token[i].value << " | " << "[" << setw(2) << token[i].pos[0] << ";" << setw(2) << token[i].pos[1] << "]";
		else cout << token[i].type << setw(5) << "|" << setw(11) << token[i].value << " | " << "[" << setw(2) << token[i].pos[0] << ";" << setw(2) << token[i].pos[1] << "]";
		cout << endl;
	}


	cout << endl << "----CONSTANTS----" << endl << endl;
	cout << "#\t" << setw(9) << "Value" << endl << endl;
	for (int i = 0; i < tab_co.size(); i++) {
		cout << i << "\t" << setw(9) << tab_co[i].value << endl;
	}
	cout << endl << "---IDENTIFIERS---" << endl << endl;
	cout << "#\t" << setw(9) << "Name" << endl << endl;
	for (int i = 0; i < tab_id.size(); i++) {
		cout << i << "\t" << setw(9) << tab_id[i].value << endl;
	}
}