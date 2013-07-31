#pragma once
#include <memory>
#include <map>
#include <string>

#include "Any.h"


namespace Profile
{
	typedef std::map<std::string,Util::nana::any> items_t;
	class Template
	{
	public:
		Template(void)
		{

		}
		~Template(void)
		{

		}

		virtual bool Load(std::string& strFilename);
		virtual bool Save(std::string& strFilename);

	private:
		items_t m_mapItems;
	};
}

