#pragma once
#include "ElementOfTree.h"
#include <queue>
#include <iostream>
#include "tree.h"

typedef enum node_types_t
{
	__invalid,
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
} node_type;

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
	node_type type;
	node* parent;
	int idx_in_parent;

	std::vector<node*> sons;

	ElementOfTree* relate;

	node()
	{
		type =_basic;
		relate = nullptr;
	}

	virtual void print()
	{
		cout << "_: " << relate->name << " ";
	}
};

typedef enum id_types_t
{
	_identificator = 1,
	_const
}id_type;

class uniq_id
{
	id_type type;
	int idx;
public:
	
	uniq_id()
	{

	}
	uniq_id(int i) : idx(i)
	{
	};
	uniq_id(id_type t, int i) : type (t), idx(i)
	{
	};
	void setIdx(int id)
	{
		idx = id;
	}
	void setType(id_type t)
	{
		type = t;
	}

	id_type getType()
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


typedef enum operand_types_t
{
	_none = 1,
	_id,
	_node
}operand_type;

class operand: public node
{
	
	public:
		
	operand_type optype;
	
	operand()
	{
		optype = _none;
	}
	operand(node* original)
	{
		this->parent = original->parent;
		this->relate = original->relate;
		idx_in_parent = original->idx_in_parent;
		type = _operand;
	}
	virtual operand_type op_type()
	{
		return optype;
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
		optype = _node;
	}
	operand_node(node* original)
	{
		this->parent = original->parent;
		this->relate = original->relate;
		idx_in_parent = original->idx_in_parent;
		type = _operand;
	
	}
	void form(node* _node)
	{
		value = _node;
	}

	operand_type op_type() override
	{
		return optype;
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
		optype = _id;
		this->parent = original->parent;
		this->relate = original->relate;
		idx_in_parent = original->idx_in_parent;
		type = _operand;
	}
	operand_type op_type() override
	{
		return optype;
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
	/*uniq_id left_op;
	uniq_id right_op;*/

	operand left_op;
	operand right_op;

	condition_node(node* original)
	{
		this->parent = original->parent;
		this->relate = original->relate;
		idx_in_parent = original->idx_in_parent;

		type = _condition;

		sons.clear();
		for (int i = 0; i < original->sons.size(); i++)
		{
			sons.push_back(original->sons[i]);
		}
	}

	void print() override
	{
		cout << "_condition: type(" << comparator << ") ";
		left_op.print(); cout << " "; 		
		right_op.print(); cout << endl;
	}

	void form(string comp, operand _op1, operand _op2)
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

	/*uniq_id* left_op;
	uniq_id* right_op;*/
	
	operand* left_op;
	operand* right_op;

	expression_node(node* original)
	{
		
		this->parent = original->parent;
		this->relate = original->relate;
		idx_in_parent = original->idx_in_parent;
		type = _expression;
		sons.clear();
		for (int i = 0; i < original->sons.size(); i++)
		{
			sons.push_back(original->sons[i]);
		}
	}
	void print() override
	{
		cout << "type : " << type << endl;
		cout << "_expression: sign(" << sign_type << ") ";
		left_op->print(); cout << " "; right_op->print(); cout << endl;
	}

	//void form(string sign_ptr, uniq_id* _op1, uniq_id* _op2)
	void form(string sign_ptr, operand* _op1, operand* _op2)
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

	/*uniq_id left_op;
	uniq_id right_op;*/
	operand left_op;
	operand right_op;

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

	void form(operand _l, operand _r)
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
		/*sons.clear();
		for (int i = 0; i < original->sons.size(); i++)
		{
			sons.push_back(original->sons[i]);
		}*/


	}


	void print() override
	{
		cout << "_root body:" << endl;
		for (int i = 0; i < instructions.size(); i++)
		{
			cout << "\t N" << i << " "; 
			instructions[i]->print(); 
			cout << endl;
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
		cout << "\n\tidentifiers table" << endl;
		for (int i = 0; i < table.size(); i++)
		{
			table[i]->print();		
		}
	}

};


typedef enum const_types_t
{
	_c_none = 1,
	_c_int,
	_c_str
}const_type;

class const_base
{
	public:
	virtual const_type type()
	{
		return _c_none;
	}
	virtual void print()
	 {}

	virtual bool isEqual(bool isInt, int _int = -1, string _str = "")
	{
		return false;
	}

	bool isNumber(string const_name)
	{
		int const_int_attempt;
		try {
			const_int_attempt = stoi(const_name);
		}
		catch (...)
		{
			// it is a string
			return false;
		}
		
		// its a number
		return true;
	}
};

class const_int : public const_base
{
public:
	int value;

