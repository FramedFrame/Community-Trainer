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

bool Error;

std::string strMask;

struct Pattern
{
	std::string Aob,Name;
	int Result,Offset,Size;
	bool ReverseCall,TakeData;
	unsigned int FinalResult;
};

std::vector<Pattern> vPattern;


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
	Error = true;
	return NULL;;
}
void DoDump()
{
	auto xx = FindProcessId((wchar_t*)PROCESS_NAME);

	if(xx == -1)
	{
		Error = true;
		return;
	}

	HANDLE h = OpenProcess(PROCESS_ALL_ACCESS,FALSE,xx);

	if(h == INVALID_HANDLE_VALUE)
	{
		Error = true;
		return;
	}

	MODULEINFO modIf;
	ZeroMemory(&modIf,sizeof(MODULEINFO));
	HMODULE hModule = GetRemoteModuleHandle(xx,L"MapleStory.exe");
	if(hModule == NULL)
	{
		Error = true;
		return;
	}
	GetModuleInformation(h,hModule,&modIf,sizeof(MODULEINFO));

	vMemDump.resize(modIf.SizeOfImage);
	DWORD dwRead = 0;
	ReadProcessMemory(h,modIf.lpBaseOfDll,vMemDump.data(),vMemDump.size(),&dwRead);
	std::cout << GetLastError() << endl;

	CloseHandle(h);
	return;
}

int CalculateAddy(int nResult)
{
	int pAddy = (int)vMemDump.data();
	return (nResult - pAddy)+0x400000;
}

#define jmp(frm, to) (int)(((int)to - (int)frm) - 5);
//1337=x-99 - 5/+5
//1337+5/+99
//1337 + 5 + 99=x
int ReverseCall(int n,int addy)
{
	return n + 5 + addy;
}
void DoScan()
{
	Memory::PatternScanner pat;
	Memory::Source src;
	src.pStart = vMemDump.data();
	src.uLen = vMemDump.size();
	pat.SetSource(src);

	for(auto &pp:vPattern)
	{
		cout << "Scanning Pattern: " << pp.Name << endl;
		Memory::Pattern p = Memory::PatternScanner::GeneratePattern(pp.Aob);
		p.uTakeResult = pp.Result;
		auto yy = pat.GetResult(p);

		if(!yy.size())
		{
			cout << "No Result for: " << pp.Name << endl;
			pp.FinalResult = -1;
			continue;
		}
		int nAddy = CalculateAddy((int)yy[0]);

		if(pp.Offset != 0)
			nAddy += pp.Offset;

		if(pp.TakeData)
		{
			if(pp.Size > 4)
				cout << "Size isn't allowed to be greater then 4!" << endl;

			if(pp.Size == 4)
			{
				int n = 0;
				char* pAddy = (char*)&n;
				char* pSrc = (char*)yy[0];
				pSrc += pp.Offset;
				memcpy(pAddy,pSrc,pp.Size);
				nAddy = n;
			}
			else if(pp.Size == 2)
			{
				short n = 0;
				char* pAddy = (char*)&n;
				char* pSrc = (char*)yy[0];
				pSrc += pp.Offset;
				memcpy(pAddy,pSrc,pp.Size);
				nAddy = n;
			}
			else if(pp.Size == 1)
			{
				char n = 0;
				char* pAddy = (char*)&n;
				char* pSrc = (char*)yy[0];
				pSrc += pp.Offset;
				memcpy(pAddy,pSrc,pp.Size);
				nAddy = n;
			}
			else
				cout << "Unknown Size: " << pp.Size << endl;
		}
		if(pp.ReverseCall)
		{
			int n = 0;
			char* pSrc = (char*)yy[0];
			pSrc += pp.Offset;
			memcpy(&n,pSrc+1,4);
			nAddy = ReverseCall(n,nAddy);
		}

		pp.FinalResult = nAddy;
	}
	return;
}
void DoPrint()
{
	char cc[1024];
	for(auto pp:vPattern)
	{
		sprintf_s(cc,strMask.c_str(),pp.Name.c_str(),pp.FinalResult);
		std::cout << cc << endl;

	}
}

int main()
{
	Error = false;
	cout << "Community Updater -> Reads from file Aob.txt and Mask.txt" << endl;
	ReadMask();
	ReadPatterns();
	DoDump();
	if(Error)
	{
		return EXIT_SUCCESS;
	}
	DoScan();

	DoPrint();

	getchar();
	return EXIT_SUCCESS;
}