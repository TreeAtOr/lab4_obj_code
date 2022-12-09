#include "AssemblyGenerator.h"
#include "myTree.h"
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

using namespace std;

AssemblyGenerator::AssemblyGenerator() {}
vector<string> make_subprogram(node *node, string prefix);
vector<string> make_function_call(function_call_node *node, string prefix);

string var2string(int i, string prefix = string()) {
  return "var_" + prefix + "_" + to_string(i);
}

string make_reference(uniq_id *id, string prefix = string()) {
  if (id->getType() == id_type::_identificator)
    return var2string(id->getIdx(), prefix);
  return "const_" + prefix + "_" + to_string(id->getIdx());
}

string const2string(const_base *row, int i, string prefix = string()) {
  auto declaration = string("const_") + prefix + "_" + to_string(i) + ": dq ";
  switch (row->type()) {
  case const_type::_c_int:
    declaration.append(to_string(((const_int *)row)->value));
    break;
  case const_type::_c_str:
    declaration.push_back('"');
    declaration.append(((const_str *)row)->value);
    declaration.append("\",10,0");
    break;

  default:
    break;
  }
  return declaration;
}

vector<string> make_expression(expression_node *node, string prefix) {
  vector<string> row;
  if (node->left_op->op_type() == operand_type::_id &&
      node->right_op->op_type() == operand_type::_id) {
    auto left_op = dynamic_cast<operand_id *>(node->left_op);
    auto right_op = dynamic_cast<operand_id *>(node->right_op);

    switch (node->sign_type) {
    case sign_types::_plus:
      row.push_back(string("mov rax, qword [") +
                    make_reference(right_op->value, prefix) + "]");
      row.push_back(string("add rax, qword [") +
                    make_reference(left_op->value, prefix) + "]");
      row.push_back(string("push rax"));

      break;

    case sign_types::_minus:
      row.push_back(string("mov rax, qword [") +
                    make_reference(right_op->value, prefix) + "]");
      row.push_back(string("sub rax, qword [") +
                    make_reference(left_op->value, prefix) + "]");
      row.push_back(string("push rax"));
      break;
    }
  }

  if (node->left_op->op_type() == operand_type::_id &&
      node->right_op->op_type() == operand_type::_node) {
    auto left_op = dynamic_cast<operand_id *>(node->left_op);
    auto right_op = dynamic_cast<operand_node *>(node->right_op);

    if (right_op->value->type == node_type::_expression) {
      auto _row = make_expression((expression_node *)right_op->value, prefix);
      row.insert(row.end(), _row.begin(), _row.end());
    }

    if (right_op->value->type == node_types_t::_function_call) {
      auto _row =
          make_function_call((function_call_node *)right_op->value, prefix);
      row.insert(row.end(), _row.begin(), _row.end());
    }

    switch (node->sign_type) {
    case sign_types::_plus:
      row.push_back(string("pop rax"));
      row.push_back(string("add rax, qword [") +
                    make_reference(left_op->value, prefix) + "]");
      row.push_back(string("push rax"));

      break;

    case sign_types::_minus:
      row.push_back(string("pop rax"));
      row.push_back(string("sub rax, qword [") +
                    make_reference(left_op->value, prefix) + "]");
      row.push_back(string("push rax"));
      break;
    }
  };

  return row;
};

vector<string> make_assignment(assignment_node *node, string prefix) {
  vector<string> row;
  if (node->left_op->op_type() == operand_type::_id &&
      node->right_op->op_type() == operand_type::_id) {
    auto left_op = dynamic_cast<operand_id *>(node->left_op);
    auto right_op = dynamic_cast<operand_id *>(node->right_op);
    right_op->value->getType();
    row.push_back(string("mov rax, qword [") +
                  make_reference(right_op->value, prefix) + "]");
    row.push_back(string("mov qword [") +
                  make_reference(left_op->value, prefix) + "],rax");
  }

  if (node->left_op->op_type() == operand_type::_id &&
      node->right_op->op_type() == operand_type::_node) {
    auto left_op = dynamic_cast<operand_id *>(node->left_op);
    auto right_op = dynamic_cast<operand_node *>(node->right_op);
    if (right_op->value->type == node_type::_expression) {
      auto _row = make_expression((expression_node *)right_op->value, prefix);
      row.insert(row.end(), _row.begin(), _row.end());
    }

    if (right_op->value->type == node_types_t::_function_call) {
      auto _row =
          make_function_call((function_call_node *)right_op->value, prefix);
      row.insert(row.end(), _row.begin(), _row.end());
    }

    row.push_back(string("pop rax"));
    row.push_back(string("mov qword [") +
                  make_reference(left_op->value, prefix) + "], rax");
  }

  return row;
};

vector<string> make_condition(condition_node *node, string prefix) {
  vector<string> row;
  if (node->left_op->op_type() == operand_type::_id &&
      node->right_op->op_type() == operand_type::_id) {
    auto left_op = dynamic_cast<operand_id *>(node->left_op);
    auto right_op = dynamic_cast<operand_id *>(node->right_op);
    right_op->value->getType();
    row.push_back(string("mov rax, qword [") +
                  make_reference(right_op->value, prefix) + "]");
    row.push_back(string("cmp qword [") +
                  make_reference(left_op->value, prefix) + "],rax");
  }

  if (node->left_op->op_type() == operand_type::_id &&
      node->right_op->op_type() == operand_type::_node) {
    auto left_op = dynamic_cast<operand_id *>(node->left_op);

    if (node->right_op->type == node_type::_expression) {
      auto right_op = dynamic_cast<operand_node *>(node->right_op);
      auto _row = make_expression((expression_node *)right_op->value, prefix);
      row.insert(row.end(), _row.begin(), _row.end());
    }

    row.push_back(string("pop rax"));
    row.push_back(string("cmp qword [") +
                  make_reference(left_op->value, prefix) + "], rax");
  }

  return row;
}

