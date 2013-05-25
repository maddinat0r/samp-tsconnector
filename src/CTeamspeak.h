#pragma once

#ifndef INC_CTEAMSPEAK_H
#define INC_CTEAMSPEAK_H

#include "main.h"
#include "thread.h"

#include <boost/regex.hpp>
#include <sstream>
#include <queue>

#ifdef BOOST_REGEX_MAX_CACHE_BLOCKS
#undef BOOST_REGEX_MAX_CACHE_BLOCKS
#endif
#define BOOST_REGEX_MAX_CACHE_BLOCKS 32

using std::stringstream;
using std::queue;



class CCommand {
public:
	CCommand(string cmd, string valname=string()) :
		Command(cmd), ValName(valname) {}
	CCommand() :
		Command(string()), ValName(string()) {}
	
	string Command;
	//Recv only
	string ValName;

	//multiple search
	string MFind;
};

typedef queue<CCommand*> CommandList;

class CTeamspeak {
private:
	static CMutex SocketMutex;
	static queue<CommandList*> SendQueue;

	int SocketID;
	string 
		IP, Port,
		LoginName, LoginPass, LoginNick;
public:
	bool Connect(string ip, string vport);
	int Login(string login, string pass, string nickname);

	bool Disconnect();
	bool SetTimeoutTime(unsigned int millisecs);

	bool Send(string cmd);
	int Recv(string *dest);
	
	static bool AddCommandListToQueue(CommandList *cmdlist);
	static CommandList *GetNextCommandList();
	static bool IsQueueEmpty();

	bool EscapeString(string *str);
	bool UnEscapeString(string *str);

	bool IsLoggedIn() {
		return (LoginName.size() > 0); }

	CTeamspeak();
	~CTeamspeak();
};
extern CTeamspeak TSServer;

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
