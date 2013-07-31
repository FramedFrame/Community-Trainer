#pragma once
#include <functional>
#include <map>
#include <mutex>

#include "Reader.h"

namespace Plugin
{
	typedef std::function<bool(IO::Reader& reader)> handlefunction_t;
	typedef std::map<uint8_t,handlefunction_t> handlemap_t;

	class Connection
	{
	public:
		Connection(void)
		{

		}
		~Connection(void)
		{

		}

		virtual void Handle(IO::Reader& reader);
		virtual void AttachHandler(uint8_t uOpcode,handlefunction_t handle);
		virtual void RemoveHandler(uint8_t uOpcode);
	private:
		std::mutex m_mtxLock;
		handlemap_t m_mapHandle;

	};
}

