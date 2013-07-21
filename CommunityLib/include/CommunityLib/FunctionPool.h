#pragma once
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <Windows.h>

#include "Any.h"

//Generates a Parameter Vector
#define PAR_ARRAY(__type,__name,...) __type __nameArray[] = {__VA_ARGS__};\
	std::vector<__type> __name(__nameArray,__nameArray + sizeof(__nameArray) / sizeof(__type))

//nana::any IMPL
#define ANY_ARRAY(__name,...) PAR_ARRAY(Util::nana::any,__name,__VA_ARGS__)


namespace Plugin
{
	using namespace std;

	typedef Util::nana::any any;
	typedef vector<any> parameter_t;
	typedef function<any(parameter_t&)> poolfunction_t;
	typedef unordered_map<string,pair<HMODULE,poolfunction_t>> functionpool_t;

	class FunctionPool
	{
	public:
		FunctionPool(void)
		{
		}
		~FunctionPool(void)
		{

		}

		virtual HMODULE GetModule()
		{
			return GetModuleHandle(NULL);
		}

		virtual bool Add(string str,poolfunction_t p);
		virtual bool Remove(string str);
		virtual void Clear();
		virtual void ClearModule(HMODULE hModule);
		
		virtual poolfunction_t* operator()(string str);
	private:
		functionpool_t m_mapFunctionPool;
		mutex m_mtxLock;
	
	};
}

