#include "Template.h"

#include <fstream>

#include <rapidjson\rapidjson.h>
#include <rapidjson\document.h>
#include <rapidjson\writer.h>
#include <rapidjson\stringbuffer.h>

using namespace Profile;

std::string ReadFileUntilEnd(std::ifstream& fstrFile);

void IterateObject(items_t& items,rapidjson::Value& val);
void IterateArray(array_t& arr,rapidjson::Value& val);

bool IterateObject(items_t& items,rapidjson::Value &value,rapidjson::Document::AllocatorType& allocator);
bool IterateArray(array_t& arr,rapidjson::Value &value,rapidjson::Document::AllocatorType& allocator);

void IterateObject(items_t& items);
void IterateArray(array_t& arr);


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
	auto x = val.MemberBegin();

	while(x != val.MemberEnd())
	{
		auto type = x->value.GetType();
		if(type == rapidjson::kArrayType)
		{
			array_t* a = new array_t();
			items[x->name.GetString()] = &a;
			IterateArray(*a,x->value);
		}
		else if(type == rapidjson::kObjectType)
		{
			items_t* i = new items_t();
			items[x->name.GetString()] = &i;
			IterateObject(*i,x->value);
		}
		else if(type == rapidjson::kFalseType || type == rapidjson::kTrueType)
		{
			bool b = x->value.GetBool();
			items[x->name.GetString()] = b;
		}
		else if(type == rapidjson::kNumberType)
		{
			if(x->value.IsInt())
				items[x->name.GetString()] = x->value.GetInt();
			else if(x->value.IsInt64())
				items[x->name.GetString()] = x->value.GetInt64();
			else if(x->value.IsUint())
				items[x->name.GetString()] = x->value.GetUint();
			else if(x->value.IsUint64())
				items[x->name.GetString()] = x->value.GetUint64();
			else if(x->value.IsDouble())
				items[x->name.GetString()] = x->value.GetDouble();
		}
		else if(type == rapidjson::kStringType)
		{
			std::string* str = new std::string(x->value.GetString());
			items[x->name.GetString()] = str;
		}

		x++;
	}
}
void IterateArray(array_t& arr,rapidjson::Value& val)
{
	auto x = val.MemberBegin();

	while(x != val.MemberEnd())
	{
		auto type = x->value.GetType();
		if(type == rapidjson::kArrayType)
		{
			array_t* a = new array_t();
			arr.push_back(&a);
			IterateArray(*a,x->value);
		}
		else if(type == rapidjson::kObjectType)
		{
			items_t* i = new items_t();
			arr.push_back(&i);
			IterateObject(*i,x->value);
		}
		else if(type == rapidjson::kFalseType || type == rapidjson::kTrueType)
		{
			bool b = x->value.GetBool();
			arr.push_back(b);
		}
		else if(type == rapidjson::kNumberType)
		{
			if(x->value.IsInt())
				arr.push_back(x->value.GetInt());
			else if(x->value.IsInt64())
				arr.push_back(x->value.GetInt64());
			else if(x->value.IsUint())
				arr.push_back(x->value.GetUint());
			else if(x->value.IsUint64())
				arr.push_back(x->value.GetUint64());
			else if(x->value.IsDouble())
				arr.push_back(x->value.GetDouble());
		}
		else if(type == rapidjson::kStringType)
		{
			std::string* str = new std::string(x->value.GetString());
			arr.push_back(str);
		}

		x++;
	}
}

void IterateObject(items_t& items)
{
	for(auto& x:items)
	{
		if(x.second.same<std::string*>())
			delete *x.second.get<std::string*>();
		else if(x.second.same<array_t*>())
		{
			array_t* a =  *x.second.get<array_t*>();
			IterateArray(*a);
			delete a;
		}
		else if(x.second.same<items_t>())
		{
			items_t* i = *x.second.get<items_t*>();
			IterateObject(*i);
			delete i;
		}
	}
}
void IterateArray(array_t& arr)
{
	for(auto& x:arr)
	{
		if(x.same<std::string*>())
			delete *x.get<std::string*>();
		else if(x.same<array_t*>())
		{
			array_t* a =  *x.get<array_t*>();
			IterateArray(*a);
			delete a;
		}
		else if(x.same<items_t>())
		{
			items_t* i = *x.get<items_t*>();
			IterateObject(*i);
			delete i;
		}
	}
}

