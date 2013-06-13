#pragma once
#include "Disassembler.h"
#include <stdint.h>
#include <vector>
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
		Dispatcher(uint32_t uAddress,std::vector<uint8_t>& vEnableBytes);
		Dispatcher(uint32_t uAddress,std::size_t uSize,...);
		Dispatcher(uint32_t uAddress,std::size_t uSize,uint8_t uData);
		Dispatcher(uint32_t uAddress,uint32_t uDestination,uint32_t* uReturn = NULL,Instruction instr = Instruction::Jump);
		~Dispatcher(void);

	protected:
		bool enable();
		bool disable();

	private:
		std::vector<uint8_t> m_vBackup;
		std::vector<uint8_t> m_vEnableBytes;
		Disassembler m_disassembler;
		uint32_t m_uAddress;


		uint32_t CalculateJumpCall(uint32_t uSource,uint32_t uDestination);
	};
}

