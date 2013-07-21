#include <functional>
#include <Windows.h>
#include <memory>
#include <vector>
#include <unordered_map>
#include <map>
#include <string>
#include <iostream>

#include <v8.h>



#pragma region stuff

#define MAKE_TEXT(__x) #__x

#define DECLARE_FUNCTION(__callConv,__nameFunc) typedef void*(


enum class CallingConv
{
	NONE,FASTCALL,STDCALL,_CDECL,THISCALL
};
std::map<std::string,CallingConv> mConvMap;


void MakeMap()
{
	mConvMap[MAKE_TEXT(__fastcall)] = CallingConv::FASTCALL;
	mConvMap[MAKE_TEXT(__stdcall)] = CallingConv::STDCALL;
	mConvMap[MAKE_TEXT(__cdecl)] = CallingConv::_CDECL;
	mConvMap[MAKE_TEXT(__thiscall)] = CallingConv::THISCALL;
}

struct FunctioContext
{
	std::vector<std::string> vParameter;
	CallingConv callingConv;
	std::string strReturn;
	void* pFunc;
};

void* CallCdecl(void* pFunc,int* pParList,int nSize)
{
	void* ppresult = 0;
	std::size_t uFix = nSize*4;
	_asm
	{
		pushad
			mov  esi,pParList
			test esi,esi
			jz DoneLooping
PushValues:
		push dword ptr [esi]
		add  esi,4
			cmp [esi],0
			jz DoneLooping
			jmp PushValues
DoneLooping:
		call pFunc
			mov  ppresult,eax
			add  esp,uFix
			popad

	}

	return ppresult;
}
void* CallStdCall(void* pFunc,int* pParList)
{
	void* ppresult = 0;

	_asm
	{
		pushad
			mov  esi,pParList
			test esi,esi
			jz DoneLooping
PushValues:
		push dword ptr [esi]
		add  esi,4
			cmp [esi],0
			jz DoneLooping
			jmp PushValues
DoneLooping:
		call pFunc
			mov  ppresult,eax
			popad

	}

	return ppresult;
}
struct FastcallParameter
{
	int* pParList;
	void* pEcx;
	void* pEdx;
};
void* CallFastCall(void* pFunc,FastcallParameter fsPar)
{
	void* ppresult = 0;

	void* pEcx = fsPar.pEcx;
	void* pEdx = fsPar.pEdx;
	void* pParList = fsPar.pParList;

	_asm
	{
		pushad
			mov ecx,pEcx
			test ecx,ecx
			jz DoneLooping

			mov edx,pEdx
			test edx,edx
			jz DoneLooping


			mov  esi,pParList
			test esi,esi
			jz DoneLooping
PushValues:
		cmp [esi],0
			jz DoneLooping
			push dword ptr [esi]
		add  esi,4
			jmp PushValues
DoneLooping:
		call pFunc
			mov  ppresult,eax
			popad

	}

	return ppresult;
}
void* CallThisCall(void* pFunc,FastcallParameter fsPar)
{
	void* ppresult = 0;

	void* pEcx = fsPar.pEcx;
	void* pParList = fsPar.pParList;

	_asm
	{
		pushad
			mov ecx,pEcx
			test ecx,ecx
			jz DoneLooping


			mov  esi,pParList
			test esi,esi
			jz DoneLooping
PushValues:
		cmp [esi],0
			jz DoneLooping
			push dword ptr [esi]
		add  esi,4
			jmp PushValues
DoneLooping:
		call pFunc
			mov  ppresult,eax
			popad

	}

	return ppresult;
}

