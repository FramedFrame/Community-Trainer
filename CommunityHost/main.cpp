#include <iostream>
#include <Windows.h>

#include <nana\gui\wvl.hpp>

#include "MainWindow.h"

int StartUp()
{
	AllocConsole();
	AttachConsole(GetCurrentProcessId());

	FILE* fp = NULL;
	freopen_s(&fp, "CON", "w", stdout);
	freopen_s(&fp, "CON", "w", stderr);

	char cc[128];
	sprintf_s(cc,"Server: %i",GetCurrentProcessId());
	SetConsoleTitleA(cc);

	return 1;
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrev,LPSTR lpCommand,int nCmdShow)
{
	StartUp();
	std::cout << "Creating Main Window" << std::endl;

	UI::MainWindow mainWindow;
	mainWindow.Show();

	nana::gui::exec();

	return 0;
}