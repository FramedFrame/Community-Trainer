#include "Client.h"
#include "Context.h"
#include <CommunityLib/Opcode.h>

#include <iostream>
#include <Windows.h>

MAKE_API_PTR(CreateWindowExA,pfnCreateWindowExA);
MAKE_API_PTR(ShowWindow,pfnShowWindow);

Client::Client()
{
	this->m_session.reset(new LibSocket::Session(*ContextInstance->ServiceProvider.get()));
	this->m_session->LinkFunctions(std::bind(&Client::OnMessage,this,std::placeholders::_1),
		std::bind(&Client::OnDisconnect,this));

	this->m_detourWindow.reset(new Memory::Detour(MAKE_CTX(pfnCreateWindowExA,[](DWORD dwExStyle,LPCSTR lpClassName,LPCSTR lpWindowName,DWORD dwStyle,
		int x,int y,int nWidth,int nHeight,HWND hWndParent,HMENU hMenu,HINSTANCE hInstance,LPVOID lpParam)->HWND{
		HWND hWnd = pfnCreateWindowExA(dwExStyle,lpClassName,lpWindowName,dwStyle,x,y,nWidth,nHeight,hWndParent,hMenu,hInstance,lpParam);	
		std::string strWindowClass = std::string(lpClassName);
		std::cout << "Some Window Created: " << strWindowClass << std::endl;
		if(!strWindowClass.compare("MapleStoryClass"))
		{
			ContextInstance->Window = hWnd;
			IO::Writer writer;
		    writer << (int8_t)ClientOpcode::WINDOW_HANDLE << hWnd;
			ContextInstance->Client->Send(writer());
			std::cout << "Maple Window: " << hWnd << std::endl;
		}
		return hWnd;	
	})));

	this->m_detourShowWindow.reset(new Memory::Detour(MAKE_CTX(pfnShowWindow,[](HWND hWnd,int nCmdShow)->BOOL{
		BOOL b = pfnShowWindow(hWnd,nCmdShow);
		if(hWnd == ContextInstance->Window && nCmdShow == SW_SHOW &&  !ContextInstance->Shown)
		{
			IO::Writer writer;
			writer << (int8_t)ClientOpcode::WINDOW_SHOWN;
			ContextInstance->Client->Send(writer());
			std::cout << "Shown Maple Window: " << hWnd << std::endl;
			ContextInstance->Shown = true;
		}
		return b;	
	})));
}


Client::~Client(void)
{
}

void Client::Start()
{
	this->m_detourWindow->Enable();
	this->m_detourShowWindow->Enable();
	this->m_session->StartConnect();
}

void Client::Stop()
{
	this->m_session->Stop();
}
void Client::Send(std::vector<uint8_t>& vData)
{
	std::cout << "Sending Data: " << vData.size() << std::endl; 
	this->m_session->Send(vData);
}
void Client::OnMessage( std::vector<uint8_t> vMessage )
{
}

void Client::OnDisconnect()
{
	std::cout << "Server Service was cancel'd. Maple Story will Terminate in a few seconds" << std::endl;
	Sleep(3000);
	TerminateProcess(GetCurrentProcess(),EXIT_FAILURE);
}
