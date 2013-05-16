#pragma once

#ifndef INC_CTEAMSPEAK_H
#define INC_CTEAMSPEAK_H

#include "main.h"

#include <boost/regex.hpp>
#include <sstream>

using std::stringstream;




class CTeamspeak {
public:
	static int SocketID;
	static bool Connect(const char *ip, const char *port);
	static bool Close();

	static bool Send(string cmd);
	static int Recv(string *dest);
	
	static bool ExpectIntVal(string valname, int *val, int *error);

	static int ParseError(string str);
	static int ParseInteger(string str, string valname);

	static bool EscapeString(string *str);
};


enum ChannelTypes {
	CHANNEL_TYPE_PERMANENT,
	CHANNEL_TYPE_SEMI_PERMANENT,
	CHANNEL_TYPE_TEMPORARY
};

#endif