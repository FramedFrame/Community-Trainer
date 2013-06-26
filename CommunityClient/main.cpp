#include <iostream>
#include <thread>
#include <Windows.h>
#include "Context.h"

#include "Client.h"

extern "C" __declspec( dllexport )void NullExport(void)
{
	std::cout << "Called Null Export" << std::endl;
}

int StartUp()
{
	AllocConsole();
	AttachConsole(GetCurrentProcessId());

	FILE* fp = NULL;
	freopen_s(&fp, "CON", "w", stdout);
	freopen_s(&fp, "CON", "w", stderr);

	char cc[128];
	sprintf_s(cc,"Client: %i",GetCurrentProcessId());
	SetConsoleTitleA(cc);

	return 1;
}

std::shared_ptr<std::thread> mainThread;
bool fExit;

std::shared_ptr<Context> ContextInstance;

void Run()
{ 
	StartUp();
	ContextInstance.reset(new Context());
	ZeroMemory(ContextInstance.get(),sizeof(Context));

	ContextInstance->ServiceProvider.reset(new LibSocket::ServiceProvider());
	ContextInstance->Client.reset(new Client());
	
	ContextInstance->Client->Start();
	ContextInstance->ServiceProvider->Run();
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrev,LPSTR lpCommand,int nCmdShow)
{
	fExit = false;
	StartUp();
	mainThread.reset(new std::thread(Run));
	
	while(!fExit)
		Sleep(500);

	return 0;
}

DWORD WINAPI DllMain(HMODULE hModule,DWORD dwReason,LPVOID lpvReversed)
{
	switch(dwReason)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)&Run,NULL,0,NULL);
		break;
	}
	return TRUE;
}