#pragma once
#include "LibSocket.h"
#include "Session.h"
#include "Server.h"

using namespace LibSocket;

#define CAST_LOW(x) reinterpret_cast<x*>(this->m_pClass)

Session::Session(LibSocket::ServiceProvider& serviceProvider)
{
	auto pSess = new Network::_Session(serviceProvider);
	this->m_pClass = pSess;
}
Session::Session(void* pBase)
{
	this->m_pClass = pBase;
}
Session::~Session(void)
{
	auto pSess = CAST_LOW(Network::_Session);
	delete pSess;
}

void Session::LinkFunctions(std::function<void(std::vector<uint8_t>&)>& funOnData,std::function<void()>& funOnDisconnect)
{
	auto pSess = CAST_LOW(Network::_Session);
	pSess->LinkFunctions(funOnData,funOnDisconnect);
}

void Session::StartConnect()
{
	auto pSess = CAST_LOW(Network::_Session);
	pSess->StartConnect();
}

void Session::Start()
{
	auto pSess = CAST_LOW(Network::_Session);
	pSess->Start();
}
void Session::Stop()
{
	auto pSess = CAST_LOW(Network::_Session);
	pSess->Stop();
}

void Session::Send(std::vector<uint8_t>& vBuffer)
{
	auto pSess = CAST_LOW(Network::_Session);
	pSess->Send(vBuffer);
}
void Session::SendRaw(std::vector<uint8_t>& vBuffer)
{
	auto pSess = CAST_LOW(Network::_Session);
	pSess->SendRaw(vBuffer);
}



Server::Server(std::function<void(LibSocket::Session&)> funAccept,LibSocket::ServiceProvider& serviceProvider)
{
	auto pSess = new Network::_Server(funAccept,serviceProvider);
	this->m_pClass = pSess;
}
Server::~Server(void)
{
	auto pSess = CAST_LOW(Network::_Server);
	delete pSess;
}

void Server::Start()
{
	auto pSess = CAST_LOW(Network::_Server);
	pSess->Start();
}
void Server::Stop()
{
	auto pSess = CAST_LOW(Network::_Server);
	pSess->Stop();
}

ServiceProvider::ServiceProvider(void)
{
	this->m_pio_service = new boost::asio::io_service();
}


ServiceProvider::~ServiceProvider(void)
{
	delete this->m_pio_service;
}

void* ServiceProvider::operator()()
{
	return this->m_pio_service;
}
void ServiceProvider::Run()
{
	(reinterpret_cast<boost::asio::io_service*>(this->m_pio_service))->run();
}
void ServiceProvider::Stop()
{
	(reinterpret_cast<boost::asio::io_service*>(this->m_pio_service))->stop();
}

