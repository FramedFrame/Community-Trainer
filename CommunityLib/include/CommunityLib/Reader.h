#pragma once
#include <stdint.h>
#include <string>
#include <typeinfo>
#include <vector>

namespace IO
{
	class Reader
	{
	public:
		Reader(std::vector<uint8_t> &vData) :
			m_vData(vData.begin(),vData.end())
		{
			this->m_uIndex = 0;
		}
		~Reader(void)
		{

		}

		virtual void ResetIndex();
		virtual uint32_t& operator()();

		template<typename T>
		bool TryRead(T& tValue,uint32_t uIndex=-1)
		{
			bool fMemberIndex = false;
			if(uIndex == -1)
			{
				uIndex = this->m_uIndex;
				fMemberIndex = true;
			}

			auto x = sizeof(T);
			if(this->CanRead(uIndex,x))
			{
				memcpy_s(&tValue,x,(this->m_vData.data()+uIndex),x);
				if(fMemberIndex)
					this->m_uIndex += x;
				return true;
			}
			return false;
		}
		template<typename T>
		T Read(uint32_t uIndex = -1)
		{
			T t;
			ZeroMemory(&t,sizeof(T));
			if(TryRead(t,uIndex))
				return t;
			else
				throw new std::exception("Out of Index");
			return t;
		}
		template<typename T>
		Reader& operator>>(T& tVal)
		{
			if(typeid(T) == typeid(std::string))
				this->ReadStringProxy(reinterpret_cast<void*>(&tVal));
			else
				tVal = this->Read<T>();
			return *this;
		}

		virtual bool TryReadString(std::string &str,uint32_t uIndex = -1);
		virtual std::string ReadString(uint32_t uIndex = -1);

	private:
		std::vector<uint8_t> m_vData;
		uint32_t m_uIndex;

		virtual bool CanRead(uint32_t nIndex,uint32_t uSize);

		inline void ReadStringProxy(void* p)
		{
			std::string* pStr = reinterpret_cast<std::string*>(p);
			*pStr = this->ReadString();
		}
	};
}

