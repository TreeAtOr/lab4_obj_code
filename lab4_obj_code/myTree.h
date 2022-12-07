#pragma once
#include "ElementOfTree.h"
#include <queue>
#include <iostream>
#include "tree.h"

enum node_types
{
	_basic = 1,
	_operand,
	_root,
	_if,
	_function_call,
	_assignment,
	_expression,
	_condition,
	_definition,
	_while,
	_func_procl
};

enum compar_types
{
	_eq = 1,
	_not_eq,
	_more, _more_eq,
	_less, _less_eq
};

enum sign_types
{
	_mul = 1, _div, _plus, _minus
};

class node
{
public:
	int type;
	node* parent;
	int idx_in_parent;

	std::vector<node*> sons;

	ElementOfTree* relate;

	node()
	{
		type = _basic;
		relate = nullptr;
	}

	virtual void print()
	{
		cout << "_: " << relate->name << " ";
	}
};

enum id_types
{
	_identificator = 1,
	_const
};

class uniq_id
{
	int type;
	int idx;
public:
	
	uniq_id()
	{

	}
	uniq_id( int i) : idx(i)
	{
	};
	void setIdx(int id)
	{
		idx = id;
	}
	void setType(int t)
	{
		type = t;
	}

	int getType()
	{
		return type;
	}
	int getIdx()
	{
		return idx;
	}
	void print()
	{
		if (type == _identificator)
			cout << "identificator: ";
		else
			cout << "conts: ";
		cout << idx << endl;
	}
};


enum operand_types
{
	_none = 1,
	_id,
	_node
};

class operand: public node
{
	public:
	operand()
	{

	}
	operand(node* original)
	{
		this->parent = original->parent;
		this->relate = original->relate;
		idx_in_parent = original->idx_in_parent;
		type = _operand;
	}
	virtual int op_type()
	{
		return _none;
	}
	virtual void print()
	{
		
	}
};

class operand_node : public operand
{
public:

	node* value;
	operand_node()
	{

	}
	operand_node(node* original)
	{
		this->parent = original->parent;
		this->relate = original->relate;
		idx_in_parent = original->idx_in_parent;
		type = _operand;
	}

	int op_type() override
	{
		return _node;
	}
	void print() override
	{
		value->print();
	}

};

class operand_id : public operand
{
public:

	uniq_id value;
	operand_id(node* original)
	{
		this->parent = original->parent;
		this->relate = original->relate;
		idx_in_parent = original->idx_in_parent;
		type = _operand;
	}
	int op_type() override
	{
		return _id;
	}
	void form(uniq_id id)
	{
		value = id;
	}

	void print() override
	{
		value.print();
	}

};


class condition_node : public node
{
public:
	int comparator;
	uniq_id left_op;
	uniq_id right_op;

	condition_node(node* original)
	{
		this->parent = original->parent;
		this->relate = original->relate;
		idx_in_parent = original->idx_in_parent;

		type = _condition;
	}

	void print() override
	{
		cout << "_condition: type(" << comparator << ") ";
		left_op.print(); cout << " "; 		
		right_op.print(); cout << endl;
	}

	void form(string comp, uniq_id _op1, uniq_id _op2)
	{
		if (comp == "==")
			comparator = _eq;
		if (comp == "!=")
			comparator = _not_eq;
		if (comp == ">")
			comparator = _more;
		if (comp == ">=")
			comparator = _more_eq;
		if (comp == "<")
			comparator = _less;
		if (comp == "<=")
			comparator = _less_eq;

		left_op = _op1;
		right_op = _op2;

	}
};

class  if_node : public node
{
public:
	
	condition_node* condition;
	std::vector<node*> do_true;
	std::vector<node*> do_false;


	if_node(node* original)
	{
		this->parent = original->parent;
		this->relate = original->relate;
		idx_in_parent = original->idx_in_parent;

		type = _if;
	}

