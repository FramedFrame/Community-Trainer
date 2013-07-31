#include "FunctionPool.h"

#define LOCK() std::unique_lock<std::mutex>(this->m_mtxLock)

using namespace std;

bool Plugin::FunctionPool::Add( string str,poolfunction_t p )
{
	LOCK();
	if(this->m_mapFunctionPool.find(str) == this->m_mapFunctionPool.end())
	{
		this->m_mapFunctionPool[str] = make_pair(this->GetModule(),p);
		return true;
	}
	return false;
}

bool Plugin::FunctionPool::Remove( string str )
{
	LOCK();
	auto y = this->m_mapFunctionPool.find(str);
	if(y != this->m_mapFunctionPool.end())
	{
		this->m_mapFunctionPool.erase(y);
		return true;
	}
	return false;
}

void Plugin::FunctionPool::Clear()
{
	LOCK();
	this->m_mapFunctionPool.clear();
}

void Plugin::FunctionPool::ClearModule( HMODULE hModule )
{
	LOCK();
	std::vector<string> vErase;
	for(auto&x:this->m_mapFunctionPool)
	{
		if(x.second.first == hModule)
			vErase.push_back(x.first);
	}

	for(auto&s:vErase)
		this->m_mapFunctionPool.erase(s);
}

Plugin::poolfunction_t* Plugin::FunctionPool::operator()( string str )
{
	return this->Get(str);
}

Plugin::poolfunction_t* Plugin::FunctionPool::Get(string str)
{
	LOCK();
	auto y = this->m_mapFunctionPool.find(str);
	if(y != this->m_mapFunctionPool.end())
	{
		return &y->second.second;
	}
	return NULL;
}
