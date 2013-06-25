/*
 * Written by ChiLaX - ChiLaX1@live.de
 * Revision 1.0
 */
<<<<<<< HEAD
#pragma once
#include "TokenType.h"
=======

/*
 * Enumeration of token types.
 */
enum pTokenType {
	nByte, //normal byte - 55, **, ??
	increment, //increment this byte whenever packet is sent - 55++
	decrement, //decrement this byte whenever packet is sent - 55--
	portalcount, //replace with the value of the portal (usage count) pointer
};
>>>>>>> 89e9af0bb688f620446ca999dc97e78fec6dbe33

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