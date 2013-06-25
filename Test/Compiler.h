#pragma once
#pragma once
#include <functional>
#include <map>
#include <memory>
#include <stack>
#include <string>
#include <stdint.h>
#include <vector>

#include "Operator.h"
#include "Token.h"
#include "Instruction.h"
#include "Variable.h"
#include "Function.h"

namespace Quertz
{
	class Compiler
	{
	public:
		Compiler(void);
		~Compiler(void);

		bool Compile(std::vector<Token> &vToken);
		std::map<std::string,std::vector<Instruction>>& operator()();
	private:
		std::vector<Token> m_vToken;
		std::stack<Function&> m_func;
		std::map<std::string,std::vector<Instruction>> m_mapInstruction;
		std::map<std::string,Function> m_mapFunction;
		std::stack<Variable&> m_variable;
		Token& m_lastToken;
		int32_t m_nIndex;

		bool CompileFunction();
		bool CompileVariable();
		bool CompileOperator();

		bool WorkBlock();

		Token Get(int nIndex);
		Token GetNext();

	};
}

