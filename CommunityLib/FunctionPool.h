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
	typedef Util::nana::any any;
	typedef std::vector<any> parameter_t;
	typedef std::function<any(parameter_t&)> poolfunction_t;
	typedef std::unordered_map<std::string,std::pair<HMODULE,poolfunction_t>> functionpool_t;

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

		virtual bool Add(std::string str,poolfunction_t p);
		virtual bool Remove(std::string str);
		virtual void Clear();
		virtual void ClearModule(HMODULE hModule);
		
		virtual poolfunction_t* operator()(std::string str);
		virtual poolfunction_t* Get(std::string str);
	private:
		functionpool_t m_mapFunctionPool;
		std::mutex m_mtxLock;
	
	};
}

