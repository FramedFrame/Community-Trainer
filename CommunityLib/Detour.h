#pragma once
#include "ToogleAble.h"
#include <Windows.h>
#include <string>

namespace Memory
{
	struct DetourContext
	{
		void** pFunc;
		void* pHook;
	};

	class Detour : public ToogleAble
	{
	public:
		Detour(DetourContext detourContext);
		~Detour(void);

		bool SetHook(void** ppFunction,void* pHook,bool fStatus);

		static std::pair<bool,PROCESS_INFORMATION> CreateProcessWithDll(std::string strExecuteable,std::string strLibary);
	protected:
		bool enable();
		bool disable();

	private:
		DetourContext m_context;
	};

	template<typename T>
	static T* MakeFuncPtr(T tFunc)
	{
		return &tFunc;
	}

	template<typename T>
	static DetourContext MakeDetourContext(T** tFuncPtr,T* tHook)
	{
		DetourContext detourContext;
		detourContext.pFunc = reinterpret_cast<void**>(tFuncPtr);
		detourContext.pHook = static_cast<void*>(tHook);
		return detourContext;
	}
}

#define MAKE_CTX(__t,__l) Memory::MakeDetourContext(&__t,(decltype(__t)(__l))) 
#define MAKE_API_PTR(__api,__name) decltype(&__api) __name = &__api
#define FAKE_EXPORT(__name) extern "C" __declspec( dllexport )void __name(void)

