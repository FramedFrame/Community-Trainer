#include "ToogleAble.h"

using namespace Memory;

ToogleAble::ToogleAble(void)
{
	this->m_fStatus = false;
}


ToogleAble::~ToogleAble(void)
{
}


bool ToogleAble::Enable(void)
{
	return this->Toogle(true);
}


bool ToogleAble::Disable(void)
{
	return this->Toogle(false);
}


bool ToogleAble::Toogle(void)
{
	return this->Toogle(!this->m_fStatus);
}


bool ToogleAble::Toogle(bool fToogle)
{
	if(this->m_fStatus == fToogle)
		return false;

	if(!fToogle?this->disable():this->enable())
	{
		this->m_fStatus = fToogle;
		return true;
	}
	return false;
}