void* CallThisCall(void* pFunc,void* pStart,void* pEnd)
{
	void* ppresult = 0;

	_asm
	{
		pushad
			mov ecx,[pStart]
		mov ecx,[ecx]
		lea  esi,pEnd

			lea eax,pStart
			add eax,4

			cmp esi,eax
			jle DoneLooping
PushValues:
		cmp esi,eax
			jle DoneLooping
			push dword ptr [esi]
		sub  esi,4
			jmp PushValues
DoneLooping:
		call pFunc
			mov  ppresult,eax
			popad

	}

	return ppresult;
}
int* BuildParameterList(std::vector<void*>& vPar,int nOffset = 0)
{
	std::size_t sizePar = vPar.size();
	if(sizePar == 0)
		return NULL;

	if(sizePar <= nOffset)
		return NULL;

	int* pParameterList = new int[sizePar+1-nOffset];
	std::size_t uIndex = 0;



	for(void* v:vPar)
	{
		if(nOffset)
		{
			nOffset--;
			continue;
		}
		pParameterList[uIndex] = (int)v;
		uIndex++;
	}

	pParameterList[sizePar-nOffset] = 0;
	return pParameterList;
}
FastcallParameter BuildFsPar(std::vector<void*> vPar,bool fThisCall = false)
{
	FastcallParameter fsPar;
	memset(&fsPar,0,sizeof(FastcallParameter));

	std::size_t uSize = fThisCall?1:2;

	if(vPar.size()>=1)
		fsPar.pEcx = vPar[0];
	else
		return fsPar;
	if(!fThisCall)
		if(vPar.size()>=2)
			fsPar.pEdx = vPar[1];
		else
			return fsPar;

	if(vPar.size()>uSize)
	{
		std::size_t uParameterSize = vPar.size() - uSize;
		if(!uParameterSize)
			return fsPar;

		fsPar.pParList = new int[uParameterSize+1];
		fsPar.pParList[uParameterSize] = 0;
		std::size_t uIndex = 0;
		while(uParameterSize)
		{
			std::size_t uAccessor = (fThisCall?uParameterSize-1:uIndex);
			fsPar.pParList[uAccessor] = (int)vPar[vPar.size()-uParameterSize];
			uIndex++;
			uParameterSize--;
		}
		return fsPar;
	}
	else
		return fsPar;
}

void* CallFunc(FunctioContext& funcCtx,std::vector<void*> vPar)
{
	int* parList = NULL;
	switch(funcCtx.callingConv)
	{
	case CallingConv::_CDECL:
		std::reverse(vPar.begin(),vPar.end());
		parList = BuildParameterList(vPar);
		return CallCdecl(funcCtx.pFunc,parList,vPar.size());
	case CallingConv::STDCALL:
		std::reverse(vPar.begin(),vPar.end());
		parList = BuildParameterList(vPar);
		return CallStdCall(funcCtx.pFunc,parList);
	case CallingConv::FASTCALL:
		return CallFastCall(funcCtx.pFunc,BuildFsPar(vPar));
	case CallingConv::THISCALL:
		return CallThisCall(funcCtx.pFunc,BuildFsPar(vPar,true));
	default:
		return 0;
	}
}

template<typename T> void* GetFuncPtr(T t)
{
	_asm
	{
		mov eax,[t]
	}
}

template<typename T>
FunctioContext FunctionCon(T t)
{


	std::string str = typeid(T).name();
	std::string strBuffer = "";
	FunctioContext funcCon;

	funcCon.pFunc = GetFuncPtr(t);

	int nFlag = 0;

	std::vector<bool> vOpened;

	for(char c:str)
	{
		if(nFlag == 0 && c == '(')
		{
			if(!strBuffer.compare("void"))
				funcCon.strReturn = strBuffer;
			else
				funcCon.strReturn = "NONE";
			strBuffer = "";
			nFlag++;
		}
		else if((nFlag == 1 || nFlag == 2) && c == '(')
			continue;
		else if(nFlag == 1 && c == ')')
		{
			std::size_t s = strBuffer.find_first_of(' ');
			if(s != -1)
				strBuffer = strBuffer.substr(0,s);

			if(strBuffer[strBuffer.size()-1] == '*')
				strBuffer = strBuffer.substr(0,strBuffer.size()-1);

			funcCon.callingConv = mConvMap[strBuffer];

			strBuffer = "";
			nFlag++;
		}
		else if(nFlag == 2 && c == '<')
		{
			vOpened.push_back(true);
			strBuffer += c;
		}
		else if(nFlag == 2 && c == '>')
		{
			vOpened.pop_back();
			strBuffer += c;
		}
		else if(nFlag == 2 && c == ')' && vOpened.size() == 0)
		{
			if(!strBuffer.empty() && strBuffer.compare("void") != 0)
				funcCon.vParameter.push_back(strBuffer);
			strBuffer = "";
			nFlag++;
		}
		else if(nFlag == 2 && c == ',' && vOpened.size() == 0)
		{
			if(!strBuffer.empty() && strBuffer.compare("void") != 0)
				funcCon.vParameter.push_back(strBuffer);
			strBuffer = "";
		}
		else
			strBuffer += c;

	}
	return funcCon;
}

