#include "SyntaxParser.h"

SyntaxParser::SyntaxParser()
{
	tree = new Tree();
}

SyntaxParser::~SyntaxParser()
{
	delete tree;
}

void SyntaxParser::parseTokens(vector<Token> listOfTockens)
{
	SyntaxParser::listOfTokens = listOfTockens;

	try {
		S(0);
		if (numCurrentToken != listOfTockens.size() - 1)
			throw SyntaxError("main is ok, error in functions definition", -1, -1);

		printTree();
		printIdentifiersTable();
		
	}
	catch (MultipleDefinitionError& err) {
		err.printMessage();
	}
	catch (SyntaxError& err) {
		err.printMessage();
	}
	catch (MissedSemicolonError& err) {
		err.printMessage();
	}
	deleteListSavedTrees();
		
}

void SyntaxParser::printTree(){
	tree->printTree();
	printf("\n\n");
	//tree->callPrintTreePromEnd();
}

void SyntaxParser::addIdToIdTable(Token typeToken, Token idToken)
{
	for (auto& typeIdPair: identifiersTable)
		if (typeIdPair.first == typeToken.value && typeIdPair.second == idToken.value) {
			string errorMsg = "This identifier >> " + typeToken.value + " " + idToken.value + " << was already defined";
			throw MultipleDefinitionError(errorMsg, idToken.pos[0], idToken.pos[1]);
		}
	identifiersTable.push_back(std::make_pair(typeToken.value, idToken.value));
}

void SyntaxParser::printIdentifiersTable()
{
	int counter = 0;
	cout << endl;
	cout << "==TABLE OF IDENTIFICATORS==" << endl;
	cout << "---------------------------" << endl;
	cout << "NUM |" << setw(8) << "TYPE" << " | " << setw(9) << "NAME" << " |" << endl;
	cout << "---------------------------" << endl;
	for (auto& typeIdPair : identifiersTable) {
		cout << setw(3) << counter << " | " << setw(7) << typeIdPair.first << " | " << setw(9) << typeIdPair.second << " |" << endl;
		counter++;
	}
	cout << "---------------------------" << endl;
	cout << endl;
}

Token SyntaxParser::getNextToken()
{
	numCurrentToken += 1;
	if (numCurrentToken >= listOfTokens.size())
		throw SyntaxError("End of tokens list", -1, -1);
	return listOfTokens[numCurrentToken];
}

void SyntaxParser::saveTreeAndIdTable()
{
	Tree* tr = new Tree(*tree);
	listSavedTrees.push_back(tr);
	numLastSavedTree++;

	listSavedIdTables.push_back(identifiersTable);
}

void SyntaxParser::loadTreeAndIdTableWithIndex(int index)
{
	Tree* tr;
	for (int i = listSavedTrees.size() - 1; i > index; i--) {
		delete listSavedTrees[i];
		listSavedTrees.pop_back();
		numLastSavedTree--;
	}

	for (int i = listSavedIdTables.size() - 1; i > index; i--) {
		listSavedIdTables.pop_back();
	}

	delete tree;
	tree = new Tree(*listSavedTrees[index]);
	identifiersTable = listSavedIdTables[index];
}

void SyntaxParser::deleteListSavedTrees()
{
	int lastIndex = listSavedTrees.size();
	for (int i = lastIndex - 1; i >= 0; i--) {
		delete listSavedTrees[i];
		listSavedTrees.pop_back();
	}
}

void SyntaxParser::throwSyntaxError(char* errorMessage, int line, int position)
{
	std::string message(errorMessage);
	throwSyntaxError(message, line, position);
}

void SyntaxParser::throwSyntaxError(string& errorMessage, int line, int position)
{
	throw SyntaxError(errorMessage, line, position);
}

