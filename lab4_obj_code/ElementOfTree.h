#pragma once
#include <vector>
#include <iostream>

#include "settings.h"

class ElementOfTree {
public:
	int id{};

	std::string name;

	std::vector<ElementOfTree*> sons;

public:
	ElementOfTree() {}

	ElementOfTree(int id, std::string name) : id(id), name(name) {}

	~ElementOfTree();
};