	void print() override
	{
		cout << "_if: ";
		condition->print();
		printf("\tthen\n");
		for (int i = 0; i < do_true.size(); i++)
		{
			printf("\t"); do_true[i]->print();
		}
		printf("\telse\n");
		for (int i = 0; i < do_false.size(); i++)
		{
			printf("\t"); do_false[i]->print();
		}
	}


	void form(node* ptr)
	{
		condition = ((condition_node*)ptr->sons[0]);
		node* then_node = ptr->sons[1];
		for (int i = 0; i < then_node->sons.size(); i++)
		{
			do_true.push_back(then_node->sons[i]);
		}
		
		node* else_node = ptr->sons[2];
		for (int i = 0; i < else_node->sons.size(); i++)
		{
			do_false.push_back(else_node->sons[i]);
		}
	}
};

class  while_node : public node
{
public:

	condition_node* condition;
	std::vector<node*> do_body;



	while_node(node* original)
	{
		this->parent = original->parent;
		this->relate = original->relate;
		idx_in_parent = original->idx_in_parent;

		type = _while;
	}

	void print() override
	{
		cout << "_while: ";
		((condition_node*)condition)->print();
		for (int i = 0; i < do_body.size(); i++)
		{
			printf("\t"); do_body[i]->print();
		}
	}

	void form(condition_node* _cond, node* _do)
	{
		condition =  _cond;
		for(int i = 0; i < _do->sons.size();i++)
			do_body.push_back(_do->sons[i]);

	}

};

class  function_call_node : public node
{
public:

	string func_name;
	node* arg;

	function_call_node(node* original)
	{
		this->parent = original->parent;
		this->relate = original->relate;
		idx_in_parent = original->idx_in_parent;
		type = _function_call;
	}

	void print() override
	{
		cout << "_function_call: " << func_name << " "; arg->print();  cout << endl;
	}


};

class expression_node : public node
{
public:
	int sign_type;

	uniq_id* left_op;
	uniq_id* right_op;
	expression_node(node* original)
	{
		this->parent = original->parent;
		this->relate = original->relate;
		idx_in_parent = original->idx_in_parent;
		type = _expression;
	}

	void print() override
	{
		cout << "_expression: sign(" << sign_type << ") ";
		left_op->print(); cout << " "; right_op->print(); cout << endl;
	}

	void form(string sign_ptr, uniq_id* _op1, uniq_id* _op2)
	{
		if (sign_ptr == "+")
			sign_type = _plus;
		if (sign_ptr == "-")
			sign_type = _minus;
		if (sign_ptr == "*")
			sign_type = _mul;
		if (sign_ptr == "/")
			sign_type = _div;

		left_op = _op1;
		right_op = _op2;
	}

};

class definition_node : public node
{
public:

	node* ident_type;
	node* ident_name;

	definition_node(node* original)
	{
		this->parent = original->parent;
		this->relate = original->relate;
		idx_in_parent = original->idx_in_parent;
		type = _definition;
	}

	void print() override
	{
		cout << "_definition: "<< ident_type->relate->name << " " << ident_name->relate->name <<endl;
	}

	void form(node* def_ptr)
	{
		ident_type = def_ptr->sons[0];
		ident_name = def_ptr->sons[1];
	}

};

class assignment_node : public node
{
public:

	uniq_id left_op;
	uniq_id right_op;

	assignment_node(node* original)
	{
		this->parent = original->parent;
		this->relate = original->relate;
		idx_in_parent = original->idx_in_parent;
		type = _assignment;
	}

	void print() override
	{
		cout << "_assignment: ";
		left_op.print();
		right_op.print();
		}

	void form(uniq_id _l, uniq_id _r)
	{
		left_op = _l;
		right_op = _r;
	}

};

class root_node:public node {

public:

	std::vector<node*> instructions;
	
