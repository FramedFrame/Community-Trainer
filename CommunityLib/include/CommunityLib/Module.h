#pragma once
#include <memory.h>
#include <string>
#include <vector>
#include <Windows.h>

#include "Info.h"

namespace Plugin
{
	enum class PluginFunction
	{
		EntryPoint,DeletePoint,Enable,Disable
	};
	class Module
	{
	public:
		Module(std::string str);
		~Module(void);

		bool Load();
		bool Free();
		bool IsValid();

		bool Call(PluginFunction func);

		Info* operator()();
	private:
		HMODULE m_pluginModule;
		Info* m_pluginInfo;

		std::string m_strPath;
	};
}

