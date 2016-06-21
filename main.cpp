#include <iostream>

#include "function.hpp"
#include "parser.hpp"
using namespace analysis;



int main()
{
	Parser parser;
	auto f = parser("sin((5+x) * 4)");

	f->Print();
	std::cout << std::endl << f->GetValue(2.0) << std::endl;

	f->GetDerivative()->Print();

	while (true);
}
