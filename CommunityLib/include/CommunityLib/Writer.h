#pragma once
#include <stdint.h>
#include <string>
#include <typeinfo>
#include <vector>

namespace IO
{

	class Writer
	{
	public:
		template<typename T>
		void Write(T tValue)
		{
			auto x = sizeof(T);
			uint8_t* pData = reinterpret_cast<uint8_t*>(&tValue);
			for(uint32_t u = 0; u < x;u++)
				this->m_vData.push_back(pData[u]);
		}
		virtual void WriteString(std::string str);


		template<typename T>
		Writer& operator<<(T tValue)
		{
			if(typeid(T) == typeid(std::string))
				this->WriteStringProxy(reinterpret_cast<void*>(&tValue));
			else
				this->Write(tValue);

			return *this;
		}

		virtual std::vector<uint8_t>& operator()();
	private:
		std::vector<uint8_t> m_vData;

		inline void WriteStringProxy(void* p)
		{
			std::string* pStr = reinterpret_cast<std::string*>(p);
			this->WriteString(*pStr);
		}
	};
}

