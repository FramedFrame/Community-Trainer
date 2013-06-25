#include "Tokenizer.h"

#include <stdarg.h>

using namespace Quertz;

Quertz::Tokenizer::Tokenizer( void )
{
	this->m_uIndex = 0;
	this->m_strScript = "";
	this->m_strBuffer = "";

	this->GenerateMaps();
}

Quertz::Tokenizer::~Tokenizer( void )
{

}

void Quertz::Tokenizer::GenerateMaps()
{
	this->m_mapTokens["func"] = TOKENS::FUNCTION;
	this->m_mapTokens["end"] = TOKENS::END;
	this->m_mapTokens["var"] = TOKENS::VARIABLE;
	this->m_mapTokens["if"] = TOKENS::IF;
	this->m_mapTokens["else"] = TOKENS::ELSE;

	this->m_mapOperator['='] = Operator::SET;

	this->m_mapOperator['!'] = Operator::NOT_EQUAL;

	this->m_mapOperator['+'] = Operator::ADD;
	this->m_mapOperator['-'] = Operator::SUB;
	this->m_mapOperator['*'] = Operator::MUL;
	this->m_mapOperator['/'] = Operator::DIV;

	this->m_mapOperator['['] = Operator::ARRAY_START;
	this->m_mapOperator[']'] = Operator::ARRAY_END;

	this->m_mapOperator['('] = Operator::BODY_START;
	this->m_mapOperator[')'] = Operator::BODY_END;

	this->m_mapOperator['>'] = Operator::GREATER;
	this->m_mapOperator['<'] = Operator::LOWER;

	this->m_mapOperator[':'] = Operator::STATIC_MEMBER;
	this->m_mapOperator['.'] = Operator::MEMBER;
	this->m_mapOperator[','] = Operator::SEPERATOR;
}

bool Quertz::Tokenizer::IsValid( int32_t uSize )
{
	int32_t uNewPos = this->m_uIndex + uSize;
	return uNewPos > 0 &&
		uNewPos < this->m_strScript.size();
}
bool Quertz::Tokenizer::IsSkipable(char c)
{
	switch(c)
	{
	case ' ':
	case '\r':
	case '\n':
	case '\t':
	case ';':
		return true;
	default:
		return false;
	}
}
bool Quertz::Tokenizer::CheckString(std::string& str,std::function<bool(char)> func)
{
	for(char c:str)
		if(!func(c))
			return false;
	return true;
}
bool Quertz::Tokenizer::Equals(bool fReverse,int32_t uSize,...)
{
	int32_t uMovement = fReverse?-1:1,
		_size = uSize,uIndex  = this->m_uIndex;
	bool fFailed = false;
	va_list vl;
	char c = '0',c1 = '0';

	if(uSize < 0)
		return false;
	if(!this->IsValid(fReverse?(-uSize):uSize))
		return false;
	if(fReverse)
		uIndex -= _size;

	va_start(vl,uSize);

	while(_size)
	{
		c1 = va_arg(vl,char);
		c = this->m_strScript[uIndex];

		if(c != c1)
		{
			fFailed = true;
			break;
		}

		uIndex++;
		_size--;
	}

	va_end(vl);

	return !fFailed;
}
bool Quertz::Tokenizer::Equals(std::string str,bool fReverse)
{
	int32_t uSize = str.size(),uIndex  = 0;
	char c = '0',*c1 = const_cast<char*>(str.c_str());
	bool fFailed = false;

	if(str.empty())
		return false;

	if(!this->IsValid(fReverse?-uSize:uSize))
		return false;
	if(fReverse)
		uIndex -= uSize;

	while(uSize)
	{
		c = this->m_strScript[uIndex];

		if(c != *c1)
		{
			fFailed = true;
			break;
		}

		uIndex++;
		uSize--;
		c1++;
	}
	return !fFailed;
}
bool Quertz::Tokenizer::Work()
{
	char c = this->Next();
	if(c == CHAR_END)
		return true;
	bool fHandled = false;

	Operator op = Operator::INVALID;
	TOKENS tk = TOKENS::INVALID;

	switch(c)
	{
	case '"':
		if(!this->ExtractString())
			return false;
		break;
	case '#':
		if(this->Equals(false,1,'*'))
		{
			if(!this->ExtractCommentBlock())
				return false;
		}
		else if(!this->ExtractCommentLine())
			return false;
		break;


	default:
		if((op = this->m_mapOperator[c]) != Operator::INVALID)
		{
			this->ExtractToken();
			this->m_vToken.push_back(Token(TOKENS::OPERATOR,"",op));
			this->m_strBuffer = "";
			break;
		}
		else if(this->IsSkipable(c))
		{
			this->ExtractToken();
			this->m_strBuffer = "";
			break;
		}

		this->m_strBuffer += c;
		break;
	}
	return Work();
}

