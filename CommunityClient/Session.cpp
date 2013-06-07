#include "Session.h"

#include <Windows.h>

using namespace Network;

using namespace boost;
using namespace boost::asio;

Session::Session(ip::tcp::socket& socket,boost::function<void(std::vector<uint8_t>&)>& funOnData,boost::function<void()>& funOnDisconnect) :
	m_funOnData(funOnData),
	m_funOnDisconnect(funOnDisconnect)
{
	this->m_ptrSocket.reset(&socket);
	this->m_header = 0;
	this->m_writeState = WRITE_STATE::HANDSHAKE;
	this->m_fError = false;
	this->m_fStop = false;
}

Session::Session(io_service& io_service,boost::function<void(std::vector<uint8_t>&)>& funOnData,boost::function<void()>& funOnDisconnect) :
	m_funOnData(funOnData),
	m_funOnDisconnect(funOnDisconnect)
{
	this->m_ptrSocket.reset(new ip::tcp::socket(io_service));
	this->m_ptrResolver.reset(new ip::tcp::resolver(io_service));
	this->m_header = 0;
	this->m_writeState = WRITE_STATE::HEADER;
	this->m_fError = false;
	this->m_fStop = false;
}
Session::~Session(void)
{
	this->Stop();
}

void Session::StartConnect()
{
	this->m_fError = false;
	this->m_fStop = false;

	ip::tcp::resolver::query q(Network::HOST,boost::lexical_cast<std::string>(Network::PORT));

	this->m_ptrResolver->async_resolve(q,boost::bind(&Session::handle_resolve,this,boost::asio::placeholders::error,boost::asio::placeholders::iterator));
}

void Session::Start()
{
	this->m_fError = false;
	this->m_fStop = false;

	this->m_ptrSocket->async_read_some(boost::asio::buffer(this->GetBuffer(),this->GetSize()),
		 boost::bind(&Session::handle_read, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
}
void Session::Stop()
{
	this->m_fStop = true;
	this->m_ptrSocket->close();
}

void Session::Send(std::vector<uint8_t>& vBuffer)
{
	auto vSendBuffer = new std::vector<uint8_t>(vBuffer.size() + HEADER_SIZE);

	header_t header = vBuffer.size();

	memcpy(vSendBuffer->data(),&header,HEADER_SIZE);
	memcpy(vSendBuffer->data()+HEADER_SIZE,vBuffer.data(),vBuffer.size());

	this->m_ptrSocket->async_send(boost::asio::buffer(*vSendBuffer),boost::bind(&Session::handle_write,
		this,boost::asio::placeholders::error,placeholders::bytes_transferred,vSendBuffer));
}

void Session::SendRaw(std::vector<uint8_t>& vBuffer)
{
	auto vSendBuffer = new std::vector<uint8_t>(vBuffer.begin(),vBuffer.end());

	this->m_ptrSocket->async_send(boost::asio::buffer(*vSendBuffer),boost::bind(&Session::handle_write,
		this,boost::asio::placeholders::error,placeholders::bytes_transferred,vSendBuffer));
}

void Session::handle_resolve(const boost::system::error_code& error,ip::tcp::resolver::iterator i)
{
	if(!error && !this->m_fStop)
	{
		ip::tcp::endpoint endpoint = *i;
		this->m_ptrSocket->async_connect(endpoint,boost::bind(&Session::handle_connect,this,boost::asio::placeholders::error));
	}
	else
	{
		this->m_fError = true;
		this->m_funOnDisconnect();
	}
}
void Session::handle_connect(const boost::system::error_code& error)
{
	if(!error && !this->m_fStop)
	{
		std::vector<uint8_t> vHandshake(4);
		uint32_t uProcessId = GetCurrentProcessId();
		memcpy(vHandshake.data(),&uProcessId,4);
		this->SendRaw(vHandshake);
		this->Start();
	}
	else
	{
		this->m_fError = true;
		this->m_funOnDisconnect();
	}
}
void Session::handle_write(const boost::system::error_code& error,std::size_t bytes_transferred,std::vector<uint8_t>* vBuffer)
{
	if(error || this->m_fStop)
	{
		this->m_fError = true;
		this->m_funOnDisconnect();
	}
	else
	{
		delete vBuffer;
		printf("Sent: %i\n",bytes_transferred);
	}

}
void Session::handle_read(const boost::system::error_code& error,size_t bytes_transferred)
{
	if(!error && !this->m_fStop)
	{
		if(this->m_writeState != WRITE_STATE::HEADER)
			this->m_funOnData(this->m_vBuffer);

		this->NextState();

		this->m_ptrSocket->async_read_some(boost::asio::buffer(this->GetBuffer(),this->GetSize()),
		 boost::bind(&Session::handle_read, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
	}
	else
	{
		this->m_fError = true;
		this->m_funOnDisconnect();
	}
}

uint8_t* Session::GetBuffer()
{
	switch(this->m_writeState)
	{
	case WRITE_STATE::DATA:
		this->m_vBuffer.resize(this->m_header);
		return this->m_vBuffer.data();
	case WRITE_STATE::HANDSHAKE:
		this->m_vBuffer.resize(4);
		return this->m_vBuffer.data();
	default:
		return reinterpret_cast<uint8_t*>(&this->m_header);
	}
}
int Session::GetSize()
{
	switch(this->m_writeState)
	{
	case WRITE_STATE::DATA:
		return this->m_header;
	case WRITE_STATE::HANDSHAKE:
		return 4;
	default:
		return HEADER_SIZE;
	}
}

void Session::NextState()
{
	switch(this->m_writeState)
	{
	case WRITE_STATE::DATA:
	case WRITE_STATE::HANDSHAKE:
		this->m_writeState = WRITE_STATE::HEADER;
		break;
	default:
		this->m_writeState = WRITE_STATE::DATA;
	}
}