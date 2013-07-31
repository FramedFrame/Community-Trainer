#include "Connection.h"

using namespace Plugin;

#define LOCK std::unique_lock<std::mutex>(this->m_mtxLock)

void Connection::Handle(IO::Reader& reader)
{
	LOCK;
	try
	{
		uint8_t u = 0;
		reader >> u;

		auto x = this->m_mapHandle.find(u);
		if(x != this->m_mapHandle.end())
			x->second(reader);
	}
	catch(...)
	{
		//Print error probably out of index exception
	}
}
void Connection::AttachHandler(uint8_t uOpcode,handlefunction_t handle)
{
	LOCK;

	auto x = this->m_mapHandle.find(uOpcode);
	if(x == this->m_mapHandle.end())
		this->m_mapHandle[uOpcode] = handle;
	else
		return;//Return false...
}
void Connection::RemoveHandler(uint8_t uOpcode)
{
	LOCK;

	auto x = this->m_mapHandle.find(uOpcode);
	if(x != this->m_mapHandle.end())
		this->m_mapHandle.erase(x);
	else
		return;//Return false...
}
