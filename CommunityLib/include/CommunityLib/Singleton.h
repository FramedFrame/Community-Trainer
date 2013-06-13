#pragma once
#include <memory>
namespace Util
{
	template<typename T>
	class Singleton
	{
	public:
		Singleton()
		{
			this->m_fIsPtr = -1;
		}
		void Create(T& tRef)
		{
			this->m_tPtr = &tRef;
			this->m_fIsPtr = 0;
		}
		void Create(T* tPtr)
		{
			this->m_tPtr = tPtr;
			this->m_fIsPtr = 1;
		}
		~Singleton()
		{
			if(this->m_fIsPtr == 1)
				delete this->m_tPtr;
		}

		T* operator()()
		{
			switch(this->m_fIsPtr)
			{
			case 0:
			case 1:
				return this->m_tPtr;
			default:
				return nullptr;
			}

		}
	private:
		T* m_tPtr;
		int32_t m_fIsPtr;
	};
}