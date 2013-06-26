#include <iostream>
#include <Windows.h>
#include <thread>
#include <typeinfo>

#include <nana\gui\wvl.hpp>
#include <nana\gui\widgets\listbox.hpp>

#include "MainWindow.h"
#include "Context.h"

#include <CommunityLib/Reader.h>
#include <CommunityLib/Writer.h>

#define MAPLE_PATH "F:\\Europe MapleStory for Vista\\MapleStory.exe"

int StartUp()
{
	AllocConsole();
	AttachConsole(GetCurrentProcessId());

	FILE* fp = NULL;
	freopen_s(&fp, "CON", "r", stdin);
	freopen_s(&fp, "CON", "w", stdout);
	freopen_s(&fp, "CON", "w", stderr);

	char cc[128];
	sprintf_s(cc,"Server: %i",GetCurrentProcessId());
	SetConsoleTitleA(cc);

	return 1;
}

void ExecuteAsio()
{
	ContextInstance->Host->Start();
	ContextInstance->ServiceProvider->Run();
}
std::shared_ptr<Context> ContextInstance;


int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrev,LPSTR lpCommand,int nCmdShow)
{
	StartUp();
	std::cout << "Creating Main Window" << std::endl;

	ContextInstance.reset(new Context());

	ContextInstance->MainWindow.reset(new UI::MainWindow());

	ContextInstance->ServiceProvider.reset(new LibSocket::ServiceProvider());
	ContextInstance->ClientManager.reset(new ClientManager(MAPLE_PATH));
	ContextInstance->Host.reset(new Host());

	ContextInstance->MainWindow->Show();
	std::thread t(ExecuteAsio);
	
	nana::gui::exec();

	ContextInstance->ServiceProvider->Stop();

	return 0;
}