#pragma once
#include "Network.h"

namespace Network
{
	enum class WRITE_STATE
	{
		HANDSHAKE,HEADER,DATA
	};

class Session
{
public:
	Session(boost::asio::ip::tcp::socket& socket,boost::function<void(std::vector<uint8_t>&)>& funOnData,boost::function<void()>& funOnDisconnect);
	Session(boost::asio::io_service& io_service,boost::function<void(std::vector<uint8_t>&)>& funOnData,boost::function<void()>& funOnDisconnect);
	~Session(void);

	void StartConnect();

	void Start();
	void Stop();

	void Send(std::vector<uint8_t>& vBuffer);
	void SendRaw(std::vector<uint8_t>& vBuffer);
private:
	bool m_fError,m_fStop;
	boost::shared_ptr<boost::asio::ip::tcp::socket> m_ptrSocket;
	boost::shared_ptr<boost::asio::ip::tcp::resolver> m_ptrResolver;

	boost::function<void(std::vector<uint8_t>&)> m_funOnData;
	boost::function<void()> m_funOnDisconnect;

	WRITE_STATE m_writeState;
	std::vector<uint8_t> m_vBuffer;
	header_t m_header;

	void handle_resolve(const boost::system::error_code& error,boost::asio::ip::tcp::resolver::iterator i);
	void handle_connect(const boost::system::error_code& error);
	void handle_write(const boost::system::error_code& error,std::size_t bytes_transferred,std::vector<uint8_t>* vBuffer);
	void handle_read(const boost::system::error_code& error,size_t bytes_transferred);

	uint8_t* GetBuffer();
	int GetSize();
	void NextState();
};
}