vector<string> make_if(if_node *node, string prefix) {
  vector<string> row;
  auto condition = make_condition(node->condition, prefix);
  row.insert(row.end(), condition.begin(), condition.end());
  auto else_label = prefix + "_else_" + to_string(node->idx_in_parent);
  auto end_label = prefix + "_end_" + to_string(node->idx_in_parent);
  switch (node->condition->comparator) {
  case _eq:
    row.push_back("jne " + else_label);
    break;
  case _not_eq:
    row.push_back("je " + else_label);
    break;
  case _more:
    row.push_back("jle " + else_label);
    break;
  case _less:
    row.push_back("jmo " + else_label);
    break;
  }

  for (auto command : node->do_true) {
    auto _row = make_subprogram(command, prefix);
    row.insert(row.end(), _row.begin(), _row.end());
  }
  row.push_back("jmp " + end_label);
  row.push_back(else_label + ':');
  for (auto command : node->do_false) {
    auto _row = make_subprogram(command, prefix);
    row.insert(row.end(), _row.begin(), _row.end());
  }
  row.push_back(end_label + ':');

  return row;
}

vector<string> make_while(while_node *node, string prefix) {
  vector<string> row;
  auto condition = make_condition(node->condition, prefix);
  row.insert(row.end(), condition.begin(), condition.end());
  auto while_label = prefix + "_while_" + to_string(node->idx_in_parent);
  auto end_label = prefix + "_while_end_" + to_string(node->idx_in_parent);
  row.push_back(while_label + ':');
  switch (node->condition->comparator) {
  case _eq:
    row.push_back("jne " + end_label);
    break;
  case _not_eq:
    row.push_back("je " + end_label);
    break;
  case _more:
    row.push_back("jle " + end_label);
    break;
  case _less:
    row.push_back("jmo " + end_label);
    break;
  }

  for (auto command : node->do_body) {
    auto _row = make_subprogram(command, prefix);
    row.insert(row.end(), _row.begin(), _row.end());
  }
  row.push_back("jmp " + while_label);
  row.push_back(end_label + ':');

  return row;
}

vector<string> make_function_call(function_call_node *node, string prefix) {

  vector<string> row;
  if (node->arg->type == _expression) {
    auto _row = make_expression((expression_node *)node->arg, prefix);
    row.insert(row.end(), _row.begin(), _row.end());
  }
  if (node->arg->type == node_type::_operand) {
    row.push_back("push " +
                  make_reference(((operand_id *)node->arg)->value, prefix));
  }

  row.push_back("call " + node->func_name);

  return row;
}

vector<string> make_subprogram(node *node, string prefix) {
  vector<string> row;
  switch (node->type) {
  case node_types_t::_assignment:
    row = make_assignment((assignment_node *)node, prefix);
    break;
  case node_types_t::_if:
    row = make_if((if_node *)node, prefix);
    break;
  case node_types_t::_while:
    row = make_while((while_node *)node, prefix);
    break;
  case node_types_t::_function_call:
    row = make_function_call((function_call_node *)node, prefix);
    row.push_back("pop rax");
    break;
  case node_types_t::_root:
    for (auto i : ((root_node *)node)->instructions) {
      auto _row = make_subprogram(i, prefix);
      row.insert(row.end(), _row.begin(), _row.end());
    }
  }
  return row;
};

vector<string> AssemblyGenerator::build(myTree &root, bool silent) {
  vector<string> rodata;
  vector<string> data;
  vector<string> text;

  rodata.push_back(string("SECTION .rodata"));
  for (int i = 0; i < root.conTable.table.size(); i++) {
    rodata.push_back(const2string(root.conTable.table.at(i), i));
  }

  data.push_back(string("SECTION .data"));
  for (int i = 0; i < root.idTable.table.size(); i++) {
    identifier *row = root.idTable.table.at(i);
    auto declaration = var2string(i) + ": dq 0";
    data.push_back(declaration);
  }

  text.push_back(string("SECTION .text"));
  text.push_back(string("global main"));

  for (auto function : root.functions) {
    auto context = function->name;
    text.push_back(context + ':');

    for (int i = 0; i < function->body->conTable.table.size(); i++) {
      rodata.push_back(
          const2string(function->body->conTable.table.at(i), i, context) );
    }

    for (int i = 0; i < function->body->idTable.table.size(); i++) {
      identifier *row = function->body->idTable.table.at(i);
      auto declaration = var2string(i, context) + ": dq 0";
      data.push_back(declaration);
    }
    text.push_back("pop qword [" + var2string(0, context) + "]");
    for (auto command : function->body->root->sons) {
      auto row = make_subprogram(command, context);
      text.insert(text.end(), row.begin(), row.end());
    }

    text.push_back("push " +
                   make_reference(function->return_value_id, context));
    text.push_back("ret");
  }

  text.push_back(string("main:"));
  auto row = make_subprogram(root.root, "");
  text.insert(text.end(), row.begin(), row.end());
  text.push_back("ret");

  auto program = rodata;
  program.insert(program.end(), data.begin(), data.end());
  program.insert(program.end(), text.begin(), text.end());

  return program;
};

void AssemblyGenerator::build_and_print(myTree &root) {
  for (auto line : this->build(root))
    cout << line << endl;
}