void SyntaxParser::S(int indexParentNode)
{
	printInDebugMode("S");

	int savedNumToken = numCurrentToken;
	saveTreeAndIdTable();
	int indexSavedTree = numLastSavedTree;
	try {
		ElementOfTree* node = new ElementOfTree(tree->getAmountElements(), "program");
		tree->addElementInTree(node, indexParentNode);
		mainProgram(tree->getAmountElements() - 1);
		functionsDefinition(0);
		return;
	}
	catch (SyntaxError&) {
		loadTreeAndIdTableWithIndex(indexSavedTree);
	}

	numCurrentToken = savedNumToken;
	try {
		ElementOfTree* node = new ElementOfTree(tree->getAmountElements(), "program");
		tree->addElementInTree(node, indexParentNode);
		mainProgram(tree->getAmountElements() - 1);
	}
	catch (SyntaxError& err) {
		err.printMessage();
		exit(-1);
	}
	
}

void SyntaxParser::mainProgram(int indexParentNode)
{
	printInDebugMode("mainProgram");

	std::string errorMessage("Error in main program");
	Token token;

	token = getNextToken();
	if (token.type != "KEYWORD" || token.value != "main")
		throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);
	token = getNextToken();
	if (token.type != "DIVIDER" || token.value != "(")
		throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);
	token = getNextToken();
	if (token.type != "DIVIDER" || token.value != ")")
		throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);
	token = getNextToken();
	if (token.type != "DIVIDER" || token.value != "{")
		throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);
	operatorsSequence(indexParentNode);
	token = getNextToken();
	if (token.type != "DIVIDER" || token.value != "}")
		throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);
}

void SyntaxParser::operatorsSequence(int indexParentNode)
{
	printInDebugMode("operatorsSequence");

	bool flag_convolution_worked_on_something = false;
	int savedNumToken = numCurrentToken;
	int indexSavedTree;

	saveTreeAndIdTable();
	indexSavedTree = numLastSavedTree;
	try {
		ElementOfTree* node = new ElementOfTree(tree->getAmountElements(), "assignment");
		tree->addElementInTree(node, indexParentNode);
		assignment(tree->getAmountElements() - 1);

		flag_convolution_worked_on_something = true;
		savedNumToken = numCurrentToken;
		saveTreeAndIdTable();
		indexSavedTree = numLastSavedTree;
	}
	catch (SyntaxError&) {
		numCurrentToken = savedNumToken;
		loadTreeAndIdTableWithIndex(indexSavedTree);
	}
	printTreeInDebug();

	try {
		ElementOfTree* node = new ElementOfTree(tree->getAmountElements(), "definition");
		tree->addElementInTree(node, indexParentNode);
		definition(tree->getAmountElements() - 1);

		flag_convolution_worked_on_something = true;
		savedNumToken = numCurrentToken;
		saveTreeAndIdTable();
		indexSavedTree = numLastSavedTree;
	}
	catch (SyntaxError&) {
		numCurrentToken = savedNumToken;
		loadTreeAndIdTableWithIndex(indexSavedTree);
	}
	printTreeInDebug();

	try {
		ElementOfTree* node = new ElementOfTree(tree->getAmountElements(), "func_call");
		tree->addElementInTree(node, indexParentNode);
		function(tree->getAmountElements() - 1);

		flag_convolution_worked_on_something = true;
		savedNumToken = numCurrentToken;
		saveTreeAndIdTable();
		indexSavedTree = numLastSavedTree;
	}
	catch (SyntaxError&) {
		numCurrentToken = savedNumToken;
		loadTreeAndIdTableWithIndex(indexSavedTree);
	}
	printTreeInDebug();

	try {
		ElementOfTree* node = new ElementOfTree(tree->getAmountElements(), "if");
		tree->addElementInTree(node, indexParentNode);
		conditionOperator(tree->getAmountElements() - 1);

		flag_convolution_worked_on_something = true;
		savedNumToken = numCurrentToken;
		saveTreeAndIdTable();
		indexSavedTree = numLastSavedTree;
	}
	catch (SyntaxError&) {
		numCurrentToken = savedNumToken;
		loadTreeAndIdTableWithIndex(indexSavedTree);
	}
	printTreeInDebug();

	try {
		ElementOfTree* node = new ElementOfTree(tree->getAmountElements(), "while");
		tree->addElementInTree(node, indexParentNode);
		cycle(tree->getAmountElements() - 1);

		flag_convolution_worked_on_something = true;
		savedNumToken = numCurrentToken;
		saveTreeAndIdTable();
		indexSavedTree = numLastSavedTree;
	}
	catch (SyntaxError&) {
		numCurrentToken = savedNumToken;
		loadTreeAndIdTableWithIndex(indexSavedTree);
	}
	printTreeInDebug();

	if (flag_convolution_worked_on_something == false)
		throw SyntaxError("error in operators sequence", -1, -1);

	try
	{
		operatorsSequence(indexParentNode);
	}
	catch (SyntaxError&){
		loadTreeAndIdTableWithIndex(indexSavedTree);
		printTreeInDebug();
	}
	
}

