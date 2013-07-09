/*
 * Written by ChiLaX - ChiLaX1@live.de
 * Revision 1.0
 */
#pragma once
#include "PacketToken.h"
#include <vector>
#include <string>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <cctype>
#include <time.h> 

using namespace std;

/*
 * Packet Class for parsing the string packet into its tokens (bytes being tokens aswell).
 */
class Packet {
public:
	Packet(string iPacket)
	{
			this->sPacket = iPacket;
			transform(this->sPacket.begin(), this->sPacket.end(),this->sPacket.begin(), ::toupper);

			stringstream ss(this->sPacket);
			string item;
			while (getline(ss, item, ' ')) {
				if (!item.empty())
					elems.push_back(item);
			}

			this->length = elems.size();
	}
	~Packet()
	{

	}

	virtual bool tokenize();
	vector<PacketToken>& getTokens();

private:
	string sPacket; //input string defining the packet
	vector<string> elems;//string vector for the elements separated by whitespaces
	unsigned int length; //total number of bytes + tokens in the packet
	vector<PacketToken> tokens; //vector containing tokens after calling tokenize
};