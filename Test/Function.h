#pragma once
#include <map>
#include <stdint.h>
#include <string>
#include <vector>

#include "Variable.h"
#include "Instruction.h"

namespace Quertz
{
	class Function
	{
	public:
		Function(void);
		~Function(void);

		void AddInstruction(Instruction& instruction);
		Variable& AddParameter(std::string str);
		Variable& AddVariable(std::string str);
	private:
		std::map<std::string,Variable> m_mapParameter,m_mapLocalVariable;
		std::vector<Instruction> m_vInstruction;
	};
}

