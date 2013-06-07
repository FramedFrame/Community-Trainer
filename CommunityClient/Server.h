#pragma once
#include "Network.h"

namespace Network
{
class Server
{
public:
	Server(boost::function<void(boost::asio::ip::tcp::socket&)> funAccept,boost::asio::io_service& io_service);
	~Server(void);

	void Start();
	void Stop();
private:
	boost::asio::io_service& m_ioService;
	boost::asio::ip::tcp::acceptor m_acceptor;
	boost::asio::ip::tcp::socket *m_pSocket;
	boost::function<void(boost::asio::ip::tcp::socket&)> m_funAcceptSocket;

	bool m_fIsListening,m_fStop;

	void handle_accept(const boost::system::error_code& error);
};
}

