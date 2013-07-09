#pragma once
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <Windows.h>

#include "Any.h"

//#define POOL_FUNCTION(x) Plugin::any x(parameter_t& vParameter)

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

