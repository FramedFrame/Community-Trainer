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
		std::shared_ptr<HostStruct> Get();

		template<typename T> static Util::nana::any PluginCtor(Plugin::parameter_t& p)
		{
			return std::make_shared<T>();
		}
		template<typename T> static  Util::nana::any PluginCtor1(Plugin::parameter_t& p)
		{
			if(p.size() < 1)
				throw std::exception("Invalid Parameter Amount");
			return std::make_shared<T>(p[0]);
		}
		template<typename T> static Util::nana::any PluginCtor2(Plugin::parameter_t& p)
		{
			if(p.size() < 2)
				throw std::exception("Invalid Parameter Amount");
			return std::make_shared<T>(p[0],p[1]);
		}
		template<typename T> static  Util::nana::any PluginCtor3(Plugin::parameter_t& p)
		{
			if(p.size() < 3)
				throw std::exception("Invalid Parameter Amount");
			return std::make_shared<T>(p[0],p[1],p[2]);
		}


		static std::string ExtractFilename(std::string& strSource);
		static void Init(Plugin::Host& host);

	private:
		std::map<std::string,std::shared_ptr<Module>> m_mapModules;
		std::shared_ptr<HostStruct> m_hostStruct;
	};
}