bool Quertz::Tokenizer::Tokenize()
{
	if(this->m_strScript.empty())
		return false;

	this->m_strBuffer = "";
	this->m_strBuffer.reserve(64);
	this->m_uIndex = 0;

	return Work();
}

bool Quertz::Tokenizer::Tokenize( std::string &strScript )
{
	this->m_strScript = strScript;
	return this->Tokenize();
}

bool Quertz::Tokenizer::ExtractString()
{
	char c = '0';
	bool fFlag = false,fFound = false;
	std::string strBuffer = "";
	strBuffer.reserve(64);

	while((c = this->Next()) != CHAR_END)
	{
		if(fFlag && c == '\\')
		{
			fFlag = false;
			continue;
		}
		else if(c == '\\')
		{
			fFlag = true;
			continue;
		}
		else if(c == '"' && !fFlag)
			break;
		else if(fFlag)
			fFlag = false;

		strBuffer += c;
	}

	if(c == CHAR_END)
		return false;

	this->m_vToken.push_back(Token(TOKENS::STRING,strBuffer));
	return true;
}
bool Quertz::Tokenizer::ExtractCommentLine()
{
	char c = '0';
	bool fFound = false;
	std::string strBuffer = "";
	strBuffer.reserve(64);

	while((c = this->Next()) != CHAR_END)
	{
		if(c == '\n')
		{
			fFound = true;
			break;
		}
	}

	if(c == CHAR_END)
		return false;

	return true;
}
bool Quertz::Tokenizer::ExtractCommentBlock()
{
	this->m_uIndex += 1;//*

	char c = '0',cPrev = '0';
	bool fFound = false;
	std::string strBuffer = "";
	strBuffer.reserve(64);

	while((c = this->Next()) != CHAR_END)
	{
		if(cPrev == '*' && c == '#')
		{
			fFound = true;
			break;
		}
		cPrev = c;
	}

	if(c == CHAR_END)
		return false;

	return true;
}

char Quertz::Tokenizer::Next()
{
	if(!IsValid(1))
		return CHAR_END;
	char c = this->m_strScript[this->m_uIndex];
	this->m_uIndex++;
	return c;
}
char Quertz::Tokenizer::Prev()
{
	if(!IsValid(-1))
		return CHAR_END;
	this->m_uIndex--;
	char c = this->m_strScript[this->m_uIndex];
	return c;
}

void Quertz::Tokenizer::PushIndex()
{
	this->m_stkIndex.push(this->m_uIndex);
}
void Quertz::Tokenizer::PopIndex()
{
	this->m_uIndex = this->m_stkIndex.top();
	this->m_stkIndex.pop();
}

void Quertz::Tokenizer::ExtractToken()
{
	if(this->m_strBuffer.empty())
		return;

	TOKENS tk;
	if((tk = this->m_mapTokens[this->m_strBuffer]) != TOKENS::INVALID)
		this->m_vToken.push_back(Token(tk));
	else if(CheckString(this->m_strBuffer,isdigit))
		this->m_vToken.push_back(Token(TOKENS::DECIMAL,this->m_strBuffer));
	else if(this->m_strBuffer.size() > 3 && this->m_strBuffer.find("0x") == 0 && CheckString(this->m_strBuffer.substr(2),isxdigit))
		this->m_vToken.push_back(Token(TOKENS::HEX,this->m_strBuffer));
	else if(CheckString(this->m_strBuffer,[](char c)->bool{
		return isdigit(c) || c=='.';
	}))
		this->m_vToken.push_back(Token(TOKENS::FLOAT,this->m_strBuffer));
	else
		this->m_vToken.push_back(Token(TOKENS::NAMED,this->m_strBuffer));
}
