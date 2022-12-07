#pragma once
#include <iostream>
#include <string>
#include <set>
#include <vector>
#include <iomanip>

#include "settings.h"
#include "Token.h"
#include "SyntaxError.h"
#include "MultipleDefinitionError.h"
#include "MissedSemicolonError.h"
#include "Tree.h"

using std::cout;
using std::endl;

class SyntaxParser {
public:

	SyntaxParser();
	~SyntaxParser();

	//TODO Must return tree and identifier table
	void parseTokens(vector<Token> listOfTockens);
	void printTree();
	void printIdentifiersTable();
	Tree* getTree() { return tree; };
private:

	vector<Token> listOfTokens;

	// <type, id>
	vector<std::pair<string, string>> identifiersTable;

	void addIdToIdTable(Token type, Token id);

	int numCurrentToken = -1;
	Token getNextToken();

	Tree* tree;

	vector<Tree*> listSavedTrees;
	vector<vector<pair<string, string>>> listSavedIdTables;
	int numLastSavedTree = -1;
	void saveTreeAndIdTable();
	void loadTreeAndIdTableWithIndex(int index);

	void deleteListSavedTrees();

	void throwSyntaxError(char* errorMessage, int line, int position);
	void throwSyntaxError(string& errorMessage, int line, int position);

	void printInDebugMode(const char* msg);
	void printTreeInDebug();

	// Non-terminal symbol functions
	void S(int indexParentNode);
	void mainProgram(int indexParentNode);
	void operatorsSequence(int indexParentNode);
	void functionsDefinition(int indexParentNode);
	void functionDefinition(int indexParentNode);
	void type(int indexParentNode);
	void passedParameters(int indexParentNode);
	void value(int indexParentNode);
	void parameter(int indexParentNode);
	void condition(int indexParentNode);
	void logicalOperators(ElementOfTree* fillThisNodeWithName);
	void comparison(ElementOfTree* fillValueOfThisNodeWithSign);
	void conditionOperator(int indexParentNode);
	void cycle(int indexParentNode);
	void _bool(int indexParentNode); // bool
	void definition(int indexParentNode);
	void assignment(int indexParentNode);
	void arythmeticalExpression(int indexParentNode);
	void sign(ElementOfTree* fillValueOfThisNodeWithSign);
	void operand(int indexParentNode);
	void function(int indexParentNode);
	// does not work with multiple arguments
	void functionArguments(int indexParentNode);
	// does not work with multiple conditions
	void conditionsSequence(int indexParentNode);
};