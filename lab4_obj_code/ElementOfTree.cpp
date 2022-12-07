#include "ElementOfTree.h"

ElementOfTree::~ElementOfTree() {
	if (DEBUG_MODE_ON && DEBUG_PRINT_CONSTRUCTORS)
		std::cout << "Destructor of " << id << "!\n";
}