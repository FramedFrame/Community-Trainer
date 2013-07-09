#include "MemoryEditor.h"

#include "Accessor.h"

std::vector<uint8_t> Plugin::MemoryEditor::Read( uint32_t uPos,uint32_t n )
{
	if(this->m_isHost)
		return Memory::ProcessAccessor::Instance->Read(uPos,n);
	else
		return Memory::InternalAccessor::Instance->Read(uPos,n);
}

bool Plugin::MemoryEditor::Write( uint32_t uPos,std::vector<uint8_t> &v )
{
	if(this->m_isHost)
		return Memory::ProcessAccessor::Instance->Write(uPos,v);
	else
		return Memory::InternalAccessor::Instance->Write(uPos,v);
}

std::vector<Plugin::Opcode> Plugin::MemoryEditor::Disassemble( uint32_t uPos,uint32_t n )
{
	std::vector<Memory::Opcode> op;
	if(this->m_isHost)
		op = Memory::ProcessAccessor::Instance->Disassemble(uPos,n);
	else
		op = Memory::InternalAccessor::Instance->Disassemble(uPos,n);

	return *reinterpret_cast<std::vector<Plugin::Opcode>*>(&op);
}
