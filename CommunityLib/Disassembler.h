#pragma once
#include <memory>
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
		std::shared_ptr<std::vector<uint8_t>> Data;
	};

	class Disassembler
	{
	public:
		Disassembler(void)
		{
			this->Init();
		}
		~Disassembler(void)
		{
			this->Free();
		}

		static std::shared_ptr<Disassembler> Instance;

		virtual std::vector<Opcode> DisassembleInstructions(uint8_t* pAddy,std::size_t uSize);
	private:
		void* m_pUdis;

		virtual void Init();
		virtual void Free();
	};

	static void CreateDisassemblerInstance()
	{
		Memory::Disassembler::Instance.reset(new Disassembler());
	}
}

