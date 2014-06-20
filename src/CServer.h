#pragma once
#ifndef INC_SERVER_H
#define INC_SERVER_H


#include <string>
#include <list>
#include <vector>

#include "CSingleton.h"

using std::string;
using std::list;
using std::vector;


enum E_CHANNEL_TYPES
{
	CHANNEL_TYPE_PERMANENT = 1,
	CHANNEL_TYPE_SEMI_PERMANENT,
	CHANNEL_TYPE_TEMPORARY
};

enum E_KICK_TYPES
{
	KICK_TYPE_CHANNEL = 1,
	KICK_TYPE_SERVER
};

//timeout time: 10 minutes



struct Client
{
	unsigned int Id;

	string
		Uid,
		Nickname;

	string Desc;

	struct Channel *CurrentChannel;
};

struct Channel
{
	unsigned int Id;

	string
		Name,
		Desc;

	string Password;

	list<Client *> Clients;

	int Type; //temporary, permanent, semi-permanent
	int NeededTalkPower;

	list<Channel *> SubChannels;
};


class CServer : public CSingleton < CServer >
{
private: //variables
	list<Channel *> m_Channels;
	list<Client *> m_Clients;


public: //functions
	void Initialize();


public: //network callbacks
	void OnPortSet(vector<string> &res);

};


#endif // INC_SERVER_H
