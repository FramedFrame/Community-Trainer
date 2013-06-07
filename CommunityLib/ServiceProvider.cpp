#include "ServiceProvider.h"

using namespace Network;

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
