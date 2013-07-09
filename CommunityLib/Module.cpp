#include "Module.h"

using namespace Plugin;

Plugin::Module::Module( std::string str )
{
	this->m_strPath = str;
	this->m_pluginModule = NULL;
}
Plugin::Module::~Module( void )
{

}

bool Plugin::Module::Load()
{
	if(this->IsValid())
		return false;

	this->m_pluginModule = LoadLibraryA(this->m_strPath.c_str());

	if(this->m_pluginModule == NULL)
		return false;

	FARPROC infoProc = GetProcAddress(this->m_pluginModule,"LoadPluginInfo");
	if(infoProc == NULL)
	{
		FreeLibrary(this->m_pluginModule);//Invalid Plugin Module
		return false;
	}

	auto infoFunc = reinterpret_cast<loadInfo_t>(infoProc);
	this->m_pluginInfo = infoFunc();

	if(this->m_pluginInfo == NULL)
	{
		FreeLibrary(this->m_pluginModule);//Invalid Plugin Module
		return false;
	}
	
	this->m_pluginInfo->EntryPoint(NULL);
}
bool Plugin::Module::Free()
{
	if(!this->IsValid())
		return false;

	if(this->m_pluginInfo->DeletePoint())
		return false;
	FreeLibrary(this->m_pluginModule);

	this->m_pluginInfo = NULL;
	this->m_pluginModule = NULL;
	return true;
}

bool Plugin::Module::IsValid()
{
	return this->m_pluginModule != NULL;
}

bool Plugin::Module::Call( PluginFunction func )
{
	if(!this->IsValid())
		return false;

	switch(func)
	{
	case PluginFunction::DeletePoint:
		return this->m_pluginInfo->DeletePoint();
	case PluginFunction::Disable:
		return this->m_pluginInfo->Disable();
	case PluginFunction::Enable:
		return this->m_pluginInfo->Enable();
	case PluginFunction::EntryPoint:
		this->m_pluginInfo->EntryPoint(NULL);
	default:
		return false;
	}
}

Info* Plugin::Module::operator()()
{
	return this->m_pluginInfo;
}
