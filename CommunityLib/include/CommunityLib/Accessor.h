#pragma once
#include <memory>
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
		Accessor(void)
		{

		}
		~Accessor(void)
		{

		}

		virtual std::vector<uint8_t> Read(uint32_t uAddress,std::size_t uSize);
		virtual bool Write(uint32_t uAddress,std::vector<uint8_t>& vData);
		virtual bool Write(uint32_t uAddress,std::size_t uSize,...);
		virtual bool Write(uint32_t uAddress,std::size_t uSize,uint8_t uData);

		virtual std::vector<Opcode> Disassemble(uint32_t uAddress,std::size_t uSize);

	protected:
		virtual bool write(uint32_t,std::vector<uint8_t>&,bool = true) = 0;
		virtual bool read(uint32_t,std::vector<uint8_t>&,bool = true) = 0;

		virtual bool IsReadable(uint32_t uFlag);
		virtual bool IsWriteable(uint32_t uFlag);
	};

	class InternalAccessor : public Accessor
	{
	public:
		static std::shared_ptr<InternalAccessor> Instance;
	protected:
		virtual bool write(uint32_t,std::vector<uint8_t>&,bool);
		virtual bool read(uint32_t,std::vector<uint8_t>&,bool);
	};

	class ProcessAccessor : public Accessor
	{
	public:
		ProcessAccessor(HANDLE hProcess)
		{
			this->m_hProcess = hProcess;
		}

		static std::shared_ptr<ProcessAccessor> Instance;

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
	static void CreateAccessorInstance(uint32_t uPar = 0)
	{
#ifdef CLIENT
			HANDLE h = OpenProcess(PROCESS_ALL_ACCESS,FALSE,uPar);
			if(h == NULL || h == INVALID_HANDLE_VALUE)
				return;//Fail Log

			auto x = new BasicAccessor(h);
#else
			auto x =  new BasicAccessor();
#endif
			BasicAccessor::Instance.reset(x);
	}
}

