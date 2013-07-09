#pragma once

#include <stdint.h>
#include <string>

namespace Plugin
{
	typedef bool(* entryPoint_t)(void*);
	typedef bool(* deletePoint_t)();

	typedef bool(* pluginFunction_t)();

	enum class Type
	{
		Client,Server
	};

	struct Info
	{
		int Version,SubVersion;
		char Author[32],Name[64],Desc[256];
		Type Type;

		entryPoint_t EntryPoint;
		deletePoint_t DeletePoint;

		pluginFunction_t Enable,Disable;
	};

	typedef Info*(* loadInfo_t)();
}