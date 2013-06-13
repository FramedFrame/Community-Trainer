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
		PatternScanner(void);
		~PatternScanner(void);

		void SetSource(Source& source);
		void* GetSource();

		std::vector<void*> GetResult(Pattern& pattern);
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

		std::vector<std::vector<void*>> GetResults(std::vector<Pattern>& vPattern);
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

		static Pattern GeneratePattern(std::string &strPattern);
		static std::vector<void*> DoScan(Source& source,Pattern& pattern);

	private:
		static std::vector<int> GenerateGoodTable(Pattern& pattern);
		static std::vector<int> GenerateBadTable(Pattern& pattern);
		static uint32_t SearchForPattern(Source& source,Pattern& pattern);
		void MakeBuffer();

		std::vector<uint8_t> m_vBuffer;

		bool m_fUseBuffer;
		Source m_source;
	};

}
