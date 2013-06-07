#include "Session.h"
#include <Windows.h>

using namespace Network;

using namespace boost;
using namespace boost::asio;

_Session::_Session(ip::tcp::socket& socket)
{
	this->m_ptrSocket.reset(&socket);
	this->m_header = 0;
	this->m_writeState = WRITE_STATE::HANDSHAKE;
	this->m_fError = false;
	this->m_fStop = false;
}

_Session::_Session(LibSocket::ServiceProvider& serviceProvider)
{
	io_service* pServ = reinterpret_cast<io_service*>(serviceProvider());
	this->m_ptrSocket.reset(new ip::tcp::socket(*pServ));
	this->m_ptrResolver.reset(new ip::tcp::resolver(*pServ));
	this->m_header = 0;
	this->m_writeState = WRITE_STATE::HEADER;
	this->m_fError = false;
	this->m_fStop = false;
}
_Session::~_Session(void)
{
	this->Stop();
	this->m_ptrSocket.reset();

}
void _Session::LinkFunctions(std::function<void(std::vector<uint8_t>&)>& funOnData,std::function<void()>& funOnDisconnect)
{
	this->m_funOnData = funOnData;
	this->m_funOnDisconnect = funOnDisconnect;
}

void _Session::StartConnect()
{
	this->m_fError = false;
	this->m_fStop = false;

	ip::tcp::resolver::query q(Network::HOST,boost::lexical_cast<std::string>(Network::PORT));

	this->m_ptrResolver->async_resolve(q,boost::bind(&_Session::handle_resolve,this,boost::asio::placeholders::error,boost::asio::placeholders::iterator));
}

void _Session::Start()
{
	this->m_fError = false;
	this->m_fStop = false;


	boost::asio::async_read(*this->m_ptrSocket.get(),boost::asio::buffer(this->GetBuffer(),this->GetSize()),
		 boost::bind(&_Session::handle_read, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
}
void _Session::Stop()
{
	this->m_fStop = true;
	this->m_ptrSocket->close();
}

void _Session::Send(std::vector<uint8_t>& vBuffer)
{
	auto vSendBuffer = new std::vector<uint8_t>(vBuffer.size() + HEADER_SIZE);

	header_t header = vBuffer.size();

	memcpy(vSendBuffer->data(),&header,HEADER_SIZE);
	memcpy(vSendBuffer->data()+HEADER_SIZE,vBuffer.data(),vBuffer.size());

	this->m_ptrSocket->async_send(boost::asio::buffer(*vSendBuffer),boost::bind(&_Session::handle_write,
		this,boost::asio::placeholders::error,vSendBuffer));
}

void _Session::SendRaw(std::vector<uint8_t>& vBuffer)
{
	auto vSendBuffer = new std::vector<uint8_t>(vBuffer.begin(),vBuffer.end());

	this->m_ptrSocket->async_send(boost::asio::buffer(*vSendBuffer),boost::bind(&_Session::handle_write,
		this,boost::asio::placeholders::error,vSendBuffer));
}

void _Session::handle_resolve(const boost::system::error_code& error,ip::tcp::resolver::iterator i)
{
	if(!error && !this->m_fStop)
	{
		ip::tcp::endpoint endpoint = *i;
		this->m_ptrSocket->async_connect(endpoint,boost::bind(&_Session::handle_connect,this,boost::asio::placeholders::error));
	}
	else
	{
		this->m_fError = true;
		this->m_funOnDisconnect();
	}
}
void _Session::handle_connect(const boost::system::error_code& error)
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
void _Session::handle_write(const boost::system::error_code& error,std::vector<uint8_t>* vBuffer)
{
	if(error || this->m_fStop)
	{
		this->m_fError = true;
		this->m_funOnDisconnect();
	}
	delete vBuffer;
}
void _Session::handle_read(const boost::system::error_code& error,size_t bytes_transferred)
{

	if(!error && !this->m_fStop && bytes_transferred)
	{
		if(this->m_writeState != WRITE_STATE::HEADER)
			this->m_funOnData(this->m_vBuffer);

		this->NextState();

		boost::asio::async_read(*this->m_ptrSocket.get(),boost::asio::buffer(this->GetBuffer(),this->GetSize()),
		 boost::bind(&_Session::handle_read, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
	}
	else if(!bytes_transferred)
	{
		return;
	}
	else
	{
		this->m_fError = true;
		this->m_funOnDisconnect();
	}
}

uint8_t* _Session::GetBuffer()
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
int _Session::GetSize()
{
	switch(this->m_writeState)
	{
	case WRITE_STATE::DATA:
		return this->m_header;
	case WRITE_STATE::HANDSHAKE:
		return 2;
	default:
		return HEADER_SIZE;
	}
}

void _Session::NextState()
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