void SyntaxParser::functionsDefinition(int indexParentNode)
{
	printInDebugMode("functionsDefinition");

	int savedNumToken = numCurrentToken;
	saveTreeAndIdTable();
	int indexSavedTree = numLastSavedTree;
	try {
		ElementOfTree* node = new ElementOfTree(tree->getAmountElements(), "function");
		tree->addElementInTree(node, indexParentNode);
		functionDefinition(tree->getAmountElements() - 1);
		return;
	}
	catch (SyntaxError&) {
		loadTreeAndIdTableWithIndex(numLastSavedTree);
		numCurrentToken = savedNumToken;

		ElementOfTree* node = new ElementOfTree(tree->getAmountElements(), "function");
		tree->addElementInTree(node, indexParentNode);
		functionDefinition(tree->getAmountElements() - 1);
		functionsDefinition(indexParentNode);
	}
}

void SyntaxParser::functionDefinition(int indexParentNode)
{
	printInDebugMode("functionDefinition");

	std::string errorMessage("Error in functionDefinition");
	Token token;

	type(indexParentNode);

	token = getNextToken();
	if (token.type != "KEYWORD" || token.value != "proc")
		throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);

	token = getNextToken();
	if (token.type != "IDENTIFIER")
		throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);
	
	// add id to tree
	ElementOfTree* node = new ElementOfTree(tree->getAmountElements(), token.value);
	tree->addElementInTree(node, indexParentNode);

	addIdToIdTable(listOfTokens[numCurrentToken - 2], token);

	token = getNextToken();
	if (token.type != "DIVIDER" || token.value != "(")
		throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);

	node = new ElementOfTree(tree->getAmountElements(), "params");
	tree->addElementInTree(node, indexParentNode);
	passedParameters(tree->getAmountElements() - 1);

	token = getNextToken();
	if (token.type != "DIVIDER" || token.value != ")")
		throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);
	token = getNextToken();
	if (token.type != "DIVIDER" || token.value != "{")
		throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);

	node = new ElementOfTree(tree->getAmountElements(), "body");
	tree->addElementInTree(node, indexParentNode);
	operatorsSequence(tree->getAmountElements() - 1);

	token = getNextToken();
	if (token.type != "KEYWORD" || token.value != "return")
		throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);

	node = new ElementOfTree(tree->getAmountElements(), "ret");
	tree->addElementInTree(node, indexParentNode);
	value(tree->getAmountElements() - 1);

	token = getNextToken();
	if (token.type != "DIVIDER" || token.value != ";") {
		Token prevToken = listOfTokens[numCurrentToken - 1];
		throw MissedSemicolonError(prevToken.pos[1], prevToken.pos[0]);
	}

	token = getNextToken();
	if (token.type != "DIVIDER" || token.value != "}")
		throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);

}

void SyntaxParser::type(int indexParentNode)
{
	printInDebugMode("type");

	std::string errorMessage("No such type");
	Token token;

	token = getNextToken();
	if (token.type != "KEYWORD" || (token.value != "bool" && token.value != "integer" && token.value != "string"))
		throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);

	ElementOfTree* node = new ElementOfTree(tree->getAmountElements(), token.value);
	tree->addElementInTree(node, indexParentNode);

}