	root_node(node* original)
	{
		this->parent = original->parent;
		this->relate = original->relate;
		idx_in_parent = original->idx_in_parent;
		type = _root;
	}


	void print() override
	{
		cout << "_root body:" << endl;
		for (int i = 0; i < instructions.size(); i++)
		{
			cout << "\t N" << i << " "; instructions[i]->print(); // << endl;
		}
	}

	void form(node* root)
	{
		for (int i = 0; i < root->sons.size(); i++)
		{
			instructions.push_back(root->sons[i]);
		}
	}

};



enum datatype_types
{
	_unk,
	_int,
	_string,
	_bool
};

class identifier
{
public:
	int type;
	string name;
	identifier() {
		type = _unk;
		
	}
	void setName(string _name)
	{
		name = _name;
	}

	void setType(string _type)
	{
		
		if (_type == "integer")
		{
			type = _int;
		}
		if (_type == "string")
		{
			type = _string;
		}
		if (_type == "bool")
		{
			type = _bool;
		}
	}

	void print()
	{
		cout << "type: " << type << " " << name << endl;
	}

};

class identifiersTable
{
public:

	vector<identifier*> table;

	void add(identifier* id)
	{
		table.push_back(id);
	}

	identifier* get(int i)
	{
		return	table[i];
	}

	identifier* get(string str)
	{
		for (int i = 0; i < table.size(); i++)
		{
			if (table[i]->name == str)
				return get(i);
		}
		return nullptr;
	}


	int getId(string str)
	{
		for (int i = 0; i < table.size(); i++)
		{
			if (table[i]->name == str)
				return i;
		}
		return -1;
	}
	void print()
	{
		for (int i = 0; i < table.size(); i++)
		{
			table[i]->print();		
		}
	}

};


enum const_types
{
	_c_none = 1,
	_c_int,
	_c_str
};

class const_base
{
	public:
	virtual int type()
	{
		return _c_none;
	}
	virtual void print()
	 {}
};

class const_int : public const_base
{
public:
	int type() override
	{
		return _c_int;
	}
	void print() override
	{
		cout << "int " << value;
	}

	int value;

	const_int(int v) : value(v)
	{

	}
};

class const_str : public const_base
{
public:
	int type() override
	{
		return _c_str;
	}
	void print() override
	{
		cout << "str "<< value;
	}

	string value;

	const_str(string v) : value(v)
	{

	}
};


class constantsTable
{
public:
	vector<const_base*> table;

	int add(const_base* elem)
	{
		table.push_back(elem);
		return ((int)table.size() - 1);
	}

	const_base* get(int i)
	{
		return table[i];
	}


	void print()
	{
		cout << "const table" << endl;
		for (int i = 0; i < table.size(); i++)
		{
			cout << i << " ";
			table[i]->print();
			cout << endl;
		}
	}
};



class reformable_tree
{
public: 
	root_node* root;
	identifiersTable idTable;
	constantsTable conTable;

	int max_depth;
	void print_tree()
	{
		print(root, 0);
	}

	void print(node* ptr, int depth)
	{
		if (ptr == nullptr)
			return;

		for (int i = 0; i < depth; i++)
			printf(" ");
		std::cout << depth << " node ";

		ptr->print();

		for (int i = 0; i != ptr->sons.size(); i++)
		{
			print(ptr->sons[i], depth + 1);
		}
	}


	void form_idTable()
	{
		for (int i = max_depth; i >= 0; i--)
		{
			check_layer_for_definition(root, 0, i);
		}
	}

