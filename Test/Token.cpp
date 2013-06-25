#include "Token.h"

using namespace Quertz;

Token::Token(TOKENS token,std::string _customField,Operator _op)
{
	this->m_token = token;
	this->CustomField = _customField;
	this->OperatorField = _op;
}
TOKENS Token::operator()()
{
	return this->m_token;
}


Token::~Token(void)
{
}
