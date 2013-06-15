#include "Client.h"


Client::Client(LibSocket::ServiceProvider& serviceProvider)
{
	this->m_session.reset(new LibSocket::Session(serviceProvider));
	this->m_session->LinkFunctions(std::bind(&Client::OnMessage,this,std::placeholders::_1),
		std::bind(&Client::OnDisconnect,this));
}


Client::~Client(void)
{
}

void Client::Start()
{
	this->m_session->StartConnect();
}

void Client::Stop()
{
	this->m_session->Stop();
}

void Client::OnMessage( std::vector<uint8_t> vMessage )
{
}

void Client::OnDisconnect()
{

}
