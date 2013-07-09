/*
 * Written by ChiLaX - ChiLaX1@live.de
 * Revision 1.0
 */

#include "Packet.h"

/*
 * Helper function for testing wether a string represents an integer number or not.
 */
bool isNumeric(string s)
{
	for (unsigned int i = 0; i < s.length(); i++)
		if (!isdigit(s[i]))
			return false;
	return true;
}


/*
 * Parses the elements of the string vector into tokens (and bytes).
 * Returns true if succesful, else it return false.
 */
bool Packet::tokenize()
{
	unsigned int count = 0, b;
	srand (time(NULL));
	string temp;

	for (std::vector<string>::iterator it = elems.begin() ; it != elems.end(); ++it)
	{
		PacketToken token;
		if ((*it).length() == 2) //byte
		{
			if (isxdigit((*it).front()) && isxdigit((*it)[1]))
			{
				token.tokenType = nByte;
				token.length = 1;
				token.index = count;
				stringstream ss(*it);
				ss >> hex >> b;
				token.byte = b;
				token.word = 0;
				this->tokens.push_back(token);
				//cout << "Token at index " << token.index << "- byte - value = " << (int)token.byte << endl;
			}
			else if (( *it == "??" ) || ( *it == "**" )) //randomized byte
			{
				token.tokenType = nByte;
				token.length = 1;
				token.index = count;
				token.byte = rand() % 256;
				token.word = 0;
				this->tokens.push_back(token);
				//cout << "Token at index " << token.index << "- byte - value = " << (int)token.byte << endl;
			}
			else
				return false;
		}
		else if (((*it).front() == '[') && ((*it).back() == ']'))
		{
			string buf = (*it).erase(0, 1);
			buf = buf.erase(buf.size()-1, 1);
			size_t pos;
			if (((pos = buf.find("--", 1)) != string::npos) || ((pos = buf.find("++", 1)) != string::npos)) //decrement or increment
			{
				if (pos != buf.length()-2)
					return false;
				if ((pos = buf.find(",", 1)) == string::npos) 
					return false;
				if (pos != buf.find_last_of(",")) //only one ',' in segment
					return false;

				stringstream ss(buf);
				getline(ss, temp, ',');
				if (!isNumeric(temp))
					return false;
				int num = atoi(temp.c_str());
				getline(ss, temp, ',');
				temp = temp.erase(temp.length()-2, string::npos);
				if (!isNumeric(temp))
					return false;
				int wor = atoi(temp.c_str());
				if ((pos = buf.find("--", 1)) != string::npos)
					token.tokenType = decrement;
				else
					token.tokenType = increment;
				token.length = num;
				token.index = count;
				token.byte = 0;
				token.word = wor;
				//cout << "Token at index " << count << "- inc/dec - value = " << token.word << " - length: " << token.length << endl;
			}
			else if ((pos = buf.find("-", 1)) != string::npos) //customized random
			{
				if (pos != buf.find_last_of("-"))
					return false;

				stringstream ss(buf);
				getline(ss, temp, '-');
				if (!isNumeric(temp))
					return false;
				int from = atoi(temp.c_str());
				getline(ss, temp, '-');
				if (!isNumeric(temp))
					return false;
				int to = atoi(temp.c_str());
				token.tokenType = nByte;
				token.length = 1;
				token.index = count;
				token.byte = (rand() % (to - from + 1) + from);
				token.word = 0;
				this->tokens.push_back(token);
				//cout << "Token at index " << count << "- byte - value = " << (int)token.byte << endl;
			}
			else if ((pos = buf.find("PORTALCOUNT", 1)) != string::npos) //portal count
			{
				if (pos != buf.length()-11)
					return false;
				if ((pos = buf.find(",",1)) == string::npos)
					return false;
				if (pos != buf.find_last_of(","))
					return false;

				stringstream ss(buf);
				getline(ss, temp, ',');
				if (!isNumeric(temp))
					return false;
				int num = atoi(temp.c_str());
				token.tokenType = portalcount;
				token.length = num;
				token.index = count;
				token.byte = 0;
				token.word = 0;
				this->tokens.push_back(token);
				//cout << "Token at index " << count << "- portal count - length = " << token.length << endl;
			}
			else
				return false;
		}
		else
			return false;

		count++;
	}
	return true;
}

/*
 * Returns the vector containing the tokens.
 */
vector<PacketToken>& Packet::getTokens() {
	return this->tokens;
}