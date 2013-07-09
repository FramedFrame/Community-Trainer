#pragma once
#include "ToogleAble.h"
#include <Windows.h>
#include <string>
#include <list>
#include <vector>

namespace Memory
{
	struct DetourContext
	{
		void** pFunc;
		void* pHook;
		bool operator==(DetourContext const& rhs)
		{
			return !memcmp(this,&rhs,sizeof(DetourContext));
		}
	};

	class Detour : public ToogleAble
	{
	public:
		Detour(DetourContext detourContext)
		{
			this->m_context = detourContext;
		}
		~Detour(void)
		{

		}
#ifndef PLUGIN
		static std::pair<bool,PROCESS_INFORMATION> CreateProcessWithDll(std::string strExecuteable,std::string strLibary);
#endif
	protected:
		virtual bool enable();
		virtual bool disable();

	private:
		DetourContext m_context;
	};

	class DetourList : public ToogleAble
	{
	public:
		DetourList(void)
		{
		}
		~DetourList(void)
		{

		}

		virtual void Add(DetourContext& dCtx);
		virtual void Remove(DetourContext& dCtx);
		virtual void Clear();

	protected:
		virtual bool enable();
		virtual bool disable();

	private:
		std::list<DetourContext> m_vContext;
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

