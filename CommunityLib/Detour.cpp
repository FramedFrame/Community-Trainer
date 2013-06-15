#include "Detour.h"

#include <Windows.h>
#include <detours.h>

using namespace Memory;

Detour::Detour(DetourContext detourContext)
{
	this->m_context = detourContext;
}


Detour::~Detour(void)
{
}

bool Detour::SetHook(void** ppFunction,void* pHook,bool fStatus)
{
	if (DetourTransactionBegin() != NO_ERROR)
		return false;

	if (DetourUpdateThread(GetCurrentThread()) == NO_ERROR)
		if ((fStatus ? DetourAttach : DetourDetach)(ppFunction, pHook) == NO_ERROR)
			if (DetourTransactionCommit() == NO_ERROR)
				return true;

	DetourTransactionAbort();
	return false;
}

bool Detour::enable()
{
	return SetHook(this->m_context.pFunc,this->m_context.pHook,true);
}
bool Detour::disable()
{
	return SetHook(this->m_context.pFunc,this->m_context.pHook,false);
}

std::pair<bool,PROCESS_INFORMATION> Detour::CreateProcessWithDll(std::string strExecuteable,std::string strLibary)
{
	std::pair<bool,PROCESS_INFORMATION> procResult;

	procResult.first = false;

	STARTUPINFOA startInfo;
	memset(&startInfo,0,sizeof(STARTUPINFOA));
	startInfo.cb = sizeof(STARTUPINFOA);
	
	PROCESS_INFORMATION processInfo;
	memset(&procResult.second,0,sizeof(PROCESS_INFORMATION));

	if(!DetourCreateProcessWithDllExA(strExecuteable.c_str(),NULL,NULL,NULL,FALSE,CREATE_DEFAULT_ERROR_MODE,NULL,NULL,&startInfo,
		&processInfo,strLibary.c_str(),NULL))
		return procResult;

	procResult.first = true;
	return procResult;
}

