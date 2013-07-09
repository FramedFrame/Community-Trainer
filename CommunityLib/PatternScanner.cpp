#include "PatternScanner.h"


using namespace Memory;

void PatternScanner::SetSource(Source& source)
{
	this->m_source = source;
}
void* PatternScanner::GetSource()
{
	return &this->m_source;
}

std::vector<void*> PatternScanner::GetResult(Pattern& pattern)
{
	bool fCreatedBuffer = false;

	if(fCreatedBuffer = (m_fUseBuffer && this->m_vBuffer.size() == 0))
		MakeBuffer();

	Source source = this->m_source;

	if(fCreatedBuffer)
		source.pStart = this->m_vBuffer.data();

	auto x = PatternScanner::DoScan(source,pattern);

	if(fCreatedBuffer)
		this->m_vBuffer.clear();

	return x;
}

std::vector<std::vector<void*>> PatternScanner::GetResults(std::vector<Pattern>& vPattern)
{
	bool fCreatedBuffer = false;

	if(fCreatedBuffer = (m_fUseBuffer && this->m_vBuffer.size() == 0))
		MakeBuffer();

	Source source = this->m_source;

	if(fCreatedBuffer)
		source.pStart = this->m_vBuffer.data();

	std::vector<std::vector<void*>> vResult;

	for(Pattern& p:vPattern)
		vResult.push_back(GetResult(p));

	if(fCreatedBuffer)
		this->m_vBuffer.clear();

	return vResult;
}


void PatternScanner::MakeBuffer()
{
	this->m_vBuffer.reserve(this->m_source.uLen);
	memcpy(this->m_vBuffer.data(),(const void*)this->m_source.pStart,this->m_source.uLen);
}