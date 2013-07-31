#include "Template.h"

#include <fstream>

#include <rapidjson\rapidjson.h>
#include <rapidjson\document.h>

using namespace Profile;


std::string ReadFileUntilEnd(std::ifstream& fstrFile)
{
	std::string strReturn;

    fstrFile.seekg(0, std::ios::end);//Move to last postion
	strReturn.resize(fstrFile.tellg());//Adjust result string
	fstrFile.seekg(0, std::ios::beg);//Move to first position

	fstrFile.read(&strReturn[0], strReturn.size());

	return strReturn;
}

void IterateObject(items_t& items,rapidjson::Value& val)
{
	auto x = val.Begin();

	while(x != val.End())
	{
		auto type = x->GetType();
		switch(x->GetType())
		{
		case rapidjson::kObjectType:
			IterateObject(*x->,val);
		}
		x++;
	}
}

bool Template::Load(std::string& strFilename)
{
	using namespace rapidjson;


	std::ifstream fstrJson(strFilename,std::ios::in | std::ios::binary);
	if(fstrJson.is_open())
	{
		auto strJson = ReadFileUntilEnd(fstrJson);
		fstrJson.close();

		Document document;
		document.Parse<0>(strJson.c_str());
		if(!document.HasParseError())
		{
			this->m_mapItems.clear();
			IterateObject(this->m_mapItems,document);
		}
	}
	return false;
}
bool Template::Save(std::string& strFilename)
{

}
