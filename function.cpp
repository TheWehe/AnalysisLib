#include "function.hpp"
#include <iostream>
#include <cmath>



namespace analysis
{
	Function::Ptr Sum::GetDerivative() const
	{
		List newSummands;
		for (Ptr ptr : summands)
		{
			newSummands.push_back(ptr->GetDerivative());
		}

		return Ptr(new Sum(newSummands))->Optimize();
	}
	double Sum::GetValue(double x) const
	{
		double f = 0;
		for (Ptr ptr : summands)
		{
			f += ptr->GetValue(x);
		}

		return f;
	}
	void Sum::Print() const
	{
		for (unsigned i = 0; i < summands.size(); i++)
		{
			if (i == 0)
			{
				summands[i]->Print();
			}
			else
			{
				std::cout << " + ";
				summands[i]->Print();
			}
		}
	}
	Function::Ptr Sum::Optimize() const
	{
		List newSummands;
		for (auto summand : summands) newSummands.push_back(summand->Optimize());

		bool change = false;
		auto c = 0.0;

		do
		{
			change = false;

			for (unsigned i = 0; i < newSummands.size(); i++)
			{
				if (newSummands[i]->GetType() == Type::Constant)
				{
					c += static_cast<Constant*>(newSummands[i].get())->GetValue(0.0);
					newSummands.erase(newSummands.begin() + i);
					i--;
					continue;
				}
				else if (newSummands[i]->GetType() == Type::Sum)
				{
					change = true;
					auto otherSummands = static_cast<Sum*>(newSummands[i].get())->GetSummands();
					for (auto summand : otherSummands) newSummands.push_back(summand->Optimize());

					newSummands.erase(newSummands.begin() + i);
					i--;
					continue;
				}
			}
		} while (change == true);

		if (newSummands.empty())
		{
			return Ptr(new Constant(c));
		}

		if (c != 0.0)
		{
			newSummands.push_back(Ptr(new Constant(c)));
		}

		return Ptr(new Sum(newSummands));
	}



	Function::Ptr Product::GetDerivative() const
	{
		List* products = new List[factors.size()];
		for (unsigned i = 0; i < factors.size(); i++)
		{
			for (unsigned j = 0; j < factors.size(); j++)
			{
				if (i == j)
				{
					products[i].push_back(factors[j]->GetDerivative());
				}
				else
				{
					products[i].push_back(factors[j]);
				}
			}
		}

		List newFactors;
		for (unsigned i = 0; i < factors.size(); i++)
		{
			newFactors.push_back(Ptr(new Product(products[i])));
		}
		delete[] products;

		return Ptr(new Sum(newFactors))->Optimize();
	}
	double Product::GetValue(double x) const
	{
		double f = 1;
		for (Ptr ptr : factors)
		{
			f *= ptr->GetValue(x);
		}

		return f;
	}
	void Product::Print() const
	{
		for (unsigned i = 0; i < factors.size(); i++)
		{
			if (i == 0)
			{
				std::cout << "(";
				factors[i]->Print();
				std::cout << ")";
			}
			else
			{
				std::cout << " * ";
				std::cout << "(";
				factors[i]->Print();
				std::cout << ")";
			}
		}
	}
	Function::Ptr Product::Optimize() const
	{
		List newFactors;
		for (auto factor : factors) newFactors.push_back(factor->Optimize());

		bool change = false;
		auto c = 1.0;

		do
		{
			change = false;
			List sums;

			for (unsigned i = 0; i < newFactors.size(); i++)
			{
				if (newFactors[i]->GetType() == Type::Constant)
				{
					c *= static_cast<Constant*>(newFactors[i].get())->GetValue(0.0);
					
					if (c == 0.0)
					{
						return Ptr(new Constant(0.0));
					}

					newFactors.erase(newFactors.begin() + i);
					i--;
					continue;
				}
				else if (newFactors[i]->GetType() == Type::Product)
				{
					change = true;
					auto otherFactors = static_cast<Product*>(newFactors[i].get())->GetFactors();
					for (auto factor : otherFactors) newFactors.push_back(factor->Optimize());

					newFactors.erase(newFactors.begin() + i);
					i--;
					continue;
				}
				// TODO: handle parenthesis
				/*else if (newFactors[i]->GetType() == Type::Sum)
				{
					sums.push_back(newFactors[i]);
					newFactors.erase(newFactors.begin() + i);
					i--;
					continue;
				}*/
			}
		} while (change == true);

		if (newFactors.empty())
		{
			return Ptr(new Constant(c));
		}

		if (c != 1.0)
		{
			newFactors.push_back(Ptr(new Constant(c)));
		}

		return Ptr(new Product(newFactors));
	}



	Function::Ptr Constant::GetDerivative() const
	{
		return Ptr(new Constant(0));
	}
	double Constant::GetValue(double x) const
	{
		return c;
	}
	void Constant::Print() const
	{
		std::cout << c;
	}
	Function::Ptr Constant::Optimize() const
	{
		return Ptr(new Constant(c));
	}
	


