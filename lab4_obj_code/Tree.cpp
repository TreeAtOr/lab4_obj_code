#include "Tree.h"

Tree::Tree(const Tree& t2) {
	root = new ElementOfTree(t2.root->id, t2.root->name);
	auto node = t2.root;
	std::queue<ElementOfTree*> queue;
	queue.push(node);

	while (!queue.empty()) {
		node = queue.front();
		queue.pop();

		for (int i = 0; i < node->sons.size(); ++i) {
			ElementOfTree* temp = new ElementOfTree(node->sons[i]->id, node->sons[i]->name);
			this->addElementInTree(temp, node->id);
			queue.push(node->sons[i]);
		}
	}
	amountElements = t2.amountElements;
	if (DEBUG_MODE_ON && DEBUG_PRINT_CONSTRUCTORS)
		std::cout << "Copying constructor is worked!\n";
}

Tree::Tree(std::string nameOfRoot) {
	root = new ElementOfTree(0, nameOfRoot);
	amountElements = 1;
}

ElementOfTree* Tree::searchFromRoot(int indexOfNode)
{
	return searchingOfElement(root, indexOfNode);
}

Tree::Tree() {
	root = new ElementOfTree(0, "root");
	amountElements = 1;
}

int Tree::getAmountElements()
{
	return amountElements;
}

ElementOfTree* Tree::searchingOfElement(ElementOfTree* node, int indexOfDesiredNode) {
	std::queue<ElementOfTree*> queue;
	queue.push(node);
	while (!queue.empty()) {
		node = queue.front();
		queue.pop();
		if (node->id == indexOfDesiredNode) return node;

		for (int i = 0; i < node->sons.size(); ++i) {
			queue.push(node->sons[i]);
		}
	}
	return nullptr;
}

void Tree::addElementInTree(ElementOfTree* node, int indexOfDesiredNode) {
	ElementOfTree* searchableElement = searchingOfElement(root, indexOfDesiredNode);
	if (searchableElement == nullptr) throw std::runtime_error("Pizdec");
	searchableElement->sons.push_back(node);
	amountElements++;
}

void Tree::printTree() {
	if (root == nullptr) {
		std::cout << "No elements!\n";
		return;
	}
	print(root);
	level = 0;
}

void Tree::print(ElementOfTree* node) {
	if (node == nullptr) return;

	for (int i = 0; i < level; i++) {
		std::cout << "| ";
	}

	std::cout << "[+](" << level << "-" << (node->name) << ")\n";

	int i = 0;
	while (i != node->sons.size()) {
		level++;
		print(node->sons[i]);
		if (i == node->sons.size() - 1) level--;
		i++;
	}
	if (node->sons.size() == 0) level--;
}

void Tree::freeMemory() {
	ElementOfTree* node = root;
	std::queue<ElementOfTree*> queue;
	queue.push(node);

	while (!queue.empty()) {
		node = queue.front();
		queue.pop();

		for (int i = 0; i < node->sons.size(); ++i) {
			queue.push(node->sons[i]);
		}
		delete node;
	}
}

Tree::~Tree() {
	if (DEBUG_MODE_ON && DEBUG_PRINT_CONSTRUCTORS)
		std::cout << "Destructor!\n";
	freeMemory();
}

void Tree::deleteNodeSChildren(int indexOfNode) {
	ElementOfTree* node = searchFromRoot(indexOfNode);
	if (node->sons.size() == 0) {
		std::cout << "The node with id " << indexOfNode << " and name "  << node->name 
			<< " doesnt have a children!\n";
		return;
	}

	ElementOfTree* currentRoot = node; // от какого узла будет происходить удаление

	std::queue<ElementOfTree*> queue;
	queue.push(node);

	while (!queue.empty()) {
		node = queue.front();
		queue.pop();

		for (int i = node->sons.size() - 1; i >= 0; --i) {
			queue.push(node->sons[i]);
			node->sons.pop_back();
		}
		if (currentRoot != node) // проверка, чтобы удалить только детей выбранного узла
			delete node;
	}
}