#include "Server.h"

using namespace Network;

using namespace boost;
using namespace boost::asio;

Server::Server(function<void(ip::tcp::socket&)> funAccept,io_service& io_service) :
	m_ioService(io_service),
	m_acceptor(io_service,ip::tcp::endpoint(ip::tcp::v4(),Network::PORT))
{
	this->m_funAcceptSocket = funAccept;
	this->m_fIsListening = false;
	this->m_fStop = false;
}


Server::~Server(void)
{
}

void Server::Start()
{
	if(!this->m_fIsListening)
		return;

	this->m_fIsListening = true;
	this->m_fStop = false;

	this->m_pSocket = new ip::tcp::socket(this->m_ioService);
	this->m_acceptor.async_accept(*m_pSocket,boost::bind(&Server::handle_accept,this,boost::asio::placeholders::error));
}
void Server::Stop()
{
	this->m_fStop = true;
}

void Server::handle_accept(const boost::system::error_code& error)
{
	if(this->m_fStop)
	{
		delete this->m_pSocket;
		return;
	}

	if(!error)
	{
		this->m_funAcceptSocket(*this->m_pSocket);
		this->m_pSocket = new ip::tcp::socket(this->m_ioService);
		this->Start();
	}
}
