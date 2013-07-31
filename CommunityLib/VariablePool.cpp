#include "VariablePool.h"

#define LOCK() std::unique_lock<std::mutex>(this->m_mtxLock)

void Plugin::VariablePool::Clear()
{
	LOCK();
	this->m_mapVariablePool.clear();
}

void Plugin::VariablePool::ClearModule( HMODULE hModule )
{
	LOCK();
	std::vector<std::string> vErase;
	for(auto& x:this->m_mapVariablePool)
	{
		if(x.second.first == hModule)
			vErase.push_back(x.first);
	}

	for(auto&s:vErase)
		this->m_mapVariablePool.erase(s);
}
