#pragma once
#include <string>
#include "Operator.h"
namespace Quertz
{
	enum class TOKENS
	{
		INVALID,FUNCTION,DECIMAL,HEX,FLOAT,END,STRING,VARIABLE,OPERATOR,NAMED,IF,ELSE,TOKEN_LIST_END
	};

	class Token
	{
	public:
		Token(TOKENS token,std::string _customField = "",Operator op = Operator::INVALID);
		~Token(void);
		TOKENS operator()();
		std::string TokenText;
		Operator OperatorField;
		std::string CustomField;
	private:
		TOKENS m_token;
	};
}



