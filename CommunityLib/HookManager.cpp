#include "HookManager.h"

#include "Detour.h"

bool Plugin::HookManager::Toogle( bool f )
{
	if(this->m_f == f)
		return false;
	auto x = *reinterpret_cast<std::vector<Memory::DetourContext>*>(&this->m_vHooks);
	if(Memory::Detour::SetHook(x,this->m_f))
	{
		this->m_f = f;
		return true;
	}
	return false;
}

bool Plugin::HookManager::Toogle()
{
	return Toogle(!this->m_f);
}

void Plugin::HookManager::Add( DetourContext dCtx )
{
	this->m_vHooks.push_back(dCtx);
}

void Plugin::HookManager::Remove( DetourContext dCtx )
{
	auto x = std::find(this->m_vHooks.begin(),this->m_vHooks.end(),dCtx);
	if(x != this->m_vHooks.end())
		this->m_vHooks.erase(x);
}

void Plugin::HookManager::Clear()
{
	this->m_vHooks.clear();
}
