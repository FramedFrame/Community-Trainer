#pragma once

//Client Opcode Set
enum class ClientOpcode
{
	INVALID,WINDOW_HANDLE,WINDOW_SHOWN,VALUE_REQUEST_RESPONSE
};

//Server Opcode Set
enum class ServerOpcode
{
	INVALID,VALUE_REQUEST
};