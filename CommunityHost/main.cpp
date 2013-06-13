
#include <functional>
#include <Windows.h>
#include <memory>
#include <vector>
#include <unordered_map>

#include "CommunityLib\Detour.h"
#include "CommunityLib\LibSocket.h"


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

std::vector<LibSocket::Session*> vSessions;

void OnDisconnect2()
{
	printf("Client Disconnected\n");
}

void OnMessage2(std::vector<uint8_t>& v)
{
	printf("Process Id: %i\n",*(int*)v.data());
	vSessions[vSessions.size()-1]->Stop();
	vSessions.pop_back();
}

void accept_socket(LibSocket::Session& session)
{
	printf("New Socket Accepted\n");
	vSessions.push_back(&session);
	std::function<void()> fff(OnDisconnect2);
	std::function<void(std::vector<uint8_t>&)> ff(OnMessage2);
	session.LinkFunctions(ff,fff);
	session.Start();
}




int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrev,LPSTR lpCommand,int nCmdShow)
{
	StartUp();

	LibSocket::ServiceProvider servProv;

	std::function<void(LibSocket::Session& session)> f(accept_socket);
	LibSocket::Server serv(f,servProv);
	serv.Start();

	servProv.Run();
	printf("Closing Press a Key\n");
	MessageBox(NULL,L"Close",L"",MB_OK);
	return 0;
}