	const_type type() override
	{
		return _c_int;
	}
	void print() override
	{
		cout << "int " << value << endl;
	}

	bool isEqual(bool isInt, int _int = -1, string _str = "") override
	{
		if (!isInt)
			return false;
		if (value == _int)
			return true;
		else
			return false;
	}
	void setValue(int v)
	{
		value = v;
	}

	const_int(int v) : value(v)
	{

	}
};

class const_str : public const_base
{
public:
	const_type type() override
	{
		return _c_str;
	}
	void print() override
	{
		cout << "str "<< value << endl;
	}

	string value;

	void setValue(string v)
	{
		value = v;
	}

	const_str(string v) : value(v)
	{

	}

	bool isEqual(bool isInt, int _int = -1, string _str = "") override
	{
		if (isInt)
			return false;
		if (value == _str)
			return true;
		else
			return false;
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

	int getId(bool isInt, int _val_int = -1, string _val_str = "")
	{
		for (int i = 0; i < table.size(); i++)
		{
			if (table[i]->isEqual(isInt, _val_int, _val_str))
				return i;
		}
		return -1;
	}

	void reform_bool_words()
	{
		
		string str = "true";

		const_int* new_true_const = new const_int(1);
		for (int i = 0; i < table.size(); i++)
		{
			
			if (table[i]->type() == _c_str)
			if (table[i]->isEqual(false, 0, str))
			{
				table[i] = new_true_const;
			}
		}
		str = "false";
		const_int* new_false_const = new const_int(0);
		for (int i = 0; i < table.size(); i++)
		{
			if (table[i]->type() == _c_str)
			if (table[i]->isEqual(false, 0, str))
			{
				table[i] = new_false_const;
			}
		}


	}

	void print()
	{
		cout << "\n\tconst table" << endl;
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
		print((root_node *) root, 0);
	}

	void print(node* ptr, int depth)
	{
		if (ptr == nullptr)
			return;

		for (int i = 0; i < depth; i++)
			printf(" ");
		//std::cout << depth << " node ";

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
				//cout << "\nDEFINITION MET:: " << endl;
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


	operand* form_new_op(node* _ptr)
	{
		operand* op;
		if (_ptr->type == _none)
		{
			// it a ID or a CONST
			//cout << "ID or a constanta" << endl;

			string const_name = _ptr->relate->name;
			int idx_in_idTable = idTable.getId(const_name);

			int idx_in_conTable;
			uniq_id* table_id;

			if (idx_in_idTable == -1)// not in idTable, it is a contanta
			{

				const_base* new_const = new const_base;

				if (new_const->isNumber(const_name)) // this const is number
				{
					// form integer constanta
					int integer_const_value = stoi(const_name);

					new_const = new const_int(integer_const_value);


					// search in conTable if exists
					idx_in_conTable = conTable.getId(true, integer_const_value);

					if (idx_in_conTable == -1) //doesnt exist, add to conTable
					{
						conTable.add(new_const);
						idx_in_conTable = conTable.getId(true, integer_const_value);
					}
				}
				else // this const is string
				{
					// form string constanta
					string integer_string_value = const_name;

					new_const = new const_str(integer_string_value);

					// search in conTable if exists
					idx_in_conTable = conTable.getId(false, 0, integer_string_value);

					if (idx_in_conTable == -1)
					{
						conTable.add(new_const);
						
						idx_in_conTable = conTable.getId(false, 0, integer_string_value);
					}
				}


				table_id = new uniq_id(_const, idx_in_conTable);
			}
			else
			{
				// is in idTable
				table_id = new uniq_id(_identificator, idx_in_idTable);
			}

			op = new operand_id(_ptr);
			((operand_id*)op)->form(*table_id);

		}
		else
		{
			// its a node
			//cout << "node" << endl;
			op = new operand_node(_ptr);
			((operand_node*)op)->form(_ptr);
		}
		return op;
	}
	

	void reform_layer(node* ptr, int depth, int layer)
	{
		if (ptr == nullptr)
			return;

		if (depth == layer)
		{
			//cout << "\t\tLAYER: " << layer << endl;
			string name = ptr->relate->name;

			if (name == "root")
			{
				root_node* new_root = new root_node(ptr);
				new_root->form(ptr->sons[0]);
				root = new_root;		
				return;
			}

			
			if (name == "cond")
			{
				node* sign = ptr->sons[0];

				condition_node* add_node = new condition_node(ptr);

				operand* op1 = form_new_op(sign->sons[0]);
				operand* op2 = form_new_op(sign->sons[1]);

				add_node->form(sign->relate->name, op1, op2);
				ptr = add_node;

				node* parent = ptr->parent;
				parent->sons[ptr->idx_in_parent] = ptr;

				
				return;
			}

			if (name == "if")
			{
				
				if_node* add_node = new if_node(ptr);
				add_node->form(ptr);
				node* parent = ptr->parent;
				ptr = add_node;
				parent->sons[ptr->idx_in_parent] = ptr;

				return;
			}

			if (name == "function")
			{
				
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
				
				assignment_node* add_node = new assignment_node(ptr);
				operand* op1 = form_new_op(ptr->sons[0]);

				operand* op2 = form_new_op(ptr->sons[1]);

				add_node->form( op1, op2);
				ptr = add_node;

				node* parent = ptr->parent;
				parent->sons[ptr->idx_in_parent] = ptr;


				return;
			}

			if (name == "program")
			{
				
				root_node* add_node = new root_node(ptr);
				add_node->form(ptr);
				ptr = (node*)add_node;
				node* parent = ptr->parent;
				parent = ptr;

				return;
			}

			if (name == "while")
			{
				
				while_node* add_node = new while_node(ptr);

				add_node->form((condition_node*)ptr->sons[0], ptr->sons[1]);
				
				ptr = (node*)add_node;
				
				node* parent = ptr->parent;
				parent->sons[ptr->idx_in_parent] = ptr;

				return;
			}
			
			if (name == "+" || name == "-" || name == "*" || name == "/")
			{

				node* sign = ptr;

				expression_node* add_node = new expression_node(sign);

				if (sign->sons.size() == 0)
				{
					// it is a string constanta
					// form string const

					string integer_string_value = sign->relate->name;
					const_str* new_const = new const_str(integer_string_value);

					// search in conTable if exists
					int idx_in_conTable = conTable.getId(false, 0, integer_string_value);

					if (idx_in_conTable == -1)
						conTable.add(new_const);

					return;
				}
				else
				{
					operand* op1 = form_new_op(sign->sons[0]);
					operand* op2 = form_new_op(sign->sons[1]);
					
					add_node->form(sign->relate->name, op1, op2);
					ptr = add_node;

					node* parent = ptr->parent;
					parent->sons[ptr->idx_in_parent] = ptr;				

				}

				
				return;
			}

			if (name == "expr")
			{
				
				node* sign = ptr->sons[0];
				expression_node* add_node = new expression_node(ptr);
			
				operand* op1 = form_new_op(sign->sons[0]);
				operand* op2 = form_new_op(sign->sons[1]);

				add_node->form(sign->relate->name, op1, op2);
				ptr = add_node;

				node* parent = ptr->parent;
				parent->sons[ptr->idx_in_parent] = ptr;

				return;
			}

			return;
		}

		for (int i = ptr->sons.size(); i > 0; i--)
		//for (int i = 0; i != ptr->sons.size(); i++)
		{
			reform_layer(ptr->sons[i-1], depth + 1, layer);
			//reform_layer(ptr->sons[i], depth + 1, layer);
		}
	}
};

class func_procl
{
public:

	int return_type;
	string name;

	reformable_tree* body;
	uniq_id* return_value_id; // int id

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
		
		body->root = (root_node*)_procl->sons[3];
	
		form_body();

		//body->root = new root_node(_procl->sons[3]);
		body->conTable.reform_bool_words();

		int idx_of_return_value = body->idTable.getId(_procl->sons[4]->sons[0]->relate->name);
		return_value_id = new uniq_id(_identificator, idx_of_return_value);
	}
	
	void form_body()
	{
		find_max_depth(body->root, 0);
		body->form_idTable();
		for (int i = body->max_depth; i >= 0; i--)
		{
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
		cout << return_type << " " << name << endl;
		body->print_tree();
		body->idTable.print();
		body->conTable.print();
		cout << "returns : ";
		body->idTable.get(return_value_id->getIdx())->print();
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
			
			form_func(root->sons[i]);
		}
	}

	void form_func(node* ptr)
	{
		string name = ptr->relate->name;
		if (name == "function")
		{
			//printf("function definition found");

			func_procl* new_procl = new func_procl();
			new_procl->form(ptr);
			//new_procl->print();
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
			//printf("\t\t%d\n", i);
			reform_layer(root, 0, i);
			//print(root, 0);
		}
		printf("\n\nfinal\n");
		print(root, 0);
		idTable.print();
		conTable.reform_bool_words();
		conTable.print();
		for (int i = 0; i < functions.size(); i++)
		{
			cout << "\n\tFUNCTION PROCL N" << i << endl;
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