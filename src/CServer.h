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


struct Channel
{
	typedef unsigned int Id_t;
	
	Channel() :
		ParentId(0),
		OrderId(0),

		Type(CHANNEL_TYPE_INVALID),
		HasPassword(false),
		WasPasswordToggled(false),

		MaxClients(-1)
	{}
	
	Id_t
		ParentId,
		OrderId;

	string Name;
	unsigned int Type; //temporary, permanent, semi-permanent
	bool
		HasPassword,
		WasPasswordToggled;

	list<unsigned int> Clients;
	int MaxClients;
};

struct Client
{
	typedef unsigned int Id_t;

	Client() {}

	string
		Uid,
		Nickname;

	string Desc;

	Channel::Id_t CurrentChannel;
};


class CServer : public CSingleton <CServer>
{
	friend class CSingleton <CServer>;
private: //variables
	unordered_map<Channel::Id_t, Channel *> m_Channels;
	Channel::Id_t m_DefaultChannel;

	unordered_map<Client::Id_t, Client *> m_Clients;

	boost::atomic<bool> m_IsLoggedIn;

	unsigned int m_ServerId;


private: //constructor / deconstructor
	CServer() :
		m_DefaultChannel(0),
		m_IsLoggedIn(false),
		m_ServerId(0)
	{}
	~CServer();


private: //functions (internal)
	inline bool IsValidChannel(Channel::Id_t cid) const
	{
		return (m_Channels.find(cid) != m_Channels.end());
	}

	void Initialize();


public: //server functions
	bool Login(string login, string pass);
	bool ChangeNickname(string nickname);
	inline bool IsLoggedIn() const
	{
		return m_IsLoggedIn;
	}

	bool SendServerMessage(string msg);

public: //channel functions
	bool CreateChannel(string name);
	bool DeleteChannel(Channel::Id_t cid);
	bool SetChannelName(Channel::Id_t cid, string name);
	inline string GetChannelName(Channel::Id_t cid) const
	{
		return IsValidChannel(cid) ? m_Channels.at(cid)->Name : string();
	}
	bool SetChannelDescription(Channel::Id_t cid, string desc);
	bool SetChannelType(Channel::Id_t cid, unsigned int type);
	inline unsigned int GetChannelType(Channel::Id_t cid) const
	{
		return IsValidChannel(cid) ? m_Channels.at(cid)->Type : CHANNEL_TYPE_INVALID;
	}
	bool SetChannelPassword(Channel::Id_t cid, string password);
	inline bool HasChannelPassword(Channel::Id_t cid) const
	{
		return IsValidChannel(cid) ? m_Channels.at(cid)->HasPassword : false;
	}
	bool SetChannelUserLimit(Channel::Id_t cid, int maxusers);
	inline int GetChannelUserLimit(Channel::Id_t cid) const
	{
		return IsValidChannel(cid) ? m_Channels.at(cid)->MaxClients : 0;
	}
	bool SetChannelParentId(Channel::Id_t cid, Channel::Id_t pcid);
	Channel::Id_t GetChannelParentId(Channel::Id_t cid) const
	{
		return IsValidChannel(cid) ? m_Channels.at(cid)->ParentId : 0;
	}
	bool SetChannelOrderId(Channel::Id_t cid, Channel::Id_t ocid);
	Channel::Id_t GetChannelOrderId(Channel::Id_t cid) const
	{
		return IsValidChannel(cid) ? m_Channels.at(cid)->OrderId : 0;
	}


public: //client functions


public: //network callbacks
	void OnLogin(vector<string> &res);
	void OnChannelList(vector<string> &res);
	void OnServerIdRetrieved(vector<string> &res);


public: //event callbacks
	void OnChannelCreated(boost::smatch &result);
	void OnChannelDeleted(boost::smatch &result);
	void OnChannelReorder(boost::smatch &result);
	void OnChannelMoved(boost::smatch &result);
	void OnChannelRenamed(boost::smatch &result);
	void OnChannelPasswordToggled(boost::smatch &result);
	void OnChannelPasswordChanged(boost::smatch &result);
	void OnChannelTypeChanged(boost::smatch &result);
	void OnChannelSetDefault(boost::smatch &result);
	void OnChannelMaxClientsChanged(boost::smatch &result);
};


#endif // INC_SERVER_H
