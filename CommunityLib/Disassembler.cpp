#include "Disassembler.h"
#include <udis86.h>

#define UD_CAST static_cast<ud*>(this->m_pUdis)

using namespace Memory;

Disassembler::Disassembler(void)
{
	this->m_pUdis = static_cast<void*>(new ud_t());
	ud_init(UD_CAST);
	ud_set_mode(UD_CAST,32);
	ud_set_syntax(UD_CAST,UD_SYN_INTEL);
}


Disassembler::~Disassembler(void)
{
	ud_t* pUd = UD_CAST;
	delete pUd;
}

std::vector<Opcode> Disassembler::DisassembleInstructions( uint8_t* pAddy,std::size_t uSize )
{
	std::vector<Opcode> vOpcodes;

	ud_set_input_buffer(UD_CAST,pAddy,uSize);

	uint8_t* pAddress = pAddy; 

	while (ud_disassemble(UD_CAST)) 
	{
		Opcode opcode;
		memset(&opcode,0,sizeof(Opcode));

		opcode.Text = std::string(ud_insn_asm(UD_CAST));
		opcode.Size = ud_insn_len(UD_CAST);

		pAddy+= opcode.Size;

		vOpcodes.push_back(opcode);
	}
	return vOpcodes;
}

static bool CreateDisassemblerInstance()
{
	Memory::DisassemblerInstance.Create(new Disassembler());
}