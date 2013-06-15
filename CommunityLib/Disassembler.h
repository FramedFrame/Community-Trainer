#pragma once
#include <stdint.h>
#include <string>
#include <vector>

#include "Singleton.h"

namespace Memory
{
	struct Opcode
	{
		std::size_t Size;
		std::string Text;
		uint32_t Address;
		std::vector<uint8_t> Data;
	};

	class Disassembler
	{
	public:
		Disassembler(void);
		~Disassembler(void);

		std::vector<Opcode> DisassembleInstructions(uint8_t* pAddy,std::size_t uSize);
	private:
		void* m_pUdis;
	};

	static Util::Singleton<Disassembler> DisassemblerInstance;
	static void CreateDisassemblerInstance()
	{
		Memory::DisassemblerInstance.Create(new Disassembler());
	}
}

