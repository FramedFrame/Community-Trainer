#pragma once
#include <memory>
#include <map>
#include <string>
#include <vector>

#include "Any.h"


namespace Profile
{
	typedef std::map<std::string,Util::nana::any> items_t;
	typedef std::vector<Util::nana::any> array_t;
	class Template
	{
	public:
		Template(void)
		{

		}
		~Template(void)
		{
			this->Reset();
		}

		virtual bool Load(std::string& strFilename);
		virtual bool Save(std::string& strFilename);
		virtual void Reset();

		virtual items_t& Browse(std::string strXPath);

	private:
		items_t m_mapItems;
	};
}

