#include "parser.hpp"
#include <cctype>



namespace analysis
{
	// TODO: add exception-based error handling
	void Tokenizer::operator()(const std::string& term)
	{
		tokens.clear();
		curToken = -1;

		int curPos = -1;
		char curChar = 0;
		
		auto posValid = [&]() {
			return curPos < term.size(); 
		};
		auto nextChar = [&]() {
			curPos++;
			if (posValid()) curChar = term[curPos];
		};

		nextChar();
		while (posValid())
		{
			while (isspace(curChar) && posValid()) nextChar();

			switch (curChar)
			{
			case '(':
				tokens.push_back(Token(Token::Type::LParen));
				nextChar();
				continue;
			case ')':
				tokens.push_back(Token(Token::Type::RParen));
				nextChar();
				continue;
			case '+':
				tokens.push_back(Token(Token::Type::Plus));
				nextChar();
				continue;
			case '-':
				tokens.push_back(Token(Token::Type::Minus));
				nextChar();
				continue;
			case '*':
				tokens.push_back(Token(Token::Type::Star));
				nextChar();
				continue;
			case '/':
				tokens.push_back(Token(Token::Type::Slash));
				nextChar();
				continue;
			case '^':
				tokens.push_back(Token(Token::Type::PowerSign));
				nextChar();
				continue;
			}

			if (isalpha(curChar))
			{
				std::string buffer;
				while (isalpha(curChar) && posValid())
				{
					buffer += curChar;
					nextChar();
				}

				if (buffer == "x")
				{
					tokens.push_back(Token(Token::Type::Variable));
					continue;
				}
				else if (buffer == "e")
				{
					tokens.push_back(Token(Token::Type::E));
					continue;
				}
				else if (buffer == "ln")
				{
					tokens.push_back(Token(Token::Type::Ln));
					continue;
				}
				else if (buffer == "sin")
				{
					tokens.push_back(Token(Token::Type::Sin));
					continue;
				}
				else if (buffer == "cos")
				{
					tokens.push_back(Token(Token::Type::Cos));
					continue;
				}
				else if (buffer == "tan")
				{
					tokens.push_back(Token(Token::Type::Tan));
					continue;
				}
				else
				{
					exit(1);
				}
			}

			if (isdigit(curChar))
			{
				std::string buffer;

				while (isdigit(curChar) && posValid())
				{
					buffer += curChar;
					nextChar();
				}

				if (curChar == '.')
				{
					buffer += curChar;
					nextChar();

					if (isdigit(curChar))
					{
						while (isdigit(curChar) && posValid())
						{
							buffer += curChar;
							nextChar();
						}
					}
					else
					{
						exit(2);
					}
				}

				tokens.push_back(Token(atof(buffer.c_str())));
				continue;
			}

			exit(3);
		}
	}
	void Tokenizer::NextToken()
	{
		curToken++;
	}
	const Token& Tokenizer::PeekNextToken(unsigned offset) const
	{
		if (curToken >= 0 && curToken + offset < tokens.size())
		{
			return tokens[curToken + offset];
		}
		
		return Token();
	}
	const Token& Tokenizer::GetCurToken() const
	{
		if (curToken >= 0 && curToken < tokens.size())
		{
			return tokens[curToken];
		}

		return Token();
	}



	Function::Ptr Parser::operator()(const std::string& term)
	{
		tokenizer(term);
		UpdateCurToken();

		auto result = ParseSum();
		Accept(Token::Type::None);
		return result->Optimize();
	}
	void Parser::Accept(Token::Type type)
	{
		if (tokenizer.GetCurToken().type != type)
		{
			exit(4);
		}
		else
		{
			UpdateCurToken();
		}
	}
	void Parser::UpdateCurToken()
	{
		tokenizer.NextToken();
		cur = tokenizer.GetCurToken();
	}
	Function::Ptr Parser::ParseSum()
	{
		Function::List summands;
		summands.push_back(ParseProduct());

		while (cur.type == Token::Type::Plus || cur.type == Token::Type::Minus)
		{
			if (cur.type == Token::Type::Minus)
			{
				Accept(Token::Type::Minus);
				summands.push_back(Function::Ptr(new Product({
					Function::Ptr(new Constant(-1.0)),
					ParseProduct()
				})));
			}
			else
			{
				Accept(Token::Type::Plus);
				summands.push_back(ParseProduct());
			}
		}

		return Function::Ptr(new Sum(summands));
	}
	Function::Ptr Parser::ParseProduct()
	{
		Function::List factors;
		factors.push_back(ParseParen());

		while (cur.type == Token::Type::Star || cur.type == Token::Type::Slash)
		{
			if (cur.type == Token::Type::Slash)
			{
				Accept(Token::Type::Slash);
				factors.push_back(Function::Ptr(new Power(ParseParen(), -1.0)));
			}
			else
			{
				Accept(Token::Type::Star);
				factors.push_back(ParseParen());
			}
		}

		return Function::Ptr(new Product(factors));
	}
	//Function::Ptr Parser::ParsePowerExponential()
	//{

	//}
	Function::Ptr Parser::ParseParen()
	{
		bool minus = false;
		if (cur.type == Token::Type::Minus)
		{
			Accept(Token::Type::Minus);
			minus = true;
		}

		Function::Ptr result;
		if (cur.type == Token::Type::LParen)
		{
			Accept(Token::Type::LParen);
			result = ParseSum();
			Accept(Token::Type::RParen);
		}
		else
		{
			result = ParseFunc();
		}

		if (minus)
		{
			result = Function::Ptr(new Product({
				Function::Ptr(new Constant(-1)),
				result
			}));
		}

		return result;
	}
	Function::Ptr Parser::ParseFunc()
	{
		if (cur.type == Token::Type::Ln)
		{
			// TODO: try this with ParseParen instead of parsing parenthesis again
			Accept(Token::Type::Ln);
			Accept(Token::Type::LParen);
			auto arg = ParseSum();
			Accept(Token::Type::RParen);
			return Function::Ptr(new Ln(arg));
		}
		else if (cur.type == Token::Type::Sin)
		{
			Accept(Token::Type::Sin);
			Accept(Token::Type::LParen);
			auto arg = ParseSum();
			Accept(Token::Type::RParen);
			return Function::Ptr(new Sin(arg));
		}
		else if (cur.type == Token::Type::Cos)
		{
			Accept(Token::Type::Cos);
			Accept(Token::Type::LParen);
			auto arg = ParseSum();
			Accept(Token::Type::RParen);
			return Function::Ptr(new Cos(arg));
		}
		else if (cur.type == Token::Type::Tan)
		{
			Accept(Token::Type::Tan);
			Accept(Token::Type::LParen);
			auto arg = ParseSum();
			Accept(Token::Type::RParen);
			return Function::Ptr(new Tan(arg));
		}
			
		return ParseVariable();
	}
	Function::Ptr Parser::ParseVariable()
	{
		if (cur.type == Token::Type::Variable)
		{
			Accept(Token::Type::Variable);
			return Function::Ptr(new Variable());
		}
		else
		{
			return ParseConstant();
		}
	}
	Function::Ptr Parser::ParseConstant()
	{
		auto value = cur.value;
		Accept(Token::Type::Constant);
		return Function::Ptr(new Constant(value));
	}
}