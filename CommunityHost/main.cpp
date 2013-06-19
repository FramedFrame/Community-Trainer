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

#define MAPLE_PATH "F:\Europe MapleStory for Vista\MapleStory.exe"

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
	ContextInstance->ServiceProvider->Run();
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrev,LPSTR lpCommand,int nCmdShow)
{
	StartUp();
	std::cout << "Creating Main Window" << std::endl;

	ContextInstance.reset(new Context());

	UI::MainWindow mainWindow;
	ContextInstance->ServiceProvider.reset(new LibSocket::ServiceProvider());
	ContextInstance->ClientManager.reset(new ClientManager(MAPLE_PATH,&mainWindow));
	ContextInstance->Host.reset(new Host(ContextInstance->ClientManager.get(),*ContextInstance->ServiceProvider.get()));

	mainWindow.Show();
	std::thread t(ExecuteAsio);
	
	nana::gui::exec();

	ContextInstance->ServiceProvider->Stop();

	return 0;
}