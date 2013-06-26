#include "Session.h"

#include "ClientManager.h"
#include "ClientContext.h"
#include "Context.h"

#include <CommunityLib\Reader.h>
#include <CommunityLib\Opcode.h>

#include <iostream>


Session::Session(LibSocket::Session& session)
{
	this->m_session.reset(&session);
	this->m_fIsHandshake = true;

	this->m_session->LinkFunctions(std::bind(&Session::OnMessage,this,std::placeholders::_1),
		std::bind(&Session::OnDisconnect,this));

	this->m_session->Start();
}

Session::~Session(void)
{
	this->Stop();
}

void Session::Stop()
{
	this->m_session->Stop();
}

void Session::OnMessage( std::vector<uint8_t> vMessage )
{
	if(this->m_fIsHandshake)
	{
		this->m_uProcessId = *reinterpret_cast<uint32_t*>(vMessage.data());
		this->m_fIsHandshake = false;
		if(!ContextInstance->ClientManager->NewSession(*this))
			this->Stop();
		return;
	}

	IO::Reader reader(vMessage);
	try
	{
		auto opcode = reader.Read<uint8_t>();
		switch(opcode)
		{
		case ClientOpcode::WINDOW_HANDLE:
			this->m_clientContext->Window = reinterpret_cast<HWND>(reader.Read<uint32_t>());
			break;
		case ClientOpcode::WINDOW_SHOWN:
			this->m_clientContext->Panel->AttachWindow((nana::gui::native_window_type)this->m_clientContext->Window);
			std::cout << "Shown Window" << std::endl;
		}
	}
	catch(std::exception* ex)
	{

	}
}

void Session::OnDisconnect()
{
	ContextInstance->ClientManager->SessionDisconnect(*this);
}

uint32_t Session::operator()()
{
	return this->m_uProcessId;
}

void Session::AppendData(std::shared_ptr<ClientContext>& pData)
{
	this->m_clientContext = pData;
}