int nq(int n,double d,std::string str)
{
	return n*n;
}

int __fastcall rrrr(int q,int r,int s,int ss)
{
	return q + r +s +ss;
}

class abc
{
public:
	void Print(int nOffset,char* s)
	{
		printf("%i,%i,%s\n",abccc,nOffset,s);
	}
	int abccc;
};
#pragma endregion


// enum class Tokens
// {
// 	INVALID,DECLATE_FUNCTION,DECLARE_VARIABLE,SET_VARIABLE,CLOSE_FUNCTION,STRING,OPERATOR,FUNCTION_BODY,IF,ELSEIF,ELSE
// };
// 
// class Token
// {
// public:
// 	Token(Tokens token)
// 	{
// 		this->m_token = token;
// 	}
// 	Tokens operator()()
// 	{
// 		return this->m_token;
// 	}
// 	std::string TokenText;
// private:
// 	Tokens m_token;
// };
// 
// std::map<Tokens,Token*> mapToken;
// 
// #define DECLARE_TOKEN(__t,__data,__txt) class Token_##__t : public Token\
// {\
// public:\
// 	Token_##__t() : Token(Tokens::__t)\
// {	this->TokenText = __txt; }\
// ##__data;\
// 	\
// };
// 
// 
// enum class Operator
// {
// 	INVALID,
// 	EQUAL,EQUAL_GREATER,EQUAL_LOWER,
// 	GREATER,LOWER,NOT_EQUAL,
// 	SET,SET_ADD,SET_SUB,
// 	ADD,SUB,MUL,DIV
// };
// std::shared_ptr<Token> ParseOperator(class Tokenizer* pT,std::pair<const std::string,Operator> &op);
// std::shared_ptr<Token> ParseDeclareVariable(class Tokenizer* pT);
// std::shared_ptr<Token> ParseDeclareFunction(class Tokenizer* pT);
// std::shared_ptr<Token> ParseString(class Tokenizer* pT);
// void ParseCommentSt(class Tokenizer* pT);
// void ParseCommentNd(class Tokenizer* pT);
// 
// 
// struct Parameter
// {
// 	std::string Name;
// 	int32_t Id;
// 
// 	Parameter(std::string _name,int32_t _id)
// 	{
// 		Name = _name;
// 		Id = _id;
// 	}
// };
// 
// 
// std::map<std::string,Operator> mapOperator;
// 
// 
// #define CHAR_END (char)0xFF
// #define CHAR_SKIPPED (char)' '
// 
// void MakeOperatorMap()
// {
// 	mapOperator["=="] == Operator::EQUAL; mapOperator[">="] = Operator::EQUAL_GREATER; mapOperator["<="] = Operator::EQUAL_LOWER;
// 	mapOperator[">"] = Operator::GREATER; mapOperator["<"] = Operator::LOWER; mapOperator["<>"] = Operator::NOT_EQUAL;
// 	mapOperator["="] = Operator::SET; mapOperator["+="] = Operator::SET_ADD; mapOperator["-="] = Operator::SET_SUB;
// 	mapOperator["+"] = Operator::ADD; mapOperator["-"] = Operator::SUB; mapOperator["*"] = Operator::MUL; mapOperator["/"] = Operator::DIV;
// }
// 
// DECLARE_TOKEN(DECLATE_FUNCTION,std::vector<Parameter> Parameter;std::string Name,"func");
// DECLARE_TOKEN(DECLARE_VARIABLE,std::string Name;int32_t Id,"var");
// DECLARE_TOKEN(STRING,std::string Text,"\"");
// DECLARE_TOKEN(OPERATOR,Operator Operator;std::vector<std::string> Involved,"op");
// DECLARE_TOKEN(FUNCTION_BODY,std::vector<std::string> Involved,"{");
// DECLARE_TOKEN(IF,int n,"if");
// DECLARE_TOKEN(ELSEIF,int n,"elif");
// DECLARE_TOKEN(ELSE,int n,"else");
// 
// 
// 
// void GenerateTokenMap()
// {
// 	mapToken[Tokens::DECLATE_FUNCTION] = new Token_DECLATE_FUNCTION();
// }
// 
// 
// class Tokenizer
// {
// public:
// 	Tokenizer()
// 	{
// 		this->m_uSize = 0;
// 		this->m_strScript = "";
// 		this->m_fAllowAll = false;
// 
// 		this->m_nLastVariableId = 0;
// 		this->m_nFunctionBlockId = 0;
// 
// 		MakeOperatorMap();
// 	}
// 
// 	~Tokenizer()
// 	{
// 		this->ResetTokens();
// 	}
// 
// 	int32_t GetNextVariableId()
// 	{
// 		this->m_nLastVariableId++;
// 		return this->m_nLastVariableId;
// 	}
// 	int32_t GetNextFunctionBlockId()
// 	{
// 		this->m_nFunctionBlockId++;
// 		return this->m_nFunctionBlockId;
// 	}
// 
// 	void ResetTokens()
// 	{
// 		this->m_mapTokens.clear();
// 	}
// 
// 	std::size_t AviableCharacter()
// 	{
// 		return this->m_strScript.size() - this->m_uSize;
// 	}
// 
// 
// 	int IsStringEqual(std::string strEqual)
// 	{
// 		if(this->AviableCharacter() < strEqual.size())
// 			return 0;
// 
// 		std::size_t u = 0;
// 		for(char c:strEqual)
// 		{
// 			if(c != this->m_strScript[u+this->m_uSize])
// 				return 0;
// 			u++;
// 		}
// 		return strEqual.size();
// 	}
// 
// 	bool SkipAble(char c)
// 	{
// 		if(this->m_fAllowAll)
// 			return false;
// 
// 		if(m_vAllowSkip.size())
// 		{
// 			auto it = std::find(this->m_vAllowSkip.begin(),this->m_vAllowSkip.end(),c);
// 			if(it != this->m_vAllowSkip.end())
// 			{
// 				this->m_vAllowSkip.erase(it);
// 				return true;
// 			}
// 		}
// 
// 		switch(c)
// 		{
// 		case '\r':
// 		case '\n':
// 		case '\t':
// 		case ' ':
// 			return true;
// 		default:
// 			return false;
// 		}
// 	}
// 
// 	char GetNext(bool fReturnDirect = false)
// 	{
// 		char c = CHAR_END;
// 		while(this->m_uSize  < this->m_strScript.size())
// 		{
// 			c = this->m_strScript[this->m_uSize];
// 			this->m_uSize++;
// 			if(this->SkipAble(c))
// 				if(fReturnDirect)
// 					return CHAR_SKIPPED;
// 				else
// 					c = CHAR_END;
// 			else
// 				break;
// 		}
// 		return c;
// 	}
// 
// 	char GetPrev(bool fReturnDirect = false)
// 	{
// 		char c = CHAR_END;
// 		while(this->m_uSize)
// 		{
// 			c = this->m_strScript[this->m_uSize];
// 			this->m_uSize--;
// 			if(this->SkipAble(c))
// 				if(fReturnDirect)
// 					return CHAR_SKIPPED;
// 				else
// 					c = CHAR_END;
// 			else
// 				break;
// 		}
// 		return c;
// 	}
// 
// 	std::size_t& operator()()
// 	{
// 		return this->m_uSize;
// 	}
// 
// 	void operator()(std::size_t u)
// 	{
// 		this->m_uSize = u;
// 	}
// 
// 	void TokenizeFunctionBlock()
// 	{
// 		bool fIsFunctionBlock = false;
// 
// 		std::vector<std::shared_ptr<Token>> vToken;
// 
// 		char c = 0,cPrev = 0;
// 		std::string strBuffer = "";
// 		strBuffer.reserve(64);
// 		Token_DECLARE_VARIABLE* var;
// 		while((c = this->GetNext()) != CHAR_END)
// 		{
// 			strBuffer += c;
// 			if(!strBuffer.compare("func"))
// 				vToken.push_back(ParseDeclareFunction(this));
// 			else if(!strBuffer.compare("if"))
// 				vToken.push_back(std::make_shared<Token_IF>());
// 			else if(!strBuffer.compare("var"))
// 				vToken.push_back(ParseDeclareVariable(this));
// 			else if(!strBuffer.compare("\""))
// 				vToken.push_back(ParseString(this));
// 			else if(!strBuffer.compare("#*"))
// 				ParseCommentNd(this);
// 			else if(!strBuffer.compare("#"))
// 				ParseCommentSt(this);
// 			else if(c == '{')
// 				this->TokenizeFunctionBlock();
// 			else if(c == '}')
// 			{
// 				this->m_mapTokens[this->GetNextFunctionBlockId()] = vToken;
// 				return;
// 			}
// 			else
// 			{
// 				bool fHandled = false;
// 				for(auto x:mapOperator)
// 				{
// 					if((x.first.size() == 1 && c == x.first[0]) ||
// 						(x.first.size() == 2 && c == x.first[0] && cPrev == x.first[1]))
// 					{
// 						vToken.push_back(ParseOperator(this,x));
// 						fHandled = true;
// 					}
// 				}
// 				if(!fHandled)
// 				{
// 					cPrev = c;
// 					continue;
// 				}
// 				c = '0';
// 			}
// 			cPrev = c;
// 			strBuffer = "";
// 		}
// 		this->m_mapTokens[this->GetNextFunctionBlockId()] = vToken;
// 		return;
// 	}
// 
// 	std::map<int,std::vector<std::shared_ptr<Token>>> Tokenize(const std::string& strScript)
// 	{
// 		this->m_strScript = strScript;
// 		this->m_uSize = 0;
// 
// 		this->ResetTokens();
// 		this->TokenizeFunctionBlock();
// 		return this->m_mapTokens;
// 	}
// 
// 	void PushAllowSkip(char c)
// 	{
// 		this->m_vAllowSkip.push_back(c);
// 	}
// 
// 	void ToogleAllow(bool f)
// 	{
// 		this->m_fAllowAll = f;
// 	}
// 
// 
// private:
// 	std::string m_strScript;
// 	std::size_t m_uSize;
// 	std::vector<char> m_vAllowSkip;
// 
// 	std::map<int,std::vector<std::shared_ptr<Token>>> m_mapTokens;
// 
// 	bool m_fAllowAll;
// 
// 	int32_t m_nLastVariableId,m_nFunctionBlockId;
// 
// };
// std::shared_ptr<Token> ParseOperator(Tokenizer* pT,std::pair<const std::string,Operator> &op)
// {
// 	bool fEnd = false,fHasFirst = false;
// 	std::string strBuffer = "";
// 	strBuffer.reserve(64);
// 	uint32_t nSkip = op.first.size()+1;
// 	char c = '0';
// 	std::size_t uIndex = (*pT)();
// 	std::size_t uStoredIndex = uIndex;
// 	auto pToken = std::make_shared<Token_OPERATOR>();
// 
// 	while(!fEnd && (c = pT->GetPrev(true)) != CHAR_END)
// 	{
// 		if(nSkip)
// 			nSkip--;
// 		else
// 		{
// 			if(!isalnum(c) && fHasFirst )//Not anymore 
// 				fEnd = true;
// 			else if(isalnum(c))
// 			{
// 				strBuffer += c;
// 				fHasFirst = true;
// 			}
// 		}
// 	}
// 
// 	if(!fEnd)
// 		return pToken;//Throw
// 
// 	std::reverse(strBuffer.begin(),strBuffer.end());
// 	pToken->Involved.push_back(strBuffer);
// 	(*pT)(uStoredIndex);
// 
// 	strBuffer = "";
// 	fEnd = false;
// 	fHasFirst = false;
// 
// 	while(!fEnd && (c = pT->GetNext(true)) != CHAR_END)
// 	{
// 		if(!isalnum(c) && fHasFirst)
// 			fEnd = true;
// 		else if(isalnum(c))
// 		{
// 			strBuffer += c;
// 			fHasFirst = true;
// 		}
// 	}
// 
// 	(*pT)()--;
// 
// 	pToken->Involved.push_back(strBuffer);
// 	pToken->Operator = op.second;
// 
// 
// 	return pToken;
// }
// std::shared_ptr<Token> ParseDeclareVariable(Tokenizer* pT)
// {
// 	bool fEnd = false,fHasFirst = false;
// 	std::string strBuffer = "";
// 	strBuffer.reserve(64);
// 	char c = '0';
// 
// 	auto pToken = std::make_shared<Token_DECLARE_VARIABLE>();
// 	while(!fEnd && (c = pT->GetNext(true)) != CHAR_END)
// 	{
// 		if(!isalnum(c) && fHasFirst)
// 			fEnd = true;
// 		else if(isalnum(c))
// 		{
// 			strBuffer += c;
// 			fHasFirst = true;
// 		}
// 	}
// 
// 	pToken->Name = strBuffer;
// 	pToken->Id = pT->GetNextVariableId();
// 	return pToken;
// }
// void ParseCommentNd(Tokenizer* pT)
// {
// 	bool fEnd = false;
// 	char c = '0',cPrev = '0';
// 
// 	pT->ToogleAllow(true);
// 	while(!fEnd && (c = pT->GetNext()) != CHAR_END)
// 	{
// 		if(c == '*')
// 			cPrev = c;
// 		else if(c == '*' && cPrev == '#')
// 			fEnd = true;
// 		else
// 			cPrev = '0';
// 	}
// 	pT->ToogleAllow(false);
// }
// void ParseCommentSt(Tokenizer* pT)
// {
// 	bool fEnd = false;
// 	char c = '0';
// 
// 	pT->ToogleAllow(true);
// 	while( !fEnd && (c = pT->GetNext()) != CHAR_END)
// 	{
// 		if(c == '\n')
// 			fEnd = true;
// 	}
// 	pT->ToogleAllow(false);
// }
// std::shared_ptr<Token> ParseString(Tokenizer* pT)
// {
// 	bool fEnd = false;
// 	std::string strBuffer = "";
// 	strBuffer.reserve(64);
// 	char c = '0';
// 
// 	auto pToken = std::make_shared<Token_STRING>();
// 	pT->ToogleAllow(true);
// 	while( !fEnd && (c = pT->GetNext()) != CHAR_END)
// 	{
// 		if(c == '\"')
// 			fEnd = true;
// 		else
// 			strBuffer += c;
// 	}
// 	pToken->Text = strBuffer;
// 	pT->ToogleAllow(false);
// 	return pToken;
// }
// std::shared_ptr<Token> ParseDeclareFunction(Tokenizer* pT)
// {
// 	bool fEnd = false;
// 	int32_t nLevel = 0;
// 	std::string strBuffer = "";
// 	strBuffer.reserve(64);
// 	char c = '0';
// 
// 	auto pToken = std::make_shared<Token_DECLATE_FUNCTION>();
// 	int32_t nId = 0;
// 
// 	while(!fEnd && (c = pT->GetNext()) != CHAR_END)
// 	{
// 		if(!nLevel)
// 			if(c == '(')
// 			{
// 				nLevel++;
// 				pToken->Name = strBuffer;
// 				strBuffer = "";
// 			}
// 			else
// 				strBuffer += c;
// 		else if(nLevel == 1)
// 			if(c == ',')
// 			{
// 				pToken->Parameter.push_back(Parameter(strBuffer,nId++));
// 				strBuffer = "";
// 			}
// 			else if(c == ')')
// 			{
// 				fEnd = true;
// 				pToken->Parameter.push_back(Parameter(strBuffer,nId++));
// 				break;
// 			}
// 			else
// 				strBuffer += c;
// 	}
// 
// 	return pToken;
// }
// #include <iostream>
// void Print(Token_DECLATE_FUNCTION* dec)
// {
// 	std::cout << "Function: " << dec->Name << " [" << dec->Parameter.size() << "]:";
// 	for(auto s:dec->Parameter)
// 	{
// 		std::cout << " " << s.Name;
// 		if(!s.Name.compare(dec->Parameter[dec->Parameter.size()-1].Name))
// 			std::cout << std::endl;
// 		else
// 			std::cout << " : ";
// 	}
// }
// void Print(Token_OPERATOR* op)
// {
// 	std::string str;
// 	for(auto yy:mapOperator)
// 		if(yy.second == op->Operator)
// 			str = yy.first;
// 	std::cout << "Operator: !| " << str << " |! {" << op->Involved.size() << "}: " << std::endl;
// 	int i = 1;
// 	for(auto y :op->Involved)
// 	{
// 		std::cout << "Involved[" << i << "]:"<< y;
// 		std::cout << std::endl;
// 		i++;
// 	}
// }
// void DeclareVariable(Token_DECLARE_VARIABLE* var)
// {
// 	std::cout << "Declare Variable " << var->Name << " , ID: " << var->Id << std::endl;
// }
// 
// void aaString(Token_STRING* str)
// {
// 	std::cout << "String " << str->Text << std::endl;
// }
// 
// void PrintTokens(std::map<int,std::vector<std::shared_ptr<Token>>> m)
// {
// 	for(auto x:m)
// 	{
// 		std::cout << "Token Scope: " << x.first << std::endl;
// 		for(auto y:x.second)
// 		{
// 			Token t = *y;
// 			Tokens tt = t();
// 			switch(tt)
// 			{
// 			case Tokens::DECLATE_FUNCTION:
// 				Print((Token_DECLATE_FUNCTION*)y.get());
// 				break;
// 			case Tokens::OPERATOR:
// 				Print((Token_OPERATOR*)y.get());
// 				break;
// 			case Tokens::DECLARE_VARIABLE:
// 				DeclareVariable((Token_DECLARE_VARIABLE*)y.get());
// 				break;
// 			case Tokens::STRING:
// 				aaString((Token_STRING*)y.get());
// 				break;
// 
// 
// 			}
// 		}
// 		std::cout << "End Scope: " << x.first << std::endl;
// 	}
// }
// 
// 
// class Variable
// {
// public:
// 	Variable()
// 	{
// 
// 	}
// 	~Variable()
// 	{
// 
// 	}
// };


