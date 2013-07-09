#include "Reader.h"

#include <iostream>


void IO::Reader::ResetIndex()
{
	this->m_uIndex = 0;
}
uint32_t& IO::Reader::operator()()
{
	return this->m_uIndex;
}

bool IO::Reader::CanRead( uint32_t nIndex,uint32_t uSize )
{
	return (this->m_uIndex + uSize) <= this->m_vData.size();
}

bool IO::Reader::TryReadString(std::string &str,uint32_t uIndex)
{
	str.clear();
	uint16_t uSize = 0;
	if(this->TryRead(uSize,uIndex))
	{
		bool fMemberIndex = false;
		if(uIndex == -1)
		{
			uIndex = this->m_uIndex;
			fMemberIndex = true;
		}
		else
			uIndex += 2;

		if(this->CanRead(uIndex,uSize))
		{
			auto it = this->m_vData.begin() + uIndex;
			str.insert(str.begin(),it,it+uSize);
			if(fMemberIndex)
				this->m_uIndex += uSize;
			return true;
		}
		else if(fMemberIndex)
			this->m_uIndex -= 2;
	}
	return false;
}

std::string IO::Reader::ReadString( uint32_t uIndex )
{
	std::string str = "";
	if(this->TryReadString(str,uIndex))
		return str;
	else
		throw new std::exception("Out of Index");
}