	int check_layer_for_definition(node* ptr, int depth, int layer)
	{
		if (ptr == nullptr)
			return 0;

		if (depth == layer)
		{

			string name = ptr->relate->name;

			if (name == "definition")
			{
				cout << "\nDEFINITION MET:: " << endl;
				identifier* newId = new identifier;
				newId->setType(ptr->sons[0]->relate->name);
				newId->setName(ptr->sons[1]->relate->name);

				idTable.add(newId);

				int id = ptr->idx_in_parent;
				ptr = ptr->parent;

				std::vector<node*>::iterator it = ptr->sons.begin();
			
				for (int i = 0; i < id; i++)
					it++;
			
				for (int i = id; i < ptr->sons.size(); i++)
					ptr->sons[i]->idx_in_parent -= 1;
			

				cout << ptr->sons.size() << "  " << endl;
				ptr->sons.erase(it);
				
				return 1;
			}

			return 0;
		}

		for (int i = 0; i != ptr->sons.size(); )
		{
			if (check_layer_for_definition(ptr->sons[i], depth + 1, layer) == 0)
				i++;
		}

		return 0;

	}

	void reform_layer(node* ptr, int depth, int layer)
	{
		if (ptr == nullptr)
			return;

		if (depth == layer)
		{
			cout << "\t\tLAYER: " << layer << endl;
			string name = ptr->relate->name;

			if (name == "root")
			{
				cout << "\nROOT MET:: " << endl;

				printf("  size  %d\n", ptr->sons.size());
				for (int i = 0; i < ptr->sons.size(); i++)
				{
					cout << " " << ptr->sons[i]->relate->name << " " << ptr->type << endl;
				}

				root_node* new_root = new root_node(ptr);
				new_root->form(ptr->sons[0]);
				root = new_root;
				root->print();

				return;
			}

			
			if (name == "cond")
			{
				cout << "\nCONDITION MET" << endl;

				node* sign = ptr->sons[0];

				condition_node* add_new = new condition_node(ptr);

				int id = idTable.getId(sign->sons[0]->relate->name);
				uniq_id id_left(id);
				if (id != -1)
					id_left.setType(_identificator);
				else
				{
					id_left.setType(_const);
					cout << ptr->sons[0]->sons[0]->type << endl;
					cout << ptr->sons[0]->sons[0]->relate->name << endl;

				}

				id = idTable.getId(sign->sons[1]->relate->name);
				uniq_id id_right(id);
				if (id != -1)
					id_right.setType(_identificator);
				else
				{
					id_right.setType(_const);
					cout << ptr->sons[0]->sons[1]->type << endl;
					cout << ptr->sons[0]->sons[1]->relate->name << endl;
				}


				add_new->form(sign->relate->name, id_left, id_right);
				ptr = add_new;

				node* parent = ptr->parent;
				parent->sons[ptr->idx_in_parent] = add_new;

			}

			if (name == "if")
			{
				cout << "\nIF MET:: " << endl;
				if_node* add_node = new if_node(ptr);
				add_node->form(ptr);
				node* parent = ptr->parent;
				ptr = add_node;
				parent->sons[ptr->idx_in_parent] = ptr;

				return;
			}

			if (name == "function")
			{
				cout << "\nFUNCTION MET:: " << endl;
				if (ptr->sons.size() > 2)
				{
					printf("finction definition found");
					return;
				}

				function_call_node* add_new = new function_call_node(ptr);
				add_new->func_name = ptr->sons[0]->relate->name;
				add_new->arg = ptr->sons[1]->sons[0];
				ptr = add_new;
				node* parent = ptr->parent;
				parent->sons[ptr->idx_in_parent] = ptr;


				return;
			}


			if (name == "assignment")
			{
				cout << "\nASSIGNMENT MET:: " << endl;

				assignment_node* add_node = new assignment_node(ptr);

				//idTable.print();
				int id = idTable.getId(ptr->sons[0]->relate->name);
				uniq_id id_left(id);
				if (id != -1)
					id_left.setType(_identificator);
				else
				{
					id_left.setType(_const);
					cout << ptr->sons[0]->type << endl;
					cout << ptr->sons[0]->relate->name << endl;

				}

				id = idTable.getId(ptr->sons[1]->relate->name);
				uniq_id id_right(id);
				if (id != -1)
					id_right.setType(_identificator);
				else
				{
					id_right.setType(_const);
					cout << ptr->sons[1]->type << endl;
					cout << ptr->sons[1]->relate->name << endl;
				}


				add_node->form(id_left, id_right);
				ptr = (node*)add_node;
				node* parent = ptr->parent;
				parent->sons[ptr->idx_in_parent] = ptr;
				return;
			}

			if (name == "program")
			{
				cout << "\nPROGRAM MET:: " << endl;

				root_node* add_node = new root_node(ptr);
				add_node->form(ptr);
				ptr = (node*)add_node;
				node* parent = ptr->parent;
				parent = ptr;

				return;
			}

			if (name == "while")
			{
				cout << "\nWHILE MET:: " << endl;

				while_node* add_node = new while_node(ptr);

				add_node->form((condition_node*)ptr->sons[0], ptr->sons[1]);
				ptr->sons[0]->print();
				ptr = (node*)add_node;
				ptr->print();
				node* parent = ptr->parent;
				parent->sons[ptr->idx_in_parent] = ptr;

				return;
			}
			
			if (name == "expr")
			{
				cout << "\nEXPRESSION MET:: " << endl;

				expression_node* add_node = new expression_node(ptr);
				node* sign = ptr->sons[0];
				cout << ptr->sons[0]->sons[0]->type << endl;
				cout << ptr->sons[0]->sons[1]->type << endl;
				cout << ptr->sons[0]->type << endl;

				int id = idTable.getId(ptr->sons[0]->sons[0]->relate->name);
				uniq_id id_left(id);

				if (id != -1)
					id_left.setType(_identificator);
				else
				{
					id_left.setType(_const);
					cout << ptr->sons[0]->sons[0]->type << endl;
					cout << ptr->sons[0]->sons[0]->relate->name << endl;
					if (ptr->sons[0]->sons[0]->sons.size() != 0)
					{
						cout << "childs: " << endl;
						cout << "\t" << ptr->sons[0]->sons[0]->sons[0]->relate->name << endl;
						cout << "\t" << ptr->sons[0]->sons[0]->sons[1]->relate->name << endl;
					}
				}


				id = idTable.getId(ptr->sons[0]->sons[1]->relate->name);
				uniq_id id_right(id);
				
				if (id != -1)
					id_right.setType(_identificator);
				else
				{
					id_right.setType(_const);
					cout << ptr->sons[0]->sons[1]->type << endl;
					cout << ptr->sons[0]->sons[1]->relate->name << endl;
					if (ptr->sons[0]->sons[1]->sons.size() != 0)
					{
						cout << "childs: " << endl;
						cout << "\t"<<ptr->sons[0]->sons[1]->sons[0]->relate->name << endl;
						cout << "\t"<<ptr->sons[0]->sons[1]->sons[1]->relate->name << endl;
					}
					//cout << ptr->sons[0]->sons[1]->relate->name << endl;
				}

				add_node->form(sign->relate->name, &id_left, &id_right);
				ptr = add_node;

				node* parent = ptr->parent;
				parent->sons[ptr->idx_in_parent] = ptr;
				ptr->print();

				return;
			}

			return;
		}

		for (int i = 0; i != ptr->sons.size(); i++)
		{
			reform_layer(ptr->sons[i], depth + 1, layer);
		}
	}
};

