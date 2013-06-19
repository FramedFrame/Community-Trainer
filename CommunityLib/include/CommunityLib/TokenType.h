/*
 * Written by ChiLaX - ChiLaX1@live.de
 * Revision 1.0
 */

/*
 * Enumeration of token types.
 */
#pragma once
enum pTokenType {
	nByte, //normal byte - 55, **, ??
	increment, //increment this byte whenever packet is sent - 55++
	decrement, //decrement this byte whenever packet is sent - 55--
	portalcount, //replace with the value of the portal (usage count) pointer
};