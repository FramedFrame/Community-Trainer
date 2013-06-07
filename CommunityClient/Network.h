#pragma once
#ifdef _DEBUG
#define D_WIN32_WINNT 0x0501
#else
#define _WIN32_WINNT 0x0501
#endif


#include <iostream>
#include <memory>
#include <stdint.h>
#include <string>
#include <vector>

#include <boost\bind.hpp>
#include <boost\asio.hpp>
#include <boost\lexical_cast.hpp>

namespace Network
{
	typedef uint16_t header_t;
	const int HEADER_SIZE = sizeof(header_t); 

	const std::string HOST = "127.0.0.1";
	const int16_t PORT = 23561;
}