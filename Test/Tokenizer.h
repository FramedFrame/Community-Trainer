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


namespace Quertz
{
	class Tokenizer
	{
	public:
		Tokenizer(void);
		~Tokenizer(void);

		bool Tokenize();		
		bool Tokenize(std::string &strScript);

		std::vector<Token>& operator()()
		{
			return this->m_vToken;
		}

	private:
		std::map<std::string,TOKENS> m_mapTokens;
		std::map<char,Operator> m_mapOperator;
		std::stack<int32_t> m_stkIndex;

		std::vector<Token> m_vToken;
		std::string m_strScript;
		int32_t m_uIndex;
		std::string m_strBuffer;

		void GenerateMaps();

		void PushIndex();
		void PopIndex();

		char Next();
		char Prev();

		bool Work();
		bool IsValid(int32_t uSize);
		bool Equals(bool fReverse,int32_t uSize,...);
		bool Equals(std::string str,bool fReverse);
		bool IsSkipable(char c);
		bool CheckString(std::string& str,std::function<bool(char)> func);

		bool ExtractString();
		bool ExtractCommentLine();
		bool ExtractCommentBlock();
		void ExtractToken();
	};

	static const char CHAR_END = 0xFF;
}

