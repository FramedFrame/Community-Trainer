#include "Compiler.h"

using namespace Quertz;

Compiler::Compiler(void) :
	m_lastToken(Token(TOKENS::INVALID))
{
	this->m_nIndex = 0;
}


Compiler::~Compiler(void)
{
}

bool Compiler::Compile(std::vector<Token> &vToken)
{
	this->m_nIndex = 0;
	this->m_vToken = vToken;

	this->m_mapInstruction.clear();
	this->m_mapFunction["main"] = Function();
	this->m_func.push(this->m_mapFunction["main"]);
	return this->WorkBlock();
}
std::map<std::string,std::vector<Instruction>>& Compiler::operator()()
{
	return this->m_mapInstruction;
}

Token Compiler::Get(int nIndex)
{
	static Token t(TOKENS::INVALID);
	static Token t2(TOKENS::TOKEN_LIST_END);

	if(0 > nIndex)
		return t;
	if(this->m_nIndex >= this->m_vToken.size())
		return  t2;

	return this->m_vToken[nIndex];
}
Token Compiler::GetNext()
{
	auto t = this->Get(this->m_nIndex);
	this->m_nIndex++;
	return t;
}
bool Compiler::CompileFunction()
{
	auto t = this->GetNext();
	if(t() != TOKENS::NAMED)
		return false;

	this->m_mapFunction[t.TokenText] = Function();
	this->m_func.push(this->m_mapFunction[t.TokenText]);

	t = this->GetNext();
	if(t() != TOKENS::OPERATOR && t.OperatorField != Operator::BODY_START)
		return false;

	Variable var;

	while(true)
	{
		t = this->GetNext();
		if(t() == TOKENS::NAMED)
			var = this->m_func.top().AddVariable(t.CustomField);
		else if(t() == TOKENS::END)
			break;
		else if(t() == TOKENS::OPERATOR && t.OperatorField == Operator::SEPERATOR);
		else
			return false;
	}

	return this->WorkBlock();
}
bool Compiler::CompileVariable()
{
	auto t = this->GetNext();
	if(t() != TOKENS::NAMED)
		return false;

	this->m_variable.push(this->m_func.top().AddParameter(t.CustomField));
	return true;
}
bool Compiler::WorkBlock()
{
	auto t = this->GetNext();

	if(t() == TOKENS::TOKEN_LIST_END)//End Reached
		return true;
	if(t() == TOKENS::INVALID)
		return false;

	switch(t())
	{
	case TOKENS::FUNCTION:
		if(!this->CompileFunction())
			return false;
		else
			this->m_func.pop();
	case TOKENS::VARIABLE:
		if(!this->CompileVariable())
			return false;
		case TOKENS::
		


	}
	return this->Work();
}
