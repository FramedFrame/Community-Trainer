#pragma once
#include "Network.h"
#include "LibSocket.h"

namespace Network
{
	enum class WRITE_STATE
	{
		HANDSHAKE,HEADER,DATA
	};

class _Session
{
public:
	_Session(boost::asio::ip::tcp::socket& socket);
	_Session(LibSocket::ServiceProvider& serviceProvider);
	~_Session(void);

	void LinkFunctions(const std::function<void(std::vector<uint8_t>&)>& funOnData,const std::function<void()>& funOnDisconnect);

	void StartConnect();

	void Start();
	void Stop();

	void Send(std::vector<uint8_t>& vBuffer);
	void SendRaw(std::vector<uint8_t>& vBuffer);
private:
	bool m_fError,m_fStop;
	std::shared_ptr<boost::asio::ip::tcp::socket> m_ptrSocket;
	std::shared_ptr<boost::asio::ip::tcp::resolver> m_ptrResolver;

	std::function<void(std::vector<uint8_t>&)> m_funOnData;
	std::function<void()> m_funOnDisconnect;

	WRITE_STATE m_writeState;
	std::vector<uint8_t> m_vBuffer;
	header_t m_header;

	void handle_resolve(const boost::system::error_code& error,boost::asio::ip::tcp::resolver::iterator i);
	void handle_connect(const boost::system::error_code& error);
	void handle_write(const boost::system::error_code& error,std::vector<uint8_t>* vBuffer);
	void handle_read(const boost::system::error_code& error,size_t bytes_transferred);

	uint8_t* GetBuffer();
	int GetSize();
	void NextState();
};
}

