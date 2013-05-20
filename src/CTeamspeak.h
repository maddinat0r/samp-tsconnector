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
	static string 
		IP, Port,
		LoginName, LoginPass;

	static bool Connect(string ip/*, const char *port*/);
	static int Login(string login, string pass);
	static int SetActiveVServer(string port);

	static bool Close();
	static bool SetTimeoutTime(unsigned int millisecs);

	static bool Send(string cmd);
	static int Recv(string *dest);
	
	static bool ExpectIntVal(string valname, int *val, int *error);
	static bool ExpectStringVal(string valname, string *val, int *error);

	static int ParseError(string str);
	static int ParseInteger(string str, string valname);
	static bool ParseString(string str, string valname, string *dest);

	static bool EscapeString(string *str);
	static bool UnEscapeString(string *str);


	static int GetClientDBIDByID(int clientid);
};


enum ChannelTypes {
	CHANNEL_TYPE_PERMANENT = 1,
	CHANNEL_TYPE_SEMI_PERMANENT,
	CHANNEL_TYPE_TEMPORARY
};

enum KickTypes {
	KICK_TYPE_CHANNEL = 1,
	KICK_TYPE_SERVER
};
#endif