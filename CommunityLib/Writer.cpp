#include "Writer.h"

using namespace IO;

Writer::Writer(void)
{
}


Writer::~Writer(void)
{
}

void Writer::WriteString( std::string str )
{
	uint16_t uSize = str.size();
	this->Write(uSize);
	for(char c:str)
		this->Write(c);
}

std::vector<uint8_t>& Writer::operator()()
{
	return this->m_vData;
}