bool IterateObject(items_t& items,rapidjson::Value &value,rapidjson::Document::AllocatorType& allocator)
{
	using namespace rapidjson;
	for(auto& x:items)
	{
		if(x.second.same<std::string*>())
		{
			Value val(kStringType);
			auto y =  *x.second.get<std::string*>();
			val.SetString(y->c_str());
			value.AddMember(x.first.c_str(),val,allocator);
		}
		else if(x.second.same<array_t*>())
		{
			Value val(kArrayType);
			auto y =  *x.second.get<array_t*>();
			IterateArray(*y,val,allocator);
			value.AddMember(x.first.c_str(),val,allocator);
		}
		else if(x.second.same<items_t>())
		{
			Value val(kObjectType);
			items_t* i = *x.second.get<items_t*>();
			IterateObject(*i,val,allocator);
			value.AddMember(x.first.c_str(),val,allocator);
		}
		else if(x.second.same<bool>())
		{
			auto y =  *x.second.get<bool>();
			Value val(y ? kTrueType : kFalseType);
			val.SetBool(y);
			value.AddMember(x.first.c_str(),val,allocator);
		}
		else if(x.second.same<int>())
		{
			auto y =  *x.second.get<int>();
			Value val(kNumberType);
			val.SetInt(y);
			value.AddMember(x.first.c_str(),val,allocator);
		}
		else if(x.second.same<unsigned int>())
		{
			auto y =  *x.second.get<unsigned int>();
			Value val(kNumberType);
			val.SetUint(y);
			value.AddMember(x.first.c_str(),val,allocator);
		}
		else if(x.second.same<int64_t>())
		{
			auto y =  *x.second.get<int64_t>();
			Value val(kNumberType);
			val.SetInt64(y);
			value.AddMember(x.first.c_str(),val,allocator);
		}
		else if(x.second.same<uint64_t>())
		{
			auto y =  *x.second.get<uint64_t>();
			Value val(kNumberType);
			val.SetUint64(y);
			value.AddMember(x.first.c_str(),val,allocator);
		}
		else if(x.second.same<double>())
		{
			auto y =  *x.second.get<double>();
			Value val(kNumberType);
			val.SetDouble(y);
			value.AddMember(x.first.c_str(),val,allocator);
		}
		else
			return false;
	}
	return true;
}
bool IterateArray(array_t& arr,rapidjson::Value &value,rapidjson::Document::AllocatorType& allocator)
{
	using namespace rapidjson;
	for(auto& x:arr)
	{
		if(x.same<std::string*>())
		{
			Value val(kStringType);
			auto y =  *x.get<std::string*>();
			val.SetString(y->c_str());
			value.PushBack(val,allocator);
		}
		else if(x.same<array_t*>())
		{
			Value val(kArrayType);
			auto y =  *x.get<array_t*>();
			IterateArray(*y,val,allocator);
			value.PushBack(val,allocator);
		}
		else if(x.same<items_t>())
		{
			Value val(kObjectType);
			items_t* i = *x.get<items_t*>();
			IterateObject(*i,val,allocator);
			value.PushBack(val,allocator);
		}
		else if(x.same<bool>())
		{
			auto y =  *x.get<bool>();
			Value val(y ? kTrueType : kFalseType);
			val.SetBool(y);
			value.PushBack(val,allocator);
		}
		else if(x.same<int>())
		{
			auto y =  *x.get<int>();
			Value val(kNumberType);
			val.SetInt(y);
			value.PushBack(val,allocator);
		}
		else if(x.same<unsigned int>())
		{
			auto y =  *x.get<unsigned int>();
			Value val(kNumberType);
			val.SetUint(y);
			value.PushBack(val,allocator);
		}
		else if(x.same<int64_t>())
		{
			auto y =  *x.get<int64_t>();
			Value val(kNumberType);
			val.SetInt64(y);
			value.PushBack(val,allocator);
		}
		else if(x.same<uint64_t>())
		{
			auto y =  *x.get<uint64_t>();
			Value val(kNumberType);
			val.SetUint64(y);
			value.PushBack(val,allocator);
		}
		else if(x.same<double>())
		{
			auto y =  *x.get<double>();
			Value val(kNumberType);
			val.SetDouble(y);
			value.PushBack(val,allocator);
		}
		else
			return false;
	}
	return true;
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
	using namespace rapidjson;
	Document document;
	Document::AllocatorType& allocator = document.GetAllocator();
	Value value(kObjectType);

	if(!IterateObject(this->m_mapItems,value,allocator))
		return false;//Parse Error

	StringBuffer stringBuffer;
	Writer<StringBuffer> writer(stringBuffer);
	document.Accept(writer);
	std::string strJson = stringBuffer.GetString();

	std::ofstream ostrJson(strJson,std::ios::out | std::ios::binary);
	if(!ostrJson.is_open())
		return false;

	ostrJson << strJson;

	ostrJson.close();

	return true;
}
void Template::Reset()
{
	IterateObject(this->m_mapItems);
	this->m_mapItems.clear();
}


items_t& Template::Browse(std::string strXPath)
{
	std::string strBuffer = "";
	items_t& map = this->m_mapItems;

	strBuffer.reserve(64);

	for(char c:strXPath)
	{

	}
}