void SyntaxParser::passedParameters(int indexParentNode)
{
	printInDebugMode("passedParameters");

	std::string errorMessage("Error in passing parameters");
	Token token;

	int savedNumToken = numCurrentToken;
	saveTreeAndIdTable();
	int indexSavedTree = numLastSavedTree;
	try {
		ElementOfTree* node = new ElementOfTree(tree->getAmountElements(), "param");
		tree->addElementInTree(node, indexParentNode);
		parameter(tree->getAmountElements() - 1);
		return;
	}
	catch (SyntaxError&) {
		numCurrentToken = savedNumToken;
		loadTreeAndIdTableWithIndex(indexSavedTree);

		ElementOfTree* node = new ElementOfTree(tree->getAmountElements(), "param");
		tree->addElementInTree(node, indexParentNode);
		parameter(tree->getAmountElements() - 1);
		token = getNextToken();
		if (token.type != "DIVIDER" || token.value != ",")
			throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);
		passedParameters(indexParentNode);
	}
}

void SyntaxParser::value(int indexParentNode)
{
	printInDebugMode("value");

	std::string errorMessage("Wrong value");
	Token token;

	int savedNumToken = numCurrentToken;
	saveTreeAndIdTable();
	int indexSavedTree = numLastSavedTree;

	try {
		ElementOfTree* node = new ElementOfTree(tree->getAmountElements(), "function");
		tree->addElementInTree(node, indexParentNode);
		function(tree->getAmountElements() - 1);
		return;
	}
	catch (SyntaxError&) {
		loadTreeAndIdTableWithIndex(indexSavedTree);
	}

	numCurrentToken = savedNumToken;
	try {
		ElementOfTree* node = new ElementOfTree(tree->getAmountElements(), "expr");
		tree->addElementInTree(node, indexParentNode);
		arythmeticalExpression(tree->getAmountElements() - 1);
		return;
	}
	catch (SyntaxError&) {
		loadTreeAndIdTableWithIndex(indexSavedTree);
	}

	try {
		numCurrentToken = savedNumToken;
		token = getNextToken();
		if (token.type != "IDENTIFIER" && token.type != "CONSTANT" && (token.type != "KEYWORD" || (token.value != "true" && token.value != "false")))
			throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);
		ElementOfTree* node = new ElementOfTree(tree->getAmountElements(), token.value);
		tree->addElementInTree(node, indexParentNode);
		return;
	}
	catch (SyntaxError&) {
		loadTreeAndIdTableWithIndex(indexSavedTree);
	}

	numCurrentToken = savedNumToken;
	token = getNextToken();
	if (token.type != "DIVIDER" || token.value != "\"")
		throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);
	token = getNextToken();
	if (token.type != "CONSTANT" && token.type != "KEYWORD")
		throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);
	ElementOfTree* node = new ElementOfTree(tree->getAmountElements(), token.value);
	tree->addElementInTree(node, indexParentNode);
	token = getNextToken();
	if (token.type != "DIVIDER" || token.value != "\"")
		throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);
}

void SyntaxParser::parameter(int indexParentNode)
{
	printInDebugMode("parameter");

	std::string errorMessage("Wrong parameter in function");
	Token token;

	int savedNumToken = numCurrentToken;
	int indexSavedTree = numLastSavedTree;
	try {
		type(indexParentNode);

		token = getNextToken();
		if (token.type != "IDENTIFIER")
			throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);

		ElementOfTree* node = new ElementOfTree(tree->getAmountElements(), token.value);
		tree->addElementInTree(node, indexParentNode);
	}
	catch (SyntaxError&) {
		loadTreeAndIdTableWithIndex(indexSavedTree);
		numCurrentToken = savedNumToken;

		type(indexParentNode);
	}
}

void SyntaxParser::condition(int indexParentNode)
{
	printInDebugMode("condition");

	std::string errorMessage("Wrong condition");
	Token token;

	int savedNumToken = numCurrentToken;
	saveTreeAndIdTable();
	int indexSavedTree = numLastSavedTree;

	try {
		token = getNextToken();
		if (token.type != "IDENTIFIER")
			throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);
		ElementOfTree* idNode = new ElementOfTree(tree->getAmountElements(), token.value);

		int indexComparisonNode = tree->getAmountElements();
		ElementOfTree* comparisonNode = new ElementOfTree(indexComparisonNode, "");
		tree->addElementInTree(comparisonNode, indexParentNode);
		comparison(comparisonNode);
		tree->addElementInTree(idNode, indexComparisonNode);

		value(indexComparisonNode);
		return;
	}
	catch (SyntaxError&) {
		loadTreeAndIdTableWithIndex(indexSavedTree);
	}

	numCurrentToken = savedNumToken;
	try {
		int indexComparisonNode = tree->getAmountElements();
		ElementOfTree* comparisonNode = new ElementOfTree(indexComparisonNode, "");
		tree->addElementInTree(comparisonNode, indexParentNode);

		value(indexComparisonNode);
		comparison(comparisonNode);
		value(indexComparisonNode);
		return;
	}
	catch (SyntaxError&) {
		loadTreeAndIdTableWithIndex(indexSavedTree);
	}

	numCurrentToken = savedNumToken;
	_bool(indexParentNode);
}

