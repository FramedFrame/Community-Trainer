#pragma once
#include "Variable.h"
namespace Quertz
{
	enum class InstructionCode
	{
		INVALID,PUSH,POP,CMP
	};
	class Instruction
	{
	public:
		Instruction(void);
		~Instruction(void);

		operator()();//Run Instruction
	private:
		std::vector<std::shared_ptr<Variable>> vInvolvedVariable;
		InstructionCode m_instructionCode;
	};
}

