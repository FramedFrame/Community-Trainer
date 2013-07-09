#pragma once
#include <memory>
#include <stdint.h>
#include <vector>

namespace Plugin
{
	struct Opcode
	{
		std::size_t Size;
		std::string Text;
		uint32_t Address;
		std::shared_ptr<std::vector<uint8_t>> Data;
	};
	class MemoryEditor
	{
	public:
		MemoryEditor(void* p,bool isHost)
		{
			this->m_p = p;
			this->m_isHost = isHost;
		}
		~MemoryEditor(void)
		{
		}

		virtual std::vector<uint8_t> Read(uint32_t uPos,uint32_t n);
		virtual bool Write(uint32_t uPos,std::vector<uint8_t> &v);
		virtual std::vector<Opcode> Disassemble(uint32_t uPos,uint32_t n);
	private:
		void* m_p;
		bool m_isHost; 
	};
}