class func_procl
{
public:

	int return_type;
	string name;
	int arg_idx;
	reformable_tree* body;
	identifier* return_value; // int id

	func_procl()
	{
	}


	void form(node* _procl)
	{
		string _type = _procl->sons[0]->relate->name;
		if (_type == "integer")
		{
			return_type = _int;
		}
		if (_type == "string")
		{
			return_type = _string;
		}
		if (_type == "bool")
		{
			return_type = _bool;
		}

		name = _procl->sons[1]->relate->name;

		identifier* new_arg = new identifier;
		new_arg->setType(_procl->sons[2]->sons[0]->sons[0]->relate->name);
		new_arg->setName(_procl->sons[2]->sons[0]->sons[1]->relate->name);
		
		
		body = new reformable_tree;
		
		body->idTable.add(new_arg);
		
		
		arg_idx = 0;
		body->root = (root_node*)_procl->sons[3];
		form_body();

	}
	
	void form_body()
	{
		find_max_depth(body->root, 0);
		body->form_idTable();
		for (int i = body->max_depth; i >= 0; i--)
		{
			printf("\t\t%d\n", i);
			body->reform_layer(body->root, 0, i);
		}

	}

	void find_max_depth(node* ptr, int depth)
	{
		if (depth > body->max_depth)
			body->max_depth = depth;

		for (int i = 0; i < ptr->sons.size(); i++)
			find_max_depth( ptr->sons[i], depth + 1);
		
	}

