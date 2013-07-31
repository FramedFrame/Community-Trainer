#pragma once
#include <stdarg.h>
#include <stdint.h>
#include <vector>

#include "Disassembler.h"
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
		Dispatcher()
		{

		}
		virtual ~Dispatcher(void);

		virtual void Init(uint32_t uAddress,std::vector<uint8_t>& vEnableBytes);
		virtual void Init(uint32_t uAddress,std::size_t uSize,...);
		virtual void Init(uint32_t uAddress,std::size_t uSize,uint8_t uData);
		virtual void Init(uint32_t uAddress,uint32_t uDestination,uint32_t* uReturn = NULL,Instruction instr = Instruction::Jump);

	protected:
		virtual bool enable();
		virtual bool disable();

	private:
		std::vector<uint8_t> m_vBackup;
		std::vector<uint8_t> m_vEnableBytes;
		Disassembler m_disassembler;
		uint32_t m_uAddress;

		virtual void Free();
		virtual uint32_t CalculateJumpCall(uint32_t uSource,uint32_t uDestination);
	};
}