// int CallFuncLua(lua_State* l,FunctioContext fCon)
// {
// 	int32_t nArgCount = lua_gettop(l);
// 
// 	std::vector<void*> v;
// 	double d = 0;
// 	for(int u=0; u< nArgCount;u++)
// 	{
// 		void* p = NULL;
// 		int i = lua_gettop(l);
// 		int t = lua_type(l, i);
// 		switch(t)
// 		{
// 		case LUA_TSTRING:
// 			p = (void*)lua_tostring(l,i);
// 			break;
// 		case LUA_TBOOLEAN:
// 			p = (void*)lua_toboolean(l,i);
// 			break;
// 		case LUA_TNUMBER:
// 			d = lua_tonumber(l,i);
// 			p = (void*)&d;
// 			break;
// 		default:
// 			p = lua_touserdata(l,i);
// 			break;
// 		}
// 		v.push_back(p);
// 		lua_pop(l, 1);
// 	}
// 	int n = (int)CallFunc(fCon,v);
// 
// 	lua_pushinteger(l,n);
// 	return 1;
// }

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"Winmm.lib")


void Print(const v8::FunctionCallbackInfo<v8::Value>& args);


v8::Handle<v8::Context> CreateShellContext(v8::Isolate* isolate) {
	// Create a template for the global object.
	v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New();
	global->Set(v8::String::New("print"), v8::FunctionTemplate::New(Print));

	return v8::Context::New(isolate, NULL, global);
}