	void print()
	{
		cout << return_type << " " << name;
		body->print_tree();
		body->idTable.print();
		body->conTable.print();
	}
};




// Господи, помоги, пожалуйста

class myTree: public reformable_tree
{
public:

	std::vector<func_procl*> functions;
	
	void copyNode(node* prev, ElementOfTree* ptr, node* current_copy, int depth, int idx)
	{
		current_copy->idx_in_parent = idx;
		current_copy->relate = ptr;
		current_copy->parent = prev;
		if (depth > max_depth)
			max_depth = depth;

		for (int i = 0; i < ptr->sons.size(); i++)
		{
			current_copy->sons.push_back(new node);
			copyNode(current_copy, ptr->sons[i], current_copy->sons[i], depth + 1, i);
		}
		
	}

	myTree(Tree* tr)
	{
		ElementOfTree* ptr = tr->getRoot();
		max_depth = 0;
		int depth = 0;

		root = (root_node*)new node;
		copyNode(nullptr, ptr, root, depth, 0);
	}


	void form_all_functions()
	{
		
		for (int i = 1; i < root->sons.size(); i++)
		{
			printf("\t\t%d\n", i);
			form_func(root->sons[i]);
		}
		print(root, 0);
	}

	void form_func(node* ptr)
	{
		string name = ptr->relate->name;
		if (name == "function")
		{
			printf("function definition found");

			func_procl* new_procl = new func_procl();
			new_procl->form(ptr);
			new_procl->print();
			functions.push_back(new_procl);

			int id = ptr->idx_in_parent;
			ptr = ptr->parent;

			std::vector<node*>::iterator it = ptr->sons.begin();
			
			for (int i = 0; i < id; i++)
				it++;	
			
			ptr->sons.erase(it);

			return;
		}
		else
			printf("error\n");

	}

	void form_new_tree()
	{
		form_all_functions();
		form_idTable();

		for (int i = max_depth; i >= 0; i--)
		{
			printf("\t\t%d\n", i);
			reform_layer(root, 0, i);
			print(root, 0);
		}
		printf("\n\nfinal\n");
		print(root, 0);
		idTable.print();
		conTable.print();
		for (int i = 0; i < functions.size(); i++)
		{
			functions[i]->print();
		}

	}


	void print_tree_by_layers()
	{
		for (int i = max_depth; i >= 0; i--)
		{
			printf("\t\t%d\n", i);
			print_layer(root, 0, i);
		} 

	}

	void print_layer(node* ptr, int depth, int layer)
	{
		if (ptr == nullptr)
			return ;

		if (depth == layer)
		{
			std::cout << " node ";
			if (ptr->parent != nullptr)
				ptr->print();
			else
				cout << ptr->relate->name << "\n\tparent: nullptr" << endl;
			return ;
		}
		
		for (int i = 0; i != ptr->sons.size(); i++)
		{
			print_layer(ptr->sons[i], depth + 1, layer);
		}
	}



	
};