void SyntaxParser::logicalOperators(ElementOfTree* fillThisNodeWithName)
{
	printInDebugMode("logicalOperators");

	std::string errorMessage("Wrong logical operator");
	Token token;

	token = getNextToken();
	if (token.type != "DIVIDER" || (token.value != "&&" && token.value != "||"))
		throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);
	fillThisNodeWithName->name = token.value;

}

void SyntaxParser::comparison(ElementOfTree* fillValueOfThisNodeWithSign)
{
	printInDebugMode("comparison");

	std::string errorMessage("Wrong comparison operator");
	Token token;

	token = getNextToken();
	if (token.type != "DIVIDER" || (token.value != "==" && token.value != "!=" && token.value != "<"
		&& token.value != ">" && token.value != "<=" && token.value != ">="))
		throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);
	fillValueOfThisNodeWithSign->name = token.value;
}

void SyntaxParser::conditionOperator(int indexParentNode)
{
	printInDebugMode("conditionOperator");

	std::string errorMessage("Wrong if-then-else-endif statement");
	Token token;

	token = getNextToken();
	if (token.type != "KEYWORD" || token.value != "if")
		throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);

	ElementOfTree* node = new ElementOfTree(tree->getAmountElements(), "cond");
	tree->addElementInTree(node, indexParentNode);
	condition(tree->getAmountElements() - 1);

	token = getNextToken();
	if (token.type != "KEYWORD" || token.value != "then")
		throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);


	node = new ElementOfTree(tree->getAmountElements(), "then");
	tree->addElementInTree(node, indexParentNode);
	operatorsSequence(tree->getAmountElements() - 1);

	token = getNextToken();
	if (token.type != "KEYWORD" || token.value != "else")
		throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);

	node = new ElementOfTree(tree->getAmountElements(), "else");
	tree->addElementInTree(node, indexParentNode);
	operatorsSequence(tree->getAmountElements() - 1);

	token = getNextToken();
	if (token.type != "KEYWORD" || token.value != "endif")
		throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);

	token = getNextToken();
	if (token.type != "DIVIDER" || token.value != ";")
		throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);
}

void SyntaxParser::cycle(int indexParentNode)
{
	printInDebugMode("cycle");

	std::string errorMessage("Wrong while-do-endwhile statement");
	Token token;

	token = getNextToken();
	if (token.type != "KEYWORD" || token.value != "while")
		throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);

	ElementOfTree* node = new ElementOfTree(tree->getAmountElements(), "cond");
	tree->addElementInTree(node, indexParentNode);
	conditionsSequence(tree->getAmountElements() - 1);

	token = getNextToken();
	if (token.type != "DIVIDER" || token.value != ";")
		throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);

	node = new ElementOfTree(tree->getAmountElements(), "body");
	tree->addElementInTree(node, indexParentNode);
	operatorsSequence(tree->getAmountElements() - 1);

	token = getNextToken();
	if (token.type != "KEYWORD" || token.value != "endwhile")
		throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);

	token = getNextToken();
	if (token.type != "DIVIDER" || token.value != ";")
		throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);
}

void SyntaxParser::_bool(int indexParentNode)
{
	printInDebugMode("bool");

	std::string errorMessage("Wrong bool value");
	Token token;

	token = getNextToken();
	if (token.type != "KEYWORD" || (token.value != "true" && token.value != "false"))
		throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);
	ElementOfTree* node = new ElementOfTree(tree->getAmountElements(), token.value);
	tree->addElementInTree(node, indexParentNode);
}

