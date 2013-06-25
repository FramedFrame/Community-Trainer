#include "Variable.h"

using namespace Quertz;

Variable::Variable()
{
}


Variable::~Variable(void)
{
}

std::string Variable::AsString()
{
	if(this->m_varType == VariableType::STRING)
		return this->m_strData;
	if(this->m_varType == VariableType::NUMBER)
		return std::to_string(this->AsNumber());
	if(this->m_varType == VariableType::FLOAT)
		return std::to_string(this->AsNumber());

	//Throw Exception
	return "";
}
int Variable::AsNumber()
{
	if(this->m_varType == VariableType::NUMBER)
		return *reinterpret_cast<int*>(this->m_vData.data());
	//Throw Exception
	return 0;
}
float Variable::AsFloat()
{
	if(this->m_varType == VariableType::FLOAT)
		return *reinterpret_cast<float*>(this->m_vData.data());
	//Throw Exception
	return 0;
}

VariableType Variable::operator()()
{
	return this->m_varType;
}

bool Variable::Set(Token t)
{
	if(t() == TOKENS::STRING)
		this->SetString(t.CustomField);
	else if(t() == TOKENS::DECIMAL)
		this->SetNumber(std::stoi(t.CustomField));
	else if(t() == TOKENS::HEX)
		this->SetNumber(std::stoi(t.CustomField));
	else if(t() == TOKENS::FLOAT)
		this->SetFloat(std::stof(t.CustomField));
	else
		return false;

	return true;
}

void Variable::SetString(std::string str)
{
	this->m_varType = VariableType::STRING;
	this->m_strData = str;
}
void Variable::SetNumber(int n)
{
	this->m_varType =  VariableType::NUMBER;
	this->m_vData.reserve(sizeof(int));
	memcpy(this->m_vData.data(),&n,sizeof(int));
}
void Variable::SetFloat(float f)
{
	this->m_varType = VariableType::FLOAT;
	this->m_vData.reserve(sizeof(float));
	memcpy(this->m_vData.data(),&f,sizeof(float));
}
