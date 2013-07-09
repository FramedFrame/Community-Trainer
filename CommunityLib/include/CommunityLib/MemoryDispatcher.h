#pragma once
namespace Plugin
{
class MemoryDispatcher
{
public:
	MemoryDispatcher(void* pInstance)
	{

	}
	~MemoryDispatcher(void)
	{

	}

	virtual bool Enable(void);
	virtual bool Disable(void);
	virtual bool Toogle(void);
	virtual bool Toogle(bool fToogle);
};
}

