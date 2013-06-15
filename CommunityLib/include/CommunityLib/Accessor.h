#pragma once
#include <stdint.h>
#include <vector>
#include <Windows.h>

#include "Disassembler.h"
#include "Singleton.h"

namespace Memory
{
	class Accessor
	{
	public:
		Accessor(void);
		~Accessor(void);

		std::vector<uint8_t> Read(uint32_t uAddress,std::size_t uSize);
		bool Write(uint32_t uAddress,std::vector<uint8_t>& vData);
		bool Write(uint32_t uAddress,std::size_t uSize,...);
		bool Write(uint32_t uAddress,std::size_t uSize,uint8_t uData);

		std::vector<Opcode> Disassemble(uint32_t uAddress,std::size_t uSize);

	protected:
		virtual bool write(uint32_t,std::vector<uint8_t>&,bool = true) = 0;
		virtual bool read(uint32_t,std::vector<uint8_t>&,bool = true) = 0;

		bool IsReadable(uint32_t uFlag);
		bool IsWriteable(uint32_t uFlag);
	};

	class InternalAccessor : public Accessor
	{
	protected:
		virtual bool write(uint32_t,std::vector<uint8_t>&,bool);
		virtual bool read(uint32_t,std::vector<uint8_t>&,bool);
	};

	class ProcessAccessor : public Accessor
	{
	public:
		ProcessAccessor(HANDLE hProcess);
	protected:
		virtual bool write(uint32_t,std::vector<uint8_t>&,bool);
		virtual bool read(uint32_t,std::vector<uint8_t>&,bool);
	private:
		HANDLE m_hProcess;
	};

#ifdef CLIENT
	typedef ProcessAccessor BasicAccessor;
#else
	typedef InternalAccessor BasicAccessor;
#endif
	static Util::Singleton<BasicAccessor> AccessorInstance;
	static void CreateAccessorInstance(uint32_t uPar = 0)
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
}

