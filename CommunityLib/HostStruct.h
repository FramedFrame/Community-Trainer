#pragma once
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Any.h"
#include "FunctionPool.h"
#include "VariablePool.h"

namespace Plugin
{
	using namespace std;

	typedef Util::nana::any any;

	typedef function<any(vector<any>&)> poolfunction_t;
	typedef any poolvariable_t;

	struct HostStruct
	{
		std::shared_ptr<FunctionPool> functionPool;
		std::shared_ptr<VariablePool> variablePool;
	};
}