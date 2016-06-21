#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>
#include "function.hpp"



namespace analysis
{
	struct Token
	{
		enum class Type
		{
			Constant,
			Variable,
			E,
			Ln,
			Sin,
			Cos,
			Tan,
			LParen,
			RParen,
			Plus,
			Minus,
			Star,
			Slash,
			PowerSign,
			None
		};

		Token() = default;
		Token(Type type) : type(type) {}
		Token(double value) : type(Type::Constant), value(value) {}

		Type type = Type::None;
		double value = 0.0;
	};

	class Tokenizer
	{
	public:
		void operator()(const std::string& term);
		void NextToken();
		const Token& PeekNextToken(unsigned offset = 1) const;
		const Token& GetCurToken() const;

	private:
		std::vector<Token> tokens;
		int curToken = -1;
	};



	class Parser
	{
	public:
		Function::Ptr operator()(const std::string& term);

	private:
		void Accept(Token::Type type);
		void UpdateCurToken();
		Function::Ptr ParseSum();
		Function::Ptr ParseProduct();
		// TODO: implement powers and exponentials
		//Function::Ptr ParsePowerExponential();
		Function::Ptr ParseParen();
		Function::Ptr ParseFunc();
		Function::Ptr ParseVariable();
		Function::Ptr ParseConstant();

		Tokenizer tokenizer;
		Token cur;
	};
}



#endif