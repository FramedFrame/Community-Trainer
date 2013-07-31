#pragma once
#include <memory>

namespace Plugin
{
	template<typename T>
	class Carrier
	{
	public:
		Carrier()
		{

		}
		Carrier(T& t)
		{
			this->m_t = t;
		}

		T& operator()()
		{
			return this->m_t;
		}

		T& Resolve()
		{
			return this->m_t;
		}

		void Set(T& t)
		{
			this->m_t = t;
		}


	private:
		T m_t;
	};

	template<typename T> std::shared_ptr<Carrier<T>> GenerateCarrierPointer(T t)
	{
		return std::make_shared<Carrier<T>>(t);
	}

	template<typename T> Carrier<T> GenerateCarrier(T t)
	{
		return Carrier<T>(t);
	}
}