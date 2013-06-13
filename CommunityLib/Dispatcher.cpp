#include "Dispatcher.h"
#include "Accessor.h"
#include <stdarg.h>

using namespace Memory;

Dispatcher::Dispatcher( uint32_t uAddress,std::vector<uint8_t>& vEnableBytes ) :
	m_vEnableBytes(vEnableBytes.begin(),vEnableBytes.end())
{
	this->m_uAddress = uAddress;
}

Dispatcher::Dispatcher( uint32_t uAddress,std::size_t uSize,... ) :
	m_vEnableBytes(uSize)
{
	this->m_uAddress = uAddress;

	va_list va;
	va_start(va, uSize);
	uint8_t uByte = 0;

	for(std::size_t u = 0; u < uSize;u++)
	{
		uByte = va_arg(va,uint8_t);
		this->m_vEnableBytes.push_back(uByte);
	}

}

Dispatcher::Dispatcher( uint32_t uAddress,std::size_t uSize,uint8_t uData ) :
	m_vEnableBytes(uSize)
{
	this->m_uAddress = uAddress;

	for(std::size_t u = 0; u < uSize;u++)
		this->m_vEnableBytes.push_back(uData);
}

Dispatcher::Dispatcher(uint32_t uAddress,uint32_t uDestination,uint32_t* uReturn,Instruction instr)
{
	this->m_uAddress = uAddress;


	auto x = DisassemblerInstance()->DisassembleInstructions(reinterpret_cast<uint8_t*>(uAddress),20);

	if(!x.size())
		return;//To do make some error message

	std::size_t uSize = 0,uIndex = 0;

	while(uSize < 5 || uIndex < x.size())
	{
		uSize += x[uIndex].Size;
		uIndex++;
	}

	if(uSize < 5)
		return;

	this->m_vEnableBytes.resize(uSize);

	this->m_vEnableBytes[0] = instr == Instruction::Call ? 0xE8 : 0xE9;
	*(reinterpret_cast<uint32_t*>(&this->m_vEnableBytes.data()[1])) = this->CalculateJumpCall(this->m_uAddress,uDestination);

	for(uIndex = 0; uIndex < (uSize-5);uIndex++)
		this->m_vEnableBytes.data()[uIndex+5] = 0x90;

	if(uReturn != NULL)
		*uReturn = uAddress + uSize;

}


Dispatcher::~Dispatcher(void)
{
}

bool Dispatcher::enable()
{
	if(!this->m_vBackup.size())
	{
		this->m_vBackup = Memory::AccessorInstance()->Read(this->m_uAddress,this->m_vEnableBytes.size());
		if(!this->m_vBackup.size())
			return false;
	}
	return Memory::AccessorInstance()->Write(this->m_uAddress,this->m_vEnableBytes);
}

bool Dispatcher::disable()
{
	return Memory::AccessorInstance()->Write(this->m_uAddress,this->m_vBackup);
}


uint32_t Dispatcher::CalculateJumpCall(uint32_t uSource,uint32_t uDestination)
{
	return (uDestination - uSource)-5;
}