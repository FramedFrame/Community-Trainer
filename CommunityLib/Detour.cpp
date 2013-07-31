#include "Detour.h"

#include <Windows.h>
#include <detours.h>

using namespace Memory;


bool SetHook(void** ppFunction,void* pHook,bool fStatus)
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
bool SetHook(DetourContext& detourCtx,bool f)
{
	return SetHook(detourCtx.pFunc,detourCtx.pHook,f);
}
bool SetHook(std::vector<DetourContext>& vDetourCtx,bool f)
{
	if (DetourTransactionBegin() != NO_ERROR)
		return false;

	if (DetourUpdateThread(GetCurrentThread()) == NO_ERROR)
	{
		for(DetourContext& detourCtx: vDetourCtx)
			if ((f ? DetourAttach : DetourDetach)(detourCtx.pFunc, detourCtx.pHook) != NO_ERROR)
			{
				DetourTransactionAbort();
				return false;
			}

		if (DetourTransactionCommit() == NO_ERROR)
			return true;

	}
	DetourTransactionAbort();
	return false;
}

Detour::~Detour()
{

}

bool Detour::enable()
{
	return SetHook(this->m_context,true);
}
bool Detour::disable()
{
	return SetHook(this->m_context,false);
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
	procResult.second = processInfo;
	return procResult;
}

DetourList::~DetourList()
{

}

void Memory::DetourList::Add( DetourContext& dCtx )
{
	if(this->Status())
		return;

	this->m_vContext.push_back(dCtx);
}

void Memory::DetourList::Remove( DetourContext& dCtx )
{
	if(this->Status())
		return;

	auto x = std::find(this->m_vContext.begin(),this->m_vContext.end(),dCtx);

	if(x == this->m_vContext.end())
		return;

	this->m_vContext.erase(x);
}

void Memory::DetourList::Clear()
{
	if(this->Status())
		return;
	this->m_vContext.clear();
}

bool Memory::DetourList::enable()
{
	std::vector<DetourContext> vdCtx(this->m_vContext.begin(),this->m_vContext.end());
	return SetHook(vdCtx,true);
}

bool Memory::DetourList::disable()
{
	std::vector<DetourContext> vdCtx(this->m_vContext.begin(),this->m_vContext.end());
	return SetHook(vdCtx,false);
}
