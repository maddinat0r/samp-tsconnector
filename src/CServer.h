#pragma once
#ifndef INC_SERVER_H
#define INC_SERVER_H


#include <string>
#include <list>
#include <vector>
#include <boost/unordered_map.hpp>
#include <boost/regex.hpp>
#include <boost/atomic.hpp>

#include "CSingleton.h"

using std::string;
using std::list;
using std::vector;
using boost::unordered_map;


enum E_CHANNEL_TYPES
{
	CHANNEL_TYPE_INVALID,
	CHANNEL_TYPE_TEMPORARY,
	CHANNEL_TYPE_SEMI_PERMANENT,
	CHANNEL_TYPE_PERMANENT
};

enum E_KICK_TYPES
{
	KICK_TYPE_INVALID,
	KICK_TYPE_CHANNEL,
	KICK_TYPE_SERVER
};


struct Client
{
	typedef unsigned int Id_t;

	Client() {}
	
	string
		Uid,
		Nickname;

	string Desc;

	struct Channel *CurrentChannel;
};

struct Channel
{
	typedef unsigned int Id_t;
	
	Channel() :
		ParentId(0),
		OrderId(0),

		Type(CHANNEL_TYPE_INVALID),
		HasPassword(false),

		MaxClients(-1)
	{}
	
	Id_t
		ParentId,
		OrderId;

	string Name;
	unsigned int Type; //temporary, permanent, semi-permanent
	bool HasPassword;

	list<unsigned int> Clients;
	int MaxClients;
};


class CServer : public CSingleton <CServer>
{
	friend class CSingleton <CServer>;
private: //variables
	unordered_map<unsigned int, Channel *> m_Channels;
	unsigned int m_DefaultChannel;

	unordered_map<unsigned int, Client *> m_Clients;

	boost::atomic<bool> m_IsLoggedIn;

private: //constructor / deconstructor
	CServer() :
		m_DefaultChannel(NULL),
		m_IsLoggedIn(false)
	{}
	~CServer() {}


private: //functions (internal)
	inline bool IsValidChannel(Channel::Id_t cid) const
	{
		return (m_Channels.find(cid) != m_Channels.end());
	}

public: //functions
	void Initialize();

	bool Login(string login, string pass);
	bool ChangeNickname(string nickname);

	bool CreateChannel(string name);
	bool DeleteChannel(Channel::Id_t cid);
	bool SetChannelName(Channel::Id_t cid, string name);
	inline string GetChannelName(Channel::Id_t cid) const
	{
		return IsValidChannel(cid) ? m_Channels.at(cid)->Name : string();
	}
	bool SetChannelDescription(Channel::Id_t cid, string desc);


public: //network callbacks
	void OnLogin(vector<string> &res);
	void OnChannelList(vector<string> &res);


public: //event callbacks
	void OnChannelCreated(boost::smatch &result);
	void OnChannelDeleted(boost::smatch &result);
};


#endif // INC_SERVER_H