void SyntaxParser::definition(int indexParentNode)
{
	printInDebugMode("definition");

	std::string errorMessage("wrong definition");
	Token token;

	type(indexParentNode);

	token = getNextToken();
	if (token.type != "IDENTIFIER")
		throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);

	ElementOfTree* node = new ElementOfTree(tree->getAmountElements(), token.value);
	tree->addElementInTree(node, indexParentNode);


	token = getNextToken();
	if (token.type != "DIVIDER" || token.value != ";") {
		Token prevToken = listOfTokens[numCurrentToken - 1];
		throw MissedSemicolonError(prevToken.pos[1], prevToken.pos[0]);
	}

	addIdToIdTable(listOfTokens[numCurrentToken - 2], listOfTokens[numCurrentToken - 1]);

}

void SyntaxParser::assignment(int indexParentNode)
{
	printInDebugMode("assignment");

	std::string errorMessage("wrong assignment of value");
	Token token;

	token = getNextToken();
	if (token.type != "IDENTIFIER")
		throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);
	ElementOfTree* node = new ElementOfTree(tree->getAmountElements(), token.value);
	tree->addElementInTree(node, indexParentNode);

	token = getNextToken();
	if (token.type != "DIVIDER" || token.value != "=")
		throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);

	value(indexParentNode);

	token = getNextToken();
	if (token.type != "DIVIDER" || token.value != ";") {
		Token prevToken = listOfTokens[numCurrentToken - 1];
		throw MissedSemicolonError(prevToken.pos[1], prevToken.pos[0]);
	}
}

void SyntaxParser::arythmeticalExpression(int indexParentNode)
{
	printInDebugMode("arythmeticalExpression");
	bool flag_recursive_error = false;

	std::string errorMessage("Wrong arythmetical expression");
	Token token;
	
	int savedNumToken = numCurrentToken;
	saveTreeAndIdTable();
	int indexSavedTree = numLastSavedTree;

	try{
		int indexSignNode = tree->getAmountElements();
		ElementOfTree* signNode = new ElementOfTree(indexSignNode, "NULL");
		tree->addElementInTree(signNode, indexParentNode);
		operand(indexSignNode);
		signNode = tree->searchFromRoot(indexSignNode);
		printTreeInDebug();
		sign(signNode);
		arythmeticalExpression(indexSignNode);
		return;
	}
	catch (SyntaxError& err){
		loadTreeAndIdTableWithIndex(indexSavedTree);
	}

	numCurrentToken = savedNumToken;
	int indexSignNode = tree->getAmountElements();
	ElementOfTree* signNode = new ElementOfTree(indexSignNode, "NULL");
	tree->addElementInTree(signNode, indexParentNode);
	operand(indexSignNode);
	signNode = tree->searchFromRoot(indexSignNode);
	sign(signNode);
	operand(indexSignNode);
}

void SyntaxParser::sign(ElementOfTree* fillValueOfThisNodeWithSign)
{
	printInDebugMode("sign");

	std::string errorMessage("Wrong arythmetical sign");
	Token token;

	token = getNextToken();
	if (token.type != "DIVIDER" || (token.value != "+" && token.value != "-" && token.value != "/" && token.value != "*"))
		throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);
	fillValueOfThisNodeWithSign->name = token.value;
}

void SyntaxParser::operand(int indexParentNode)
{
	printInDebugMode("operand");

	std::string errorMessage("wrong operand");
	Token token;

	int savedNumToken = numCurrentToken;
	saveTreeAndIdTable();
	printTreeInDebug();
	int indexSavedTree = numLastSavedTree;

	try {
		ElementOfTree* node = new ElementOfTree(tree->getAmountElements(), "function");
		tree->addElementInTree(node, indexParentNode);
		function(tree->getAmountElements() - 1);
		return;
	}
	catch (SyntaxError&) {
		loadTreeAndIdTableWithIndex(indexSavedTree);
	}

	numCurrentToken = savedNumToken;
	try {
	token = getNextToken();
	if (token.type != "IDENTIFIER")
		throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);
	ElementOfTree* node = new ElementOfTree(tree->getAmountElements(), token.value);
	tree->addElementInTree(node, indexParentNode);
	return;
	}
	catch (SyntaxError&) {
		loadTreeAndIdTableWithIndex(indexSavedTree);
	}

	numCurrentToken = savedNumToken;
	token = getNextToken();
	if (token.type != "CONSTANT")
		throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);
	ElementOfTree* node = new ElementOfTree(tree->getAmountElements(), token.value);
	tree->addElementInTree(node, indexParentNode);
	return;
}

