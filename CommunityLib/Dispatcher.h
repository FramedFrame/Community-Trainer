#pragma once
#include "Disassembler.h"
#include <stdint.h>
#include <vector>
#include <stdarg.h>

#include "ToogleAble.h"

namespace Memory
{
	enum class Instruction
	{
		Call,Jump
	};

	class Dispatcher : public ToogleAble
	{
	public:
		Dispatcher(uint32_t uAddress,std::vector<uint8_t>& vEnableBytes) :
			m_vEnableBytes(vEnableBytes.begin(),vEnableBytes.end())
		{
			Init(uAddress,vEnableBytes);
		}
		Dispatcher(uint32_t uAddress,std::size_t uSize,...) :
			m_vEnableBytes(uSize)
		{
			va_list args;
			va_start(args, uSize);
			Init(uAddress,uSize,args);
			va_end(args);
		}
		Dispatcher(uint32_t uAddress,std::size_t uSize,uint8_t uData) :
			m_vEnableBytes(uSize)
		{
			Init(uAddress,uSize,uData);
		}
		Dispatcher(uint32_t uAddress,uint32_t uDestination,uint32_t* uReturn = NULL,Instruction instr = Instruction::Jump)
		{
			Init(uAddress,uDestination,uReturn,instr);
		}
		~Dispatcher(void)
		{
			Free();
		}

	protected:
		virtual bool enable();
		virtual bool disable();

	private:
		std::vector<uint8_t> m_vBackup;
		std::vector<uint8_t> m_vEnableBytes;
		Disassembler m_disassembler;
		uint32_t m_uAddress;

		virtual void Init(uint32_t uAddress,std::vector<uint8_t>& vEnableBytes);
		virtual void Init(uint32_t uAddress,std::size_t uSize,va_list args);
		virtual void Init(uint32_t uAddress,std::size_t uSize,uint8_t uData);
		virtual void Init(uint32_t uAddress,uint32_t uDestination,uint32_t* uReturn = NULL,Instruction instr = Instruction::Jump);
		virtual void Free();
		virtual uint32_t CalculateJumpCall(uint32_t uSource,uint32_t uDestination);
	};
}

