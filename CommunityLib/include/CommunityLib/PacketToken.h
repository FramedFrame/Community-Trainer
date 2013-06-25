/*
 * Written by ChiLaX - ChiLaX1@live.de
 * Revision 1.0
 */
#pragma once
#include "TokenType.h"
/*
 * PacketToken structure to contain information in an array making it possible 
 * to directly access the token by an index and knowing its type and byte count.
 */
struct PacketToken {
	unsigned int index; //in packet
	unsigned int length; //byte count
	pTokenType tokenType; //from enumeration TokenType
	unsigned char byte; //if TokenType is byte, the value is saved here
	int word; //if TokenType is increment or decrement, the value is saved here
};