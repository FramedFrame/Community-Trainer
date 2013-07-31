#pragma once
#include <stdint.h>
#include <string>
#include <vector>

namespace Memory
{
	struct Pattern
	{
		std::vector<uint8_t> vRandomPattern,vPattern;
		uint32_t uTakeResult;
	};
	struct Source
	{
		void* pStart; uint32_t uLen;
	};

	class PatternScanner
	{
	public:
		PatternScanner(void)
		{
			this->m_fUseBuffer = false;
		}

		virtual void SetSource(Source& source);
		virtual void* GetSource();

		virtual std::vector<void*> GetResult(Pattern& pattern);
		template<typename T>
		std::vector<T> GetResult(Pattern& pattern)
		{
			std::vector<void*> vResults = GetResult(vPattern);
			std::vector<T> vResult(vResults.size());
			for(void* p:vResults)
				vResult.push_back(reinterpret_cast<T>(p));
			return vResult;

			return reinterpret_cast<T>(GetResult(pattern));
		}

		virtual std::vector<std::vector<void*>> GetResults(std::vector<Pattern>& vPattern);
		template<typename T>
		std::vector<std::vector<T>> GetResults(std::vector<Pattern>& vPattern)
		{
			std::vector<std::vector<void*>> vResults = GetResults(vPattern);
			std::vector<std::vector<T>> vResult(vResults.size());

			for(std::vector<T> &v:vResults)
				for(void* p:v)
					vResult.push_back(reinterpret_cast<T>(p));

			return vResult;
		}

		static Pattern GeneratePattern(std::string &strPattern)
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
		static std::vector<void*> DoScan(Source& source,Pattern& pattern)
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

	private:
		static void AddCharRange(std::vector<char>& v,char min,char max)
		{
			for(char x = min;x < max+1;x++)
				v.push_back(x);
		}

		static bool IsIndexAble(std::vector<char> &v,char c)
		{
			return std::find(v.begin(),v.end(),c) != v.end();
		}

		static std::vector<int> GenerateGoodTable(Pattern& pattern)
		{
			size_t uSize = pattern.vPattern.size();

			std::vector<int> vGoodSuffixTable(uSize+1);
			size_t i = 0;
			for (size_t j = 0; j <= uSize; j++) 
			{
				for (i = uSize - 1; i >= 1; i--) 
				{
					for (size_t k = 1; k <= j; k++) 
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
		static std::vector<int> GenerateBadTable(Pattern& pattern)
		{
			std::vector<int> vBadSuffixTable(UCHAR_MAX+1);

			size_t uSize = pattern.vPattern.size();

			for(int i = 0; i < UCHAR_MAX+1;i++)
				vBadSuffixTable[i] = uSize;

			for(size_t u = 0;u < pattern.vPattern.size();u++)
				vBadSuffixTable[(int)pattern.vPattern[u]] = uSize - u - 1;

			return vBadSuffixTable;
		}
		static uint32_t SearchForPattern(Source& source,Pattern& pattern)
		{
			std::vector<int> vGoodSuffix = PatternScanner::GenerateGoodTable(pattern),vBadSuffix = PatternScanner::GenerateBadTable(pattern);

			size_t pLen = pattern.vPattern.size(),sLen = source.uLen;
			uint8_t* pData = (uint8_t*)source.pStart;

			pLen--;
			size_t i = pLen, j = 0;
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
		virtual void MakeBuffer();

		std::vector<uint8_t> m_vBuffer;

		bool m_fUseBuffer;
		Source m_source;
	};

}
