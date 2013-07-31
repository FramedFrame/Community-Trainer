#pragma once
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace Plugin
{
	typedef std::vector<std::string> pluginPool_t;

	class PluginPool
	{
	public:
		PluginPool(void)
		{

		}
		~PluginPool(void)
		{

		}

		virtual bool Require(std::string strName);

		virtual void Add(std::string strName);
		virtual void Remove(std::string strRemove);
	private:
		std::mutex m_mtxLock;
		pluginPool_t m_pluginPool;
	};
}

