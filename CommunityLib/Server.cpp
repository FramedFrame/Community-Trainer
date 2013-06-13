#include "Server.h"



using namespace Network;

using namespace boost;
using namespace boost::asio;

_Server::_Server(const std::function<void(LibSocket::Session&)>& funAccept,LibSocket::ServiceProvider& serviceProvider) :
	m_ioService(*reinterpret_cast<io_service*>(serviceProvider())),
	m_acceptor(*reinterpret_cast<io_service*>(serviceProvider()),ip::tcp::endpoint(ip::tcp::v4(),Network::PORT))
{
	this->m_funAcceptSocket = funAccept;
	this->m_fIsListening = false;
	this->m_fStop = false;
}


_Server::~_Server(void)
{
}

void _Server::Start()
{
	if(this->m_fIsListening)
		return;

	this->m_fIsListening = true;
	this->m_fStop = false;

	this->m_pSocket = new ip::tcp::socket(this->m_ioService);
	this->m_acceptor.async_accept(*m_pSocket,boost::bind(&_Server::handle_accept,this,boost::asio::placeholders::error));
}
void _Server::Stop()
{
	this->m_fStop = true;
}

void _Server::handle_accept(const boost::system::error_code& error)
{
	if(this->m_fStop)
	{
		delete this->m_pSocket;
		return;
	}

	if(!error)
	{
		auto x = new Network::_Session(*this->m_pSocket);
		auto p = new LibSocket::Session(x);
		this->m_funAcceptSocket(*p);
		this->m_pSocket = new ip::tcp::socket(this->m_ioService);
		this->m_acceptor.async_accept(*m_pSocket,boost::bind(&_Server::handle_accept,this,boost::asio::placeholders::error));
	}
}
