#include "Session.h"

#include "ClientManager.h"


Session::Session(ClientManager* pClientManager,LibSocket::Session& session)
{
	this->m_pClientManager = pClientManager;

	this->m_session.reset(&session);
	this->m_fIsHandshake = true;

	this->m_session->LinkFunctions(std::bind(&Session::OnMessage,this,std::placeholders::_1),
		std::bind(&Session::OnDisconnect,this));

	this->m_pClientManager->NewSession(*this);

	this->m_session->Start();
}

Session::~Session(void)
{

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
		if(this->m_pClientManager->NewSession(*this))
			this->Stop();
		return;
	}
}

void Session::OnDisconnect()
{
	this->m_pClientManager->SessionDisconnect(*this);
}

uint32_t Session::operator()()
{
	return this->m_uProcessId;
}
