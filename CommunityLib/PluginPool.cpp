#include "PluginPool.h"

using namespace Plugin;

bool PluginPool::Require(std::string strName)
{
	unique_lock<mutex>(this->m_mtxLock);

	auto x = std::find(this->m_pluginPool.begin(),this->m_pluginPool.end(),strName);
	return x != this->m_pluginPool.end();
}

void PluginPool::Add(std::string strName)
{
	unique_lock<mutex>(this->m_mtxLock);

	this->m_pluginPool.push_back(strName);
}
void PluginPool::Remove(std::string strName)
{
	unique_lock<mutex>(this->m_mtxLock);

	auto x = std::find(this->m_pluginPool.begin(),this->m_pluginPool.end(),strName);
	if(x != this->m_pluginPool.end())
		this->m_pluginPool.erase(x);
}