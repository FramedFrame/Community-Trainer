#pragma once
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <Windows.h>

#include "Any.h"

namespace Plugin
{

	typedef std::unordered_map<std::string,std::pair<HMODULE,std::shared_ptr<int>>> variablenpool_t;


	class VariablePool
	{
	public:
		VariablePool(void)
		{
		}
		~VariablePool()
		{

		}

		virtual HMODULE GetModule()
		{
			return GetModuleHandle(NULL);
		}

		template<typename T>
		void DeleteObject(void* pObject)
		{
			T* tp = reinterpret_cast<T*>(pObject);
			delete tp;
		}

		template<typename T>
		bool Add(std::string str,std::shared_ptr<T> tValue)
		{
			std::unique_lock<std::mutex>(this->m_mtxLock);
			if(this->m_mapVariablePool.find(str) == this->m_mapVariablePool.end())
			{
				auto x = reinterpret_cast<shared_ptr<int>*>(&tValue);
				this->m_mapVariablePool[str] = std::make_pair(this->GetModule(),*x);
				return true;
			}
			return false;
		}
		bool Remove(std::string str)
		{
			std::unique_lock<std::mutex>(this->m_mtxLock);
			auto y = this->m_mapVariablePool.find(str);
			if(y != this->m_mapVariablePool.end())
			{
				this->m_mapVariablePool.erase(y);
				return true;
			}
			return false;
		}
		virtual void Clear();
		virtual void ClearModule(HMODULE hModule);

		template<typename T>
		T* Get(std::string str)
		{
			std::unique_lock<std::mutex>(this->m_mtxLock);
			auto y = this->m_mapVariablePool.find(str);
			if(y != this->m_mapVariablePool.end())
			{
				auto x = reinterpret_cast<std::shared_ptr<T>*>(&(y->second.second));
				return (*x).get();
			}
			return NULL;
		}

		template<typename T>
		bool operator()(std::string str,T& t)
		{
			std::unique_lock<std::mutex>(this->m_mtxLock);
			auto y = this->m_mapVariablePool.find(str);
			if(y != this->m_mapVariablePool.end())
			{
				auto x = reinterpret_cast<std::shared_ptr<T>*>(&(y->second.second));
				t = *((*x).get());
				return true;
			}
			return false;
		}
	private:
		variablenpool_t m_mapVariablePool;
		std::mutex m_mtxLock;

	};

}
#define POOL_FUNCTION(x) Plugin::any x(parameter_t& vParameter)