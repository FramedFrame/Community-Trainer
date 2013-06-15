#include <CommunityLib/LibSocket.h>
#include <functional>
#include <Windows.h>
#include <memory>

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

std::shared_ptr<LibSocket::ServiceProvider> serv;

void OnDisconnect()
{
	printf("Disconnected\n");
	serv.get()->Stop();
}

void OnMessage(std::vector<uint8_t>& v)
{
	printf("New Message\n");
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrev,LPSTR lpCommand,int nCmdShow)
{
	StartUp();

	serv.reset(new LibSocket::ServiceProvider());
	std::function<void()> fff(OnDisconnect);
	std::function<void(std::vector<uint8_t>&)> ff(OnMessage);

	LibSocket::Session session(*serv.get());
	session.LinkFunctions(ff,fff);
	session.StartConnect();

	serv->Run();
	printf("Exit\n");
	MessageBoxA(NULL,"Done2","ABC",MB_OK);
	serv.reset();
	return 0;
}