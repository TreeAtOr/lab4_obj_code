//#pragma once
//#include "ElementOfTree.h"
//#include <queue>
//#include <iostream>
//#include "tree.h"
//
//enum nodetypes
//{
//	_root,
//	_assignment,
//	_condition,
//	_definition,
//	_function,
//	_expession,
//	_while,
//	_dowhile
//};
//
////class root_node : public node
////{
////	
////};
//
//
//class node
//{
//public:
//	int type;
//	std::string name;
//	virtual void print() {};
//	void setName(string _name)
//	{
//		name = _name;
//	}
//};
//
//class assignment_node : public node
//{
//public:
//	assignment_node(ElementOfTree* left, ElementOfTree* right)
//	{
//		type = _assignment;
//		left_op = new node;
//		right_op = new node;
//		left_op->name = left->name;
//		right_op->name = right->name;
//		
//		cout << "triplet formed: assignment" << endl;
//		cout << " left " << left_op->name << endl;
//		cout << " right " << right_op->name << endl;
//		
//	}
//
//	node* left_op;
//	node* right_op;
//	void print() override
//	{
//		cout << ": assignment" << endl;
//		cout << " left " << left_op->name << endl;
//		cout << " right " << right_op->name << endl;
//	}
//};
//
//
//
//
//
//class ObjTree
//{
//public:
//
//	node* root;
//	Tree* lex_tree;
//
//
//	ObjTree(Tree* _tr)
//	{
//		root = nullptr;
//		lex_tree = _tr;
//	}
//
//	void callPrintTreePromEnd()
//	{
//		ElementOfTree* ptr = lex_tree->getRoot();
//		printTreeFromEnd(ptr, 0);
//	}
//
//	
//	node* printTreeFromEnd(ElementOfTree* ptr, int depth)
//	{
//		if (ptr->sons.size() == 0)
//			return nullptr;
//		node* prev_node = nullptr;
//		for (int i = ptr->sons.size() - 1; i >= 0; i--)
//		{
//			prev_node = printTreeFromEnd(ptr->sons[i], depth + 1);
//		}
//		cout << " prev is ";
//		if (prev_node != nullptr)
//			prev_node->print();
//		else
//			cout << " nullptr"<<endl;
//
//
//		node* add_node = new node;
//		/*if (ptr->sons.size() == 2)
//		{
//			formTriplet(ptr);
//		}*/
//
//		/*if (ptr->sons.size() > 2)
//		{
//			printf("more than two sons found: ");
//			cout << ptr->name << endl;
//			for (int i = 0; i < ptr->sons.size(); i++)
//			{
//				cout << ptr->sons[i]->name << endl;
//			}
//		}*/
//		/*if (ptr->name == "function")
//		{
//			cout << "triplet function" << endl;
//			for (int i = 0; i < ptr->sons.size(); i++)
//			{
//				cout << " " << ptr->sons[i]->name << endl;
//			}
//		}
//		if (ptr->name == "definition")
//		{
//			cout << "triplet definition" << endl;
//			for (int i = 0; i < ptr->sons.size(); i++)
//			{
//				cout << " " << ptr->sons[i]->name << endl;
//			}
//		}*/
//		if (ptr->name == "assignment")
//		{
//			cout << "triplet assignment" << endl;
//			for (int i = 0; i < ptr->sons.size(); i++)
//			{
//				cout << " " << ptr->sons[i]->name << endl;
//			}
//			add_node = new assignment_node(ptr->sons[0], ptr->sons[1]);
//		}
//		/*if (ptr->name == "program")
//		{
//			cout << "program" << endl;
//			for (int i = 0; i < ptr->sons.size(); i++)
//			{
//				cout << " " << ptr->sons[i]->name << endl;
//			}
//		}
//		if (ptr->name == "cond")
//		{
//			cout << "condition" << endl;
//			cout << " " << ptr->sons[0]->name << endl;
//			ptr = ptr->sons[0];
//			for (int i = 0; i < ptr->sons.size(); i++)
//			{
//				cout << " " << ptr->sons[i]->name << endl;
//			}
//		}
//		if (ptr->name == "expr")
//		{
//			cout << "expression" << endl;
//			cout << " " << ptr->sons[0]->name << endl;
//			ptr = ptr->sons[0];
//			for (int i = 0; i < ptr->sons.size(); i++)
//			{
//				cout << " " << ptr->sons[i]->name << endl;
//			}
//		}*/
//		return add_node;
//		/*for (int j = 0; j < depth; j++)
//			printf(" ");*/
//		//std::cout << ptr->name << std::endl;
//	}
//
//	void printObjTree()
//	{
//		callPrintTreePromEnd();
//	}
//};