v8::Handle<v8::Value> CallFunction(v8::Handle<v8::Context> context,std::vector<void*> vParameter,std::string str)
{
	using namespace v8;

	auto fn = Handle<Function>::Cast(context->Global()->Get(String::New(str.c_str())));
	std::vector<Handle<Value>> vArguments;
	for(void* v:vParameter)
		vArguments.push_back(String::New((char*)v));

	return fn->Call(context->Global(),vArguments.size(),vArguments.data());
}

void DoV8(std::string str)
{
	using namespace v8;
	try
	{
		// Get the default Isolate created at startup.
		Isolate* isolate = Isolate::GetCurrent();

		// Create a stack-allocated handle scope.
		HandleScope handle_scope(isolate);

		// Create a new context.
		Handle<Context> context = CreateShellContext(isolate);

		// Here's how you could create a Persistent handle to the context, if needed.
		Persistent<Context> persistent_context(isolate, context);

		// Enter the created context for compiling and
		// running the hello world script. 
		Context::Scope context_scope(context);

		// Create a string containing the JavaScript source code.
		Handle<String> source = String::New(str.c_str());

		// Compile the source code.
		Handle<Script> script = Script::Compile(source);

		// Run the script to get the result.
		Handle<Value> result = script->Run();
		std::string str1 = "Neger1",
			str2 = " is also a Neger";
		std::vector<void*> vv;
		vv.push_back((void*)str1.c_str());
		vv.push_back((void*)str2.c_str());
		auto y = CallFunction(context,vv,"test_function");

		// The persistent handle needs to be eventually disposed.
		persistent_context.Dispose();

		// Convert the result to an ASCII string and print it.
		String::AsciiValue ascii(result);
		printf("1: %s\n", *ascii);

		String::AsciiValue ascii2(y);
		printf("2: %s\n", *ascii2);
	}
	catch(...)
	{
		std::cout << "Some Error" << std::endl;
	}

	// Convert the result to an ASCII string and print it.
	//	String::AsciiValue ascii(result);
	//printf("%s\n", *ascii);

}

#define PAR_ARRAY(__type,__name,...) __type __nameArray[] = {__VA_ARGS__};\
	std::vector<__type> __name(__nameArray,__nameArray + sizeof(__nameArray) / sizeof(__type))


int main()
{
	PAR_ARRAY(std::string,vv,"nnn","nnn","sdasd");

	for(auto& s:vv)
		std::cout << s << std::endl;
// 	MakeMap();
// 	DoV8("print('abc'); 'Hello' + ', World!'; function test_function(str1,str2) { return str1 + str2; } ");//Script

	getchar();
	return 0;
}

// Extracts a C string from a V8 Utf8Value.
const char* ToCString(const v8::String::Utf8Value& value) {
	return *value ? *value : "<string conversion failed>";
}

void Print(const v8::FunctionCallbackInfo<v8::Value>& args) {
	bool first = true;
	for (int i = 0; i < args.Length(); i++) {
		v8::HandleScope handle_scope(args.GetIsolate());
		if (first) {
			first = false;
		} else {
			printf(" ");
		}
		v8::String::Utf8Value str(args[i]);
		const char* cstr = ToCString(str);
		printf("%s", cstr);
	}
	printf("\n");
}