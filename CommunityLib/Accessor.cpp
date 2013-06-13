#include "Accessor.h"

#include <stdarg.h>

using namespace Memory;

Accessor::Accessor(void)
{
}


Accessor::~Accessor(void)
{
}

std::vector<uint8_t> Memory::Accessor::Read( uint32_t uAddress,std::size_t uSize )
{
	std::vector<uint8_t> vData(uSize);
	this->read(uAddress,vData);
	return vData;
}

bool Memory::Accessor::Write( uint32_t uAddress,std::vector<uint8_t>& vData )
{
	return this->write(uAddress,vData);
}

bool Memory::Accessor::Write( uint32_t uAddress,std::size_t uSize,... )
{
	std::vector<uint8_t> vData(uSize);

	va_list va;
	va_start(va, uSize);
	uint8_t uByte = 0;

	for(std::size_t u = 0; u < uSize;u++)
	{
		uByte = va_arg(va,uint8_t);
		vData.push_back(uByte);
	}

	return this->write(uAddress,vData);
}

bool Memory::Accessor::Write( uint32_t uAddress,std::size_t uSize,uint8_t uData )
{
	std::vector<uint8_t> vData(uSize);

	for(std::size_t u = 0; u < uSize;u++)
		vData.push_back(uData);

	return this->write(uAddress,vData);
}

std::vector<Opcode> Memory::Accessor::Disassemble( uint32_t uAddress,std::size_t uSize )
{
	return DisassemblerInstance()->DisassembleInstructions(reinterpret_cast<uint8_t*>(uAddress),uSize);
}

bool Memory::Accessor::IsReadable(uint32_t uFlag)
{
	if (!uFlag|| uFlag & PAGE_GUARD)
		return false;

	if ((uFlag & PAGE_WRITECOPY) || (uFlag & PAGE_READWRITE) ||
		(uFlag & PAGE_READWRITE) || (uFlag & PAGE_EXECUTE_READWRITE) ||
		(uFlag & PAGE_EXECUTE_WRITECOPY))
		return true;

	return false;
}
bool  Memory::Accessor::IsWriteable(uint32_t uFlag)
{
	if (!uFlag || uFlag & PAGE_GUARD)
		return false;

	if ((uFlag & PAGE_EXECUTE_READ) || (uFlag & PAGE_EXECUTE_READWRITE) ||
		(uFlag & PAGE_READONLY) || (uFlag & PAGE_READWRITE))
		return true;
}


bool Memory::InternalAccessor::write( uint32_t uAddress,std::vector<uint8_t>& v,bool fFlagSet)
{
	LPVOID lpvAddress = reinterpret_cast<LPVOID>(uAddress);

	MEMORY_BASIC_INFORMATION mbi;
	memset(&mbi,0,sizeof(MEMORY_BASIC_INFORMATION));

	if(!VirtualQuery(lpvAddress,&mbi,v.size()))
		if(fFlagSet)
			return false;

	DWORD uOldProtect = -1;

	if(!IsWriteable(mbi.Protect))
	{
		if(!fFlagSet)
			return false;

		if(!VirtualProtect(lpvAddress,v.size(),PAGE_EXECUTE_READWRITE,&uOldProtect))
			return false;
	}

	memcpy_s(lpvAddress,v.size(),v.data(),v.size());

	if(uOldProtect != -1)
		if(!VirtualProtect(lpvAddress,v.size(),uOldProtect,&uOldProtect))
			return false;

	return true;
}

bool Memory::InternalAccessor::read( uint32_t uAddress,std::vector<uint8_t>& v,bool fFlagSet)
{
	LPVOID lpvAddress = reinterpret_cast<LPVOID>(uAddress);

	MEMORY_BASIC_INFORMATION mbi;
	memset(&mbi,0,sizeof(MEMORY_BASIC_INFORMATION));

	if(!VirtualQuery(lpvAddress,&mbi,v.size()))
		if(fFlagSet)
			return false;

	DWORD uOldProtect = -1;

	if(!IsReadable(mbi.Protect))
	{
		if(!fFlagSet)
			return false;

		if(!VirtualProtect(lpvAddress,v.size(),PAGE_EXECUTE_READWRITE,&uOldProtect))
			return false;
	}

	if(memcpy_s(v.data(),v.size(),lpvAddress,v.size()) != 0)
		return false;

	if(uOldProtect != -1)
		if(!VirtualProtect(lpvAddress,v.size(),uOldProtect,&uOldProtect))
			return false;

	return true;
}

Memory::ProcessAccessor::ProcessAccessor( HANDLE hProcess )
{
	this->m_hProcess = hProcess;
}

bool Memory::ProcessAccessor::write( uint32_t uAddress,std::vector<uint8_t>& v,bool fFlagSet)
{
	LPVOID lpvAddress = reinterpret_cast<LPVOID>(uAddress);

	MEMORY_BASIC_INFORMATION mbi;
	memset(&mbi,0,sizeof(MEMORY_BASIC_INFORMATION));

	if(!VirtualQueryEx(this->m_hProcess,lpvAddress,&mbi,v.size()))
		if(fFlagSet)
			return false;

	DWORD uOldProtect = -1;

	if(!IsWriteable(mbi.Protect))
	{
		if(!fFlagSet)
			return false;

		if(!VirtualProtectEx(this->m_hProcess,lpvAddress,v.size(),PAGE_EXECUTE_READWRITE,&uOldProtect))
			return false;
	}

	SIZE_T uWritten = 0;

	if(!WriteProcessMemory(this->m_hProcess,lpvAddress,v.data(),v.size(),&uWritten))
		return false;
	
	if(!uWritten)
		return false;

	if(uOldProtect != -1)
		if(!VirtualProtectEx(this->m_hProcess,lpvAddress,v.size(),uOldProtect,&uOldProtect))
			return false;

	return true;
}

bool Memory::ProcessAccessor::read( uint32_t uAddress,std::vector<uint8_t>& v,bool fFlagSet)
{
	LPVOID lpvAddress = reinterpret_cast<LPVOID>(uAddress);

	MEMORY_BASIC_INFORMATION mbi;
	memset(&mbi,0,sizeof(MEMORY_BASIC_INFORMATION));

	if(!VirtualQueryEx(this->m_hProcess,lpvAddress,&mbi,v.size()))
		if(fFlagSet)
			return false;

	DWORD uOldProtect = -1;

	if(!IsReadable(mbi.Protect))
	{
		if(!fFlagSet)
			return false;

		if(!VirtualProtectEx(this->m_hProcess,lpvAddress,v.size(),PAGE_EXECUTE_READWRITE,&uOldProtect))
			return false;
	}

	SIZE_T uRead = 0;

	if(!ReadProcessMemory(this->m_hProcess,lpvAddress,v.data(),v.size(),&uRead))
		return false;

	if(!uRead)
		return false;

	if(uOldProtect != -1)
		if(!VirtualProtectEx(this->m_hProcess,lpvAddress,v.size(),uOldProtect,&uOldProtect))
			return false;

	return true;
}


 void CreateAccessorInstance(uint32_t uPar = 0)
 {
#ifdef CLIENT
	 HANDLE h = OpenProcess(PROCESS_ALL_ACCESS,FALSE,uPar);
	 if(h == NULL)
		 return;//Fail Log

	 AccessorInstance.Create( new BasicAccessor(h));
#else
	 AccessorInstance.Create( new BasicAccessor());
#endif
 }
