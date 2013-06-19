#pragma once
#include <functional>
#include <iostream>
#include <memory>
#include <stdint.h>
#include <string>
#include <vector>


#ifdef _LIB
#include <boost\bind.hpp>
#include <boost\asio.hpp>
#include <boost\lexical_cast.hpp>
#include <boost\function.hpp>
#endif



namespace Network
{
	typedef uint16_t header_t;
	const int HEADER_SIZE = sizeof(header_t); 

	const std::string HOST = "127.0.0.1";
	const int16_t PORT = 23561;
}