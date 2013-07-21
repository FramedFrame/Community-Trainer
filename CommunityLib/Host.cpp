#include "Host.h"

Plugin::Host::Host( void )
{
	this->m_hostStruct.reset(new HostStruct());
	this->m_hostStruct->functionPool.reset(new FunctionPool());
	this->m_hostStruct->pluginPool.reset(new PluginPool());
	this->m_hostStruct->variablePool.reset(new VariablePool());
}

Plugin::Host::~Host( void )
{

}

bool Plugin::Host::LoadModule( std::string& strSource )
{
	auto moduleName = Plugin::Host::ExtractFilename(strSource);

	if(this->m_mapModules.find(moduleName) != this->m_mapModules.end())//Module Already exists
		return false;

	auto y = this->m_mapModules[moduleName];
	y.reset(new Plugin::Module(strSource,this->m_hostStruct.get()));
	if(y->Load())
	{
		this->m_hostStruct->pluginPool->Add(strSource);
		return true;
	}
	return false;
}

bool Plugin::Host::ReleaseModule( std::string& strSource )
{
	if(this->m_mapModules.find(strSource) == this->m_mapModules.end())//Module doesnt exists
		return false;

	auto y = this->m_mapModules[strSource];
	bool f = y->Free();
	this->m_mapModules.erase(strSource);
	this->m_hostStruct->pluginPool->Remove(strSource);
	return f;
}

std::string Plugin::Host::ExtractFilename(std::string& strSource)
{
	auto x = strSource.find_last_of('\\');
	if(x == -1)
	{
		x = strSource.find_last_of('/');
		if(x == -1)
			return "";
	}

	return strSource.substr(x+1);
}

std::shared_ptr<Plugin::HostStruct> Plugin::Host::operator()()
{
	return this->m_hostStruct;
}