void SyntaxParser::function(int indexParentNode)
{
	printInDebugMode("function");

	std::string errorMessage("wrong function call");
	Token token;

	int savedNumToken = numCurrentToken;
	saveTreeAndIdTable();
	int indexSavedTree = numLastSavedTree;

	try {
		token = getNextToken();
		if (token.type != "IDENTIFIER")
			throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);
		ElementOfTree* node = new ElementOfTree(tree->getAmountElements(), token.value);
		tree->addElementInTree(node, indexParentNode);

		node = new ElementOfTree(tree->getAmountElements(), "args");
		tree->addElementInTree(node, indexParentNode);
		token = getNextToken();
		if (token.type != "DIVIDER" || token.value != "(")
			throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);
		token = getNextToken();
		if (token.type != "DIVIDER" || token.value != ")")
			throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);

		return;
	}
	catch (SyntaxError&) {
		loadTreeAndIdTableWithIndex(indexSavedTree);
	}

	numCurrentToken = savedNumToken;
	token = getNextToken();
	if (token.type != "IDENTIFIER")
		throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);
	ElementOfTree* node = new ElementOfTree(tree->getAmountElements(), token.value);
	tree->addElementInTree(node, indexParentNode);

	token = getNextToken();
	if (token.type != "DIVIDER" || token.value != "(")
		throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);

	node = new ElementOfTree(tree->getAmountElements(), "args");
	tree->addElementInTree(node, indexParentNode);
	functionArguments(tree->getAmountElements() - 1);

	token = getNextToken();
	if (token.type != "DIVIDER" || token.value != ")")
		throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);
}

void SyntaxParser::functionArguments(int indexParentNode)
{
	printInDebugMode("functionArguments");

	std::string errorMessage("wrong function arguments");
	Token token;

	int savedNumToken = numCurrentToken;
	saveTreeAndIdTable();
	int indexSavedTree = numLastSavedTree;

	try {
		value(indexParentNode);
		return;
	}
	catch (SyntaxError&) {
		loadTreeAndIdTableWithIndex(indexSavedTree);
	}

	numCurrentToken = savedNumToken;
	//TODO change
	value(indexParentNode);
	if (token.type != "DIVIDER" || token.value != ",")
		throw SyntaxError(errorMessage, token.pos[0], token.pos[1]);
	functionArguments(indexParentNode);
}

void SyntaxParser::conditionsSequence(int indexParentNode)
{
	printInDebugMode("conditionsSequence");

	std::string errorMessage("wrong conditions sequence");
	Token token;

	int savedNumToken = numCurrentToken;
	saveTreeAndIdTable();
	int indexSavedTree = numLastSavedTree;


	try {
		condition(indexParentNode);
		printTreeInDebug();
		return;
	}
	catch (SyntaxError&) {
		loadTreeAndIdTableWithIndex(indexSavedTree);
	}

	numCurrentToken = savedNumToken;
	
	int indexOperatorNode = tree->getAmountElements();
	ElementOfTree* operatorNode = new ElementOfTree(indexOperatorNode, token.value);
	tree->addElementInTree(operatorNode, indexParentNode);

	condition(indexOperatorNode);
	logicalOperators(operatorNode);
	conditionsSequence(indexOperatorNode);
}

void SyntaxParser::printInDebugMode(const char* msg) {
	if (DEBUG_MODE_ON && DEBUG_PRINT_RECURSIVE_FUNCTION_CALLS)
		cout << msg << endl;
}

void SyntaxParser::printTreeInDebug()
{
	if (DEBUG_MODE_ON && DEBUG_PRINT_TREE) {
		cout << endl;
		printTree();
		cout << endl;
	}
}
