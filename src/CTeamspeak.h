#pragma once

#ifndef INC_CTEAMSPEAK_H
#define INC_CTEAMSPEAK_H

#include "main.h"

#include <boost/regex.hpp>
#include <sstream>

#ifdef BOOST_REGEX_MAX_CACHE_BLOCKS
#undef BOOST_REGEX_MAX_CACHE_BLOCKS
#endif
#define BOOST_REGEX_MAX_CACHE_BLOCKS 32

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
	




};


/*
enum ChannelTypes {
	CHANNEL_TYPE_PERMANENT = 1,
	CHANNEL_TYPE_SEMI_PERMANENT,
	CHANNEL_TYPE_TEMPORARY
};
*/

enum KickTypes {
	KICK_TYPE_CHANNEL = 1,
	KICK_TYPE_SERVER
};
#endif
