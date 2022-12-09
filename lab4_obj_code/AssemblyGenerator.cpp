#include "AssemblyGenerator.h"
#include "myTree.h"
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

using namespace std;

AssemblyGenerator::AssemblyGenerator() {}

string const2string(const_base *row, int i, string prefix = string()) {
  auto declaration = string("const_") + prefix + "_" + to_string(i) + ": db ";
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
      row.push_back(string("mov rax, byte [") + prefix +
                    to_string(right_op->value.getIdx()) + "]");
      row.push_back(string("add rax, byte [") + prefix +
                    to_string(left_op->value.getIdx()) + "]");
      row.push_back(string("push rax"));

      break;

    case sign_types::_minus:
      row.push_back(string("mov rax, byte [") + prefix +
                    to_string(right_op->value.getIdx()) + "]");
      row.push_back(string("sub rax, byte [") + prefix +
                    to_string(left_op->value.getIdx()) + "]");
      row.push_back(string("push rax"));
      break;
    }
  }

  if (node->left_op->op_type() == operand_type::_id &&
      node->right_op->op_type() == operand_type::_node) {
    auto left_op = dynamic_cast<operand_id *>(node->left_op);
    if (node->right_op->type == node_type::_expression) {
      auto right_op = dynamic_cast<operand_node *>(node->right_op);
      auto _row = make_expression((expression_node *)right_op->value, prefix);
      row.insert(row.end(), _row.begin(), _row.end());
    }

    switch (node->sign_type) {
    case sign_types::_plus:
      row.push_back(string("pop rax"));
      row.push_back(string("add rax, byte [") + prefix +
                    to_string(left_op->value.getIdx()) + "]");
      row.push_back(string("push rax"));

      break;

    case sign_types::_minus:
      row.push_back(string("pop rax"));
      row.push_back(string("sub rax, byte [") + prefix +
                    to_string(left_op->value.getIdx()) + "]");
      row.push_back(string("push rax"));
      break;
    }
  };

  return row;
};

vector<string> make_assignment(assignment_node *node, string prefix) {
  vector<string> row;
  cout << node->left_op.type << " " << node->right_op.type << endl;
  if (node->left_op.op_type() == operand_type::_id &&
      node->right_op.op_type() == operand_type::_id) {
    auto left_op = dynamic_cast<operand_id &>(node->left_op);
    auto right_op = dynamic_cast<operand_id &>(node->right_op);

    row.push_back(string("mov rax, byte [") + prefix +
                  to_string(right_op.value.getIdx()) + "]");
    row.push_back(string("mov byte [") + prefix +
                  to_string(left_op.value.getIdx()) + "], rax");
  }

  if (node->left_op.op_type() == operand_type::_id &&
      node->right_op.op_type() == operand_type::_node) {
    auto left_op = dynamic_cast<operand_id &>(node->left_op);

    if (node->right_op.type == node_type::_expression) {
      auto right_op = dynamic_cast<operand_node &>(node->right_op);
      auto _row = make_expression((expression_node *)right_op.value, prefix);
      row.insert(row.end(), _row.begin(), _row.end());
    }

    row.push_back(string("pop rax"));
    row.push_back(string("mov byte [") + prefix +
                  to_string(left_op.value.getIdx()) + "], rax");
  }

  return row;
};

vector<string> make_subprogram(node *node, string prefix) {
  cout << node->type << " " << prefix << endl;
  vector<string> row;
  switch (node->type) {
  case node_types_t::_assignment:
    row = make_assignment((assignment_node *)node, prefix);
    break;

  case node_types_t::_root:
    for (auto i : ((root_node*)node) -> instructions) {
      auto _row = make_subprogram(i, prefix);
      for(auto j: _row) cout << j << endl;
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
    auto declaration = string(row->name) + ": db 0";
    data.push_back(declaration);
  }

  text.push_back(string("SECTION .text"));

  for (auto function : root.functions) {
    auto context = function->name;
    text.push_back(context + ':');

    for (int i = 0; i < function->body->conTable.table.size(); i++) {
      rodata.push_back(
          const2string(function->body->conTable.table.at(i), i, context));
    }

    for (int i = 0; i < function->body->idTable.table.size(); i++) {
      identifier *row = function->body->idTable.table.at(i);
      auto declaration = context + "_" + string(row->name) + ": db 0";
      data.push_back(declaration);
    }
    for (auto command : function->body->root->sons) {
      auto row = make_subprogram(command, context);
      text.insert(text.end(), row.begin(), row.end());
    }
  }

  text.push_back(string("main:"));
  auto row = make_subprogram(root.root, "");
  text.insert(text.end(), row.begin(), row.end());

  auto program = rodata;
  program.insert(program.end(), data.begin(), data.end());
  program.insert(program.end(), text.begin(), text.end());

  return program;
};

void AssemblyGenerator::build_and_print(myTree &root) {
  for (auto line : this->build(root))
    cout << line << endl;
}