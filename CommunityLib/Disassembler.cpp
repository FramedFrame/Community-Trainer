#include "Disassembler.h"
#include <udis86.h>

#define UD_CAST static_cast<ud*>(this->m_pUdis)


using namespace Memory;

std::shared_ptr<Disassembler> Disassembler::Instance;


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
	std::size_t u = 0;

	while (ud_disassemble(UD_CAST)) 
	{
		if(!strcmp(ud_insn_asm(UD_CAST),"invalid"))
			break;

		Opcode opcode;
		memset(&opcode,0,sizeof(Opcode));

		opcode.Text = std::string(ud_insn_asm(UD_CAST));
		opcode.Size = ud_insn_len(UD_CAST);
		opcode.Address = reinterpret_cast<uint32_t>(pAddress);

		opcode.Data.reset(new std::vector<uint8_t>());
		for(u = 0;u < opcode.Size;u++)
			opcode.Data->push_back(pAddress[u]);


		pAddress+= opcode.Size;

		vOpcodes.push_back(opcode);
	}
	return vOpcodes;
}