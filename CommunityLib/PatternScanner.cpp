#include "PatternScanner.h"


using namespace Memory;


PatternScanner::PatternScanner(void)
{
	this->m_fUseBuffer = false;
}


PatternScanner::~PatternScanner(void)
{
}

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

void AddCharRange(std::vector<char>& v,char min,char max)
{
	for(char x = min;x < max+1;x++)
		v.push_back(x);
}

bool IsIndexAble(std::vector<char> &v,char c)
{
	return std::find(v.begin(),v.end(),c) != v.end();
}

Pattern PatternScanner::GeneratePattern(std::string &strPattern)
{
	Pattern p;
	p.uTakeResult = 0;

	std::vector<char> vDigit;
	AddCharRange(vDigit,'a','f');
	AddCharRange(vDigit,'A','F');
	AddCharRange(vDigit,'0','9');


	std::vector<char> vRandomizer;
	vRandomizer.push_back('?');
	vRandomizer.push_back('*');

	int nSubSize = 0;

	for(size_t u = strPattern.size()-1; u > 0;u--)
	{
		if(IsIndexAble(vDigit,strPattern[u]))
			break;
		nSubSize++;
	}

	std::string strWorkString = !nSubSize ? strPattern : strPattern.substr(0,strPattern.size()  - nSubSize);

	for(size_t u = 0; u < strPattern.size();u++)
	{
		if(IsIndexAble(vRandomizer,strPattern[u]))
		{
			if(u+1 < strPattern.size() && IsIndexAble(vRandomizer,strPattern[u+1]))
				u++;

			p.vPattern.push_back(0);
			p.vRandomPattern.push_back(1);
		}
		else if(IsIndexAble(vDigit,strPattern[u]))
		{
			size_t uSubSize = 1;
			if(u+1 < strPattern.size() && IsIndexAble(vDigit,strPattern[u+1]))
			{
				uSubSize = 2;
				u++;
			}

			std::string str = strPattern.substr(uSubSize==2?u-1:u,uSubSize);

			unsigned long l = strtoul(str.c_str(),NULL,16);

			p.vPattern.push_back(l);
			p.vRandomPattern.push_back(0);
		}
	}
	return p;
}
std::vector<void*> PatternScanner::DoScan(Source& source,Pattern& pattern)
{
	std::vector<void*> vResult;
	bool fScan = true;
	Source src = source;
	int nResultAmount = 0;

	while(fScan)
	{
		uint32_t u = SearchForPattern(src,pattern);
		if(u != -1)
		{
			if(!pattern.uTakeResult)
			{
				vResult.push_back(reinterpret_cast<void*>(reinterpret_cast<uint8_t*>(src.pStart) + u));
			}
			else if(pattern.uTakeResult-1 == nResultAmount)
			{
				if(pattern.uTakeResult-1 == nResultAmount)
				{
					vResult.push_back(reinterpret_cast<void*>(reinterpret_cast<uint8_t*>(src.pStart) + u));
					break;
				}
				else
					nResultAmount++;
			}
		}
		else break;

		src.uLen -= u+1;
		src.pStart = reinterpret_cast<uint8_t*>(src.pStart) + u + 1;
	}

	return vResult;
}


std::vector<int> PatternScanner::GenerateGoodTable(Pattern& pattern)
{
	size_t uSize = pattern.vPattern.size();

	std::vector<int> vGoodSuffixTable(uSize+1);
	int i = 0;
	for (int j = 0; j <= uSize; j++) 
	{
		for (i = uSize - 1; i >= 1; i--) 
		{
			for (int k = 1; k <= j; k++) 
			{
				if (i - k < 0)
					break;
				if(pattern.vRandomPattern[uSize - k] == 1 || pattern.vRandomPattern[i - k] == 1)
					continue;
				if (pattern.vPattern[uSize - k] != pattern.vPattern[i - k]) 
					goto nexttry;
			}
			goto matched;
nexttry:
			;
		}
matched:
		vGoodSuffixTable[j] = uSize - i;
	}
	return vGoodSuffixTable;
}
std::vector<int> PatternScanner::GenerateBadTable(Pattern& pattern)
{
	std::vector<int> vBadSuffixTable(UCHAR_MAX+1);

	size_t uSize = pattern.vPattern.size();

	for(int i = 0; i < UCHAR_MAX+1;i++)
		vBadSuffixTable[i] = uSize;

	for(size_t u = 0;u < pattern.vPattern.size();u++)
		vBadSuffixTable[(int)pattern.vPattern[u]] = uSize - u - 1;

	return vBadSuffixTable;
}
uint32_t PatternScanner::SearchForPattern(Source& source,Pattern& pattern)
{
	std::vector<int> vGoodSuffix = PatternScanner::GenerateGoodTable(pattern),vBadSuffix = PatternScanner::GenerateBadTable(pattern);

	size_t pLen = pattern.vPattern.size(),sLen = source.uLen;
	uint8_t* pData = (uint8_t*)source.pStart;

	pLen--;
	int i = pLen, j = 0;
	while (i < sLen) 
	{
		j = 0;
		while (j <= pLen) 
		{
			if (pattern.vRandomPattern[pLen-j] == 1 || pData[i - j] == pattern.vPattern[pLen - j])
				j++;
			else 
			{
				i += vBadSuffix[(unsigned char)pData[i - j]] > vGoodSuffix[j] ? vBadSuffix[(unsigned char)pData[i - j]] - j : vGoodSuffix[j];
				goto newi;
			}
		}
		return i - pLen;
newi:
		;
	}

	return -1;
}

void PatternScanner::MakeBuffer()
{
	this->m_vBuffer.reserve(this->m_source.uLen);
	memcpy(this->m_vBuffer.data(),(const void*)this->m_source.pStart,this->m_source.uLen);
}