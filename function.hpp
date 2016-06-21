// TODO: Function::Print should work with strings rather than stdout
// TODO: Sum::Optimize and Product::Optimize must summarize powers, variables and so on
#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include <memory>
#include <vector>



namespace analysis
{
	class Function
	{
	public:
		enum class Type
		{
			Sum,
			Product,
			Constant,
			Variable,
			Power,
			Exp,
			Ln,
			Sin,
			Cos,
			Tan
		};

		using Ptr = std::shared_ptr<Function>;
		using List = std::vector<Ptr>;

		virtual ~Function() {}

		virtual Ptr GetDerivative() const = 0;
		virtual double GetValue(double x) const = 0;
		virtual void Print() const = 0;
		virtual Ptr Optimize() const = 0;
		virtual Type GetType() const = 0;
	};



	class Sum : public Function
	{
	public:
		Sum(const List& summands) : summands(summands) {}

		Ptr GetDerivative() const override;
		double GetValue(double x) const override;
		void Print() const override;
		Ptr Optimize() const override;
		Type GetType() const override { return Type::Sum; }
		const List& GetSummands() const { return summands; }

	private:
		List summands;
	};



	class Product : public Function
	{
	public:
		Product(const List& factors) : factors(factors) {}

		Ptr GetDerivative() const override;
		double GetValue(double x) const override;
		void Print() const override;
		Ptr Optimize() const override;
		Type GetType() const override { return Type::Product; }
		const List& GetFactors() const { return factors; }

	private:
		List factors;
	};



	class Constant : public Function
	{
	public:
		Constant(double c) : c(c) {}

		Ptr GetDerivative() const override;
		double GetValue(double x) const override;
		void Print() const override;
		Ptr Optimize() const override;
		Type GetType() const override { return Type::Constant; }

	private:
		double c;
	};



	class Variable : public Function
	{
	public:
		Ptr GetDerivative() const override;
		double GetValue(double x) const override;
		void Print() const override;
		Ptr Optimize() const override;
		Type GetType() const override { return Type::Variable; }
	};



	class Power : public Function
	{
	public:
		Power(Ptr base, double expo) : base(base), expo(expo) {}

		Ptr GetDerivative() const override;
		double GetValue(double x) const override;
		void Print() const override;
		Ptr Optimize() const override;
		Type GetType() const override { return Type::Power; }

	private:
		Ptr base;
		double expo;
	};



	class Exp : public Function
	{
	public:
		Exp(Ptr expo) : expo(expo) {}

		Ptr GetDerivative() const override;
		double GetValue(double x) const override;
		void Print() const override;
		Ptr Optimize() const override;
		Type GetType() const override { return Type::Exp; }

	private:
		Ptr expo;
	};



	class Ln : public Function
	{
	public:
		Ln(Ptr arg) : arg(arg) {}

		Ptr GetDerivative() const override;
		double GetValue(double x) const override;
		void Print() const override;
		Ptr Optimize() const override;
		Type GetType() const override { return Type::Ln; }

	private:
		Ptr arg;
	};



	class Sin : public Function
	{
	public:
		Sin(Ptr arg) : arg(arg) {}

		Ptr GetDerivative() const override;
		double GetValue(double x) const override;
		void Print() const override;
		Ptr Optimize() const override;
		Type GetType() const override { return Type::Sin; }

	private:
		Ptr arg;
	};



	class Cos : public Function
	{
	public:
		Cos(Ptr arg) : arg(arg) {}

		Ptr GetDerivative() const override;
		double GetValue(double x) const override;
		void Print() const override;
		Ptr Optimize() const override;
		Type GetType() const override { return Type::Cos; }

	private:
		Ptr arg;
	};



	class Tan : public Function
	{
	public:
		Tan(Ptr arg) : arg(arg) {}

		Ptr GetDerivative() const override;
		double GetValue(double x) const override;
		void Print() const override;
		Ptr Optimize() const override;
		Type GetType() const override { return Type::Tan; }

	private:
		Ptr arg;
	};
}



#endif
