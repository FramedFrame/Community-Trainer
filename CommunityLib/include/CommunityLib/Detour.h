#pragma once
#include "ToogleAble.h"

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
	static DetourContext MakeDetourContext(T* tFuncPtr,T tHook)
	{
		DetourContext detourContext;
		detourContext.pFunc = reinterpret_cast<void**>(tFuncPtr);
		detourContext.pHook = static_cast<void*>(&tHook);
		return detourContext;
	}
}

