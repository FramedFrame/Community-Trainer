#include "Host.h"

#include "Session.h"
#include "ClientManager.h"

#include "Context.h"


Host::Host()
{

	this->m_server.reset(new LibSocket::Server(std::bind(&Host::NewSession,this,std::placeholders::_1),
		*ContextInstance->ServiceProvider.get()));
}


Host::~Host(void)
{
}

void Host::Start()
{
	this->m_server->Start();
}
void Host::Stop()
{
	this->m_server->Stop();
}

void Host::NewSession( LibSocket::Session& session )
{
	Session* pSession = new Session(session);
}
