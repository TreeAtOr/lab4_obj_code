#include "myTree.h"
using namespace std;

class AssemblyGenerator {
    public:
    vector<string> build(myTree& root, bool silent = false);

    AssemblyGenerator();

    void build_and_print(myTree& root);

};