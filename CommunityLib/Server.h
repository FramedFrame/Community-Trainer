#pragma once
#include "Network.h"
#include "Session.h"
#include "LibSocket.h"


namespace Network
{
class _Server
{
public:
	_Server(std::function<void(LibSocket::Session&)> funAccept,LibSocket::ServiceProvider& serviceProvider);
	~_Server(void);

	void Start();
	void Stop();
private:
	boost::asio::io_service& m_ioService;
	boost::asio::ip::tcp::acceptor m_acceptor;
	boost::asio::ip::tcp::socket *m_pSocket;
	std::function<void(LibSocket::Session&)> m_funAcceptSocket;

	bool m_fIsListening,m_fStop;

	void handle_accept(const boost::system::error_code& error);
};
}

