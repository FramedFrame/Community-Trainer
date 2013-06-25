#pragma once
#include <stdint.h>
#include <string>
#include <vector>

#include "Token.h"

namespace Quertz
{
	enum class VariableType
	{
		INVALID,NUMBER,FLOAT,STRING
	};
	class Variable
	{
	public:
		Variable();
		~Variable(void);

		std::string AsString();
		int AsNumber();
		float AsFloat();

		bool Set(Token t);
		void SetString(std::string str);
		void SetNumber(int n);
		void SetFloat(float f);

		VariableType operator()();
	private:
		std::string m_strData;
		std::vector<uint8_t> m_vData;
		VariableType m_varType;
	};
}

