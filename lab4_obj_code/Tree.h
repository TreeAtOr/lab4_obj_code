#pragma once
#include "ElementOfTree.h"
#include <queue>
#include <iostream>

class Tree {
private:
	ElementOfTree* root;

	int level; 

	void print(ElementOfTree* node);

	int amountElements = 0;

	ElementOfTree* searchingOfElement(ElementOfTree* node, int indexOfDesiredNode);

	void freeMemory();

public:
	Tree();

	~Tree();

	Tree(const Tree& t2);

	Tree(std::string nameOfRoot);

	ElementOfTree* searchFromRoot(int indexOfNode);

	void deleteNodeSChildren(int indexOfNode);

	int getAmountElements();

	void addElementInTree(ElementOfTree* node, int indexOfDesiredNode);
	
	void printTree();

	void callPrintTreePromEnd()
	{
		ElementOfTree* ptr = root;
		printTreeFromEnd(ptr, 0);
	}

	void printTreeFromEnd(ElementOfTree* ptr, int depth)
	{
		if (ptr == nullptr)
			return;

		for (int i = ptr->sons.size()-1; i >= 0;i--)
		{
			printTreeFromEnd(ptr->sons[i], depth+1);	
		}

		for(int j = 0; j < depth; j++)
				printf(" ");
			std::cout << ptr->name << std::endl;
	}

	ElementOfTree* getRoot()
	{
		return root;
	}
};