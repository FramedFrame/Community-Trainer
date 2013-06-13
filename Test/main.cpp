#include <functional>
#include <Windows.h>
#include <memory>
#include <vector>
#include <unordered_map>
#include <map>
#include <string>


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


enum class Tokens
{
	INVALID,DECLATE_FUNCTION,PARAMETER_FUNCTION,CLOSE_FUNCTION
};

class Token
{
public:
	Token(Tokens token)
	{
		this->m_token = token;
	}
	Token operator()()
	{
		return this->m_token;
	}
private:
	Tokens m_token;
};



#define DECLARE_TOKEN(__t,__data) class Token_##__t : Token\
	{\
	public:\
	Token_##__t() : Token(Tokens::DECLATE_FUNCTION)\
	{	}\
##__data;\
};



DECLARE_TOKEN(DECLARE_FUNCTION,std::vector<std::string> Parameter);



class Tokenizer
{
public:
	Tokenizer()
	{

	}

	~Tokenizer()
	{
		this->ResetTokens();
	}

	void ResetTokens()
	{
		for(Token* t:this->m_vTokens)
			delete t;

		this->m_vTokens.clear();
	}

	std::size_t AviableCharacter()
	{
		return this->m_strScript.size() - this->m_uSize;
	}


	int IsStringEqual(std::string strEqual)
	{
		if(this->AviableCharacter() < strEqual.size())
			return 0;

		std::size_t u = 0;
		for(char c:strEqual)
		{
			if(c != this->m_strScript[u+this->m_uSize])
				return 0;
			u++;
		}
		return strEqual.size();
	}

	std::vector<Token*> Tokenize(const std::string& strScript)
	{
		this->m_strScript = strScript;
		this->m_uSize = 0;

		this->ResetTokens();

		for(this->m_uSize = 0; this->m_uSize  < this->m_strScript.size();this->m_uSize++)
		{

		}
	}


private:
	std::string& m_strScript;
	std::size_t m_uSize;

	std::vector<Token*> m_vTokens;


};


int main()
{

	auto fx = FunctionCon(abc::Print);

	abc* aaa = new abc();
	std::vector<void*> v;
	v.push_back(aaa);
	CallFunc(fx,)
	getchar();
	return 0;
}
