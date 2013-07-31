#pragma once
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Any.h"
#include "Connection.h"
#include "FunctionPool.h"
#include "PluginPool.h"
#include "VariablePool.h"

namespace Plugin
{
	typedef Util::nana::any any;

	typedef std::function<any(std::vector<any>&)> poolfunction_t;
	typedef any poolvariable_t;

	struct HostStruct
	{
		std::shared_ptr<FunctionPool> functionPool;
		std::shared_ptr<VariablePool> variablePool;
		std::shared_ptr<PluginPool> pluginPool;
		std::shared_ptr<Connection> connection;
	};
}