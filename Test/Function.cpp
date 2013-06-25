#include "Function.h"

using namespace Quertz;

Function::Function(void)
{
}


Function::~Function(void)
{
}

void Function::AddInstruction(Instruction& instruction)
{
	this->m_vInstruction.push_back(instruction);
}
Variable& Function::AddParameter(std::string str)
{
	this->m_mapParameter[str] = Variable();
	return this->m_mapParameter[str];
}
Variable& Function::AddVariable(std::string str)
{
	this->m_mapLocalVariable[str] = Variable();
	return this->m_mapLocalVariable[str];
}