	Function::Ptr Variable::GetDerivative() const
	{
		return Ptr(new Constant(1));
	}
	double Variable::GetValue(double x) const
	{
		return x;
	}
	void Variable::Print() const
	{
		std::cout << "x";
	}
	Function::Ptr Variable::Optimize() const
	{
		return Ptr(new Variable());
	}
	


	Function::Ptr Power::GetDerivative() const
	{
		if (expo == 1.0)
		{
			return base->Optimize();
		}
		else
		{
			return Ptr(new Product({
				base->GetDerivative(),
				Ptr(new Product({ Ptr(new Constant(expo)), Ptr(new Power(base, expo - 1)) }))
			}))->Optimize();
		}
	}
	double Power::GetValue(double x) const
	{
		return pow(base->GetValue(x), expo);
	}
	void Power::Print() const
	{
		std::cout << "(";
		base->Print();
		std::cout << ")^" << expo;
	}
	Function::Ptr Power::Optimize() const
	{
		if (expo == 0)
		{
			return Ptr(new Constant(1));
		}

		if (expo == 1)
		{
			return base->Optimize();
		}

		auto newBase = base->Optimize();
		if (newBase->GetType() == Type::Constant)
		{
			return Ptr(new Constant(pow(static_cast<Constant*>(newBase.get())->GetValue(0.0), expo)));
		}
		else
		{
			return Ptr(new Power(newBase, expo));
		}
	}
	


	Function::Ptr Exp::GetDerivative() const
	{
		return Ptr(new Product({
			expo->GetDerivative(),
			Ptr(new Exp(expo))
		}))->Optimize();
	}
	double Exp::GetValue(double x) const
	{
		return exp(expo->GetValue(x));
	}
	void Exp::Print() const
	{
		std::cout << "e^(";
		expo->Print();
		std::cout << ")";
	}
	Function::Ptr Exp::Optimize() const
	{
		auto newExpo = expo->Optimize();

		if (newExpo->GetType() == Type::Constant)
		{
			return Ptr(new Constant(exp(static_cast<Constant*>(newExpo.get())->GetValue(0.0))));
		}
		else
		{
			return Ptr(new Exp(newExpo));
		}
	}
	


	Function::Ptr Ln::GetDerivative() const
	{
		return Ptr(new Product({
			arg->GetDerivative(),
			Ptr(new Power(arg, -1))
		}))->Optimize();
	}
	double Ln::GetValue(double x) const
	{
		return log(arg->GetValue(x));
	}
	void Ln::Print() const
	{
		std::cout << "ln^(";
		arg->Print();
		std::cout << ")";
	}
	Function::Ptr Ln::Optimize() const
	{
		auto newArg = arg->Optimize();

		if (newArg->GetType() == Type::Constant)
		{
			return Ptr(new Constant(log(static_cast<Constant*>(newArg.get())->GetValue(0.0))));
		}
		else
		{
			return Ptr(new Ln(newArg));
		}
	}
	


	Function::Ptr Sin::GetDerivative() const
	{
		return Ptr(new Product({
			Ptr(new Cos(arg)),
			arg->GetDerivative()
		}))->Optimize();
	}
	double Sin::GetValue(double x) const
	{
		return sin(arg->GetValue(x));
	}
	void Sin::Print() const
	{
		std::cout << "sin(";
		arg->Print();
		std::cout << ")";
	}
	Function::Ptr Sin::Optimize() const
	{
		auto newArg = arg->Optimize();

		if (newArg->GetType() == Type::Constant)
		{
			return Ptr(new Constant(sin(static_cast<Constant*>(newArg.get())->GetValue(0.0))));
		}
		else
		{
			return Ptr(new Sin(newArg));
		}
	}
	


	Function::Ptr Cos::GetDerivative() const
	{
		return Ptr(new Product({
			Ptr(new Product({ Ptr(new Constant(-1)), Ptr(new Sin(arg)) })),
			arg->GetDerivative()
		}))->Optimize();
	}
	double Cos::GetValue(double x) const
	{
		return cos(arg->GetValue(x));
	}
	void Cos::Print() const
	{
		std::cout << "cos(";
		arg->Print();
		std::cout << ")";
	}
	Function::Ptr Cos::Optimize() const
	{
		auto newArg = arg->Optimize();

		if (newArg->GetType() == Type::Constant)
		{
			return Ptr(new Constant(cos(static_cast<Constant*>(newArg.get())->GetValue(0.0))));
		}
		else
		{
			return Ptr(new Cos(newArg));
		}
	}
	


	Function::Ptr Tan::GetDerivative() const
	{
		return Ptr(new Product({
			Ptr(new Power(Ptr(new Cos(arg)), 2)),
			arg->GetDerivative()
		}))->Optimize();
	}
	double Tan::GetValue(double x) const
	{
		return tan(arg->GetValue(x));
	}
	void Tan::Print() const
	{
		std::cout << "tan(";
		arg->Print();
		std::cout << ")";
	}
	Function::Ptr Tan::Optimize() const
	{
		auto newArg = arg->Optimize();

		if (newArg->GetType() == Type::Constant)
		{
			return Ptr(new Constant(tan(static_cast<Constant*>(newArg.get())->GetValue(0.0))));
		}
		else
		{
			return Ptr(new Tan(newArg));
		}
	}
}
