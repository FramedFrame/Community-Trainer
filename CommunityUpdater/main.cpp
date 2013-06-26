#include <iostream>
#include <fstream>

#define CLIENT 1

#include <Windows.h>
#include <TlHelp32.h>
#include <CommunityLib/Accessor.h>
#include <CommunityLib/PatternScanner.h>

#include <Psapi.h>
#pragma comment(lib,"Psapi.lib ")

using namespace std;

const wchar_t* PROCESS_NAME = L"MapleStory.exe";

const char* MASK_FILE = "C:\\Users\\Home\\Documents\\Community-Trainer\\Debug\\Mask.txt",
	*AOB_FILE = "C:\\Users\\Home\\Documents\\Community-Trainer\\Debug\\Aob.txt";

std::vector<uint8_t> vMemDump;

std::string strMask;

struct Pattern
{
	std::string Aob,Name;
	int Result,Offset,Size;
	bool ReverseCall,TakeData;
	unsigned int FinalResult;
};

std::vector<Pattern> vPattern;


std::string Convert(std::wstring& wstr)
{
	std::string str;
	str.resize(wstr.size());


	//std::wcstombs((char*)str.data(),wstr.data(),wstr.size());

	return str;
}

std::vector<std::string> SplitString(std::string& str,char split)
{
	std::vector<std::string> v;
	std::string strBuf = "";
	strBuf.reserve(64);
	for(char c:str)
	{
		if(c == split)
		{
			if(strBuf == "")
				continue;

			v.push_back(strBuf);
			strBuf = "";
		}
		else
		strBuf += c;
	}
	if(strBuf != "")
		v.push_back(strBuf);
	return v;
}

std::string RemoveSpaces(std::string& str)
{
	std::string strBuf = "";
	strBuf.reserve(64);
	for(char c:str)
	{
		if(c != ' ')
			strBuf += c;
	}
	return strBuf;
}

DWORD FindProcessId(wchar_t* szProcName)
{
	HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,NULL);

	if(h == INVALID_HANDLE_VALUE)
	{
		cout << "Snapshot failed" << endl;
		return -1;
	}

	PROCESSENTRY32 pe32;
	ZeroMemory(&pe32,sizeof(PROCESSENTRY32));
	pe32.dwSize = sizeof(PROCESSENTRY32);

	if(!Process32First(h,&pe32))
	{
		cout << "Snapshot is empty" << endl;
		CloseHandle(h);
		return -1;
	}

	do
	{
		if(!lstrcmpW(szProcName,pe32.szExeFile))
		{
			wcout << "Found: " << pe32.szExeFile << endl;
			CloseHandle(h);
			return pe32.th32ProcessID;
		}
	}
	while(Process32Next(h,&pe32));


	CloseHandle(h);
	return -1;
}

void Test(DWORD dwProcId)
{
	Memory::CreateAccessorInstance(dwProcId);
	Memory::CreateDisassemblerInstance();

	auto y = Memory::BasicAccessor::Instance->Disassemble(0x400000,0x40);

	for(Memory::Opcode& op:y)
	{
		cout << hex << (0x400000 + (op.Address-y[0].Address)) << ": " << op.Text << endl;
	}
	return;
}


void ReadMask()
{
	std::ifstream maskStream(MASK_FILE);
		if(!maskStream.is_open())
		cout << "Failed to open: " << MASK_FILE << endl;

	std::getline(maskStream,strMask);
	maskStream.close();
}
void ParseCommand(std::string& str,Pattern& p)
{
	auto x = SplitString(str,':');
	x[0] = RemoveSpaces(x[0]);

	if(x.size() < 2)
		return;

	if(!x[0].compare("aob"))
		p.Aob = x[1];
	else if(!x[0].compare("name"))
		p.Name = x[1];
	else if(!x[0].compare("result"))
		p.Result = std::stoi(x[1]);
	else if(!x[0].compare("offset"))
		p.Offset = std::stoi(x[1]);
	else if(!x[0].compare("size"))
		p.Size = std::stoi(x[1]);
	else if(!x[0].compare("reverse"))
		p.ReverseCall = std::stoi(x[1]);
	else if(!x[0].compare("data"))
		p.TakeData = std::stoi(x[1]);
}
void ParsePattern(std::string&str)
{
	auto x = SplitString(str,',');
	Pattern p;
	ZeroMemory(&p,sizeof(p));
	p.Aob = "";
	p.Name = "";


	for(std::string& s:x)
	{
		ParseCommand(s,p);
	}

	vPattern.push_back(p);

}
void ReadPatterns()
{
	std::ifstream maskStream(AOB_FILE);

	if(!maskStream.is_open())
		cout << "Failed to open: " << AOB_FILE << endl;

	std::string str = "";
	while(std::getline(maskStream,str))
	{
		ParsePattern(str);
	}
	maskStream.close();
}
HMODULE GetRemoteModuleHandle(DWORD lpProcessId, LPCWSTR lpModule)
{
    HANDLE hSnapshot;
    MODULEENTRY32 me32;
 
    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, lpProcessId);
    if (hSnapshot != INVALID_HANDLE_VALUE)
    {
        me32.dwSize = sizeof(MODULEENTRY32);
        if (Module32First(hSnapshot, &me32))
        {
			CloseHandle(hSnapshot);
			return me32.hModule;
		}
	}
	return NULL;;
}
void DoDump()
{
	auto xx = FindProcessId((wchar_t*)PROCESS_NAME);

	HANDLE h = OpenProcess(PROCESS_ALL_ACCESS,FALSE,xx);

	MODULEINFO modIf;
	ZeroMemory(&modIf,sizeof(MODULEINFO));
	HMODULE hModule = GetRemoteModuleHandle(xx,L"MapleStory.exe");
	GetModuleInformation(h,hModule,&modIf,sizeof(MODULEINFO));

	vMemDump.resize(modIf.SizeOfImage);
	DWORD dwRead = 0;
	ReadProcessMemory(h,modIf.lpBaseOfDll,vMemDump.data(),vMemDump.size(),&dwRead);
	std::cout << GetLastError() << endl;

	CloseHandle(h);
	return;
}

void DoScan()
{
	Memory::PatternScanner pat;
	Memory::Source src;
	src.pStart = vMemDump.data();
	src.uLen = vMemDump.size();
	pat.SetSource(src);

	for(auto pp:vPattern)
	{
		Memory::Pattern p = Memory::PatternScanner::GeneratePattern(pp.Aob);
		p.uTakeResult = pp.Result;
		auto yy = pat.GetResult(p);
		int pAddy1 = (int)yy[0];
		int pAddy2 = (int)vMemDump.data();
		int pAddy3 = (pAddy1 - pAddy2)+0x400000;
		cout << "Found Result: " << std::hex << (pAddy3);
	}
	return;
}

int main()
{
	cout << "Community Updater -> Reads from file Aob.txt and Mask.txt" << endl;
	ReadMask();
	ReadPatterns();
	DoDump();
	DoScan();

	getchar();
	return EXIT_SUCCESS;
}