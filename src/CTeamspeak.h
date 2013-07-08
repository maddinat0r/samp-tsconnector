#pragma once

#ifndef INC_CTEAMSPEAK_H
#define INC_CTEAMSPEAK_H

#include "main.h"
#include "thread.h"

#include <queue>

#ifdef BOOST_REGEX_MAX_CACHE_BLOCKS
#undef BOOST_REGEX_MAX_CACHE_BLOCKS
#endif
#define BOOST_REGEX_MAX_CACHE_BLOCKS 32
#include <boost/regex.hpp>

#include <boost/lexical_cast.hpp>

using std::queue;



class CCommand {
public:
	CCommand(string cmd, string valname=string()) :
		Command(cmd), ValName(valname) {}
	
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

	bool LoggedIn, Connected;
public:
	bool ConnectT();
	int LoginT();
	
	bool Connect(char *ip, char *vport);
	int Login(char *login, char *pass, char *nickname);

	bool Disconnect();
	bool SetTimeoutTime(unsigned int millisecs);

	int Send(string cmd);
	int Recv(string *dest);
	
	static bool AddCommandListToQueue(CommandList *cmdlist);
	static CommandList *GetNextCommandList();

	bool EscapeString(string &str);
	bool UnEscapeString(string &str);

	inline bool IsLoggedIn() const {
		return LoggedIn; }

	inline bool IsConnected() const {
		return Connected; }

	inline const char *GetPort() const {
		return Port.c_str();
	}

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
