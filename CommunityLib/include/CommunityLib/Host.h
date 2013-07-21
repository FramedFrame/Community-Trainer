#pragma once
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Module.h"
#include "HostStruct.h"
namespace Plugin
{
	class Host
	{
	public:
		Host(void);
		~Host(void);

		bool LoadModule(std::string& strSource);
		bool ReleaseModule(std::string& strSource); 

		std::shared_ptr<HostStruct> operator()();

		static std::string ExtractFilename(std::string& strSource);

	private:
		std::map<std::string,std::shared_ptr<Module>> m_mapModules;
		std::shared_ptr<HostStruct> m_hostStruct;
	};
}

