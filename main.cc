#include "grammar.tab.hh"
#include <iostream>

int main() {
	INode* res;
	yy::parser parse(res);
	parse();
	Context context;
	value_type code = res->apply(context);
	std::cout << std::endl << "Program finished with exit code " << code << std::endl;
}
