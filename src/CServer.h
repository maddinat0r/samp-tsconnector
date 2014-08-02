#pragma once
#ifndef INC_SERVER_H
#define INC_SERVER_H


#include <string>
#include <list>
#include <vector>
#include <boost/unordered_map.hpp>
#include <boost/regex.hpp>
#include <boost/atomic.hpp>
#include <boost/thread/mutex.hpp>

#include "CSingleton.h"

using std::string;
using std::list;
using std::vector;
using boost::unordered_map;
using boost::mutex;


struct Channel
{
	typedef unsigned int Id_t;
	static const Id_t Invalid = -1;

	enum class Types
	{
		INVALID,
		TEMPORARY,
		SEMI_PERMANENT,
		PERMANENT
	};
	

	Id_t
		ParentId = Invalid,
		OrderId = Invalid;

	string Name;
	Types Type = Types::INVALID;
	bool
		HasPassword = false,
		WasPasswordToggled = false;
	int RequiredTalkPower = 0;

	list<unsigned int> Clients;
	int MaxClients = -1;
};

struct Client
{
	typedef unsigned int Id_t;
	static const Id_t Invalid = 0;

	enum class KickTypes
	{
		INVALID,
		CHANNEL,
		SERVER
	};

	Id_t DatabaseId = Invalid;
	string Uid;

	Channel::Id_t CurrentChannel = Channel::Invalid;
};


class CServer : public CSingleton <CServer>
{
	friend class CSingleton <CServer>;
private: //variables
	unordered_map<Channel::Id_t, Channel *> m_Channels;
	Channel::Id_t m_DefaultChannel = Channel::Invalid;
	mutex m_ChannelMtx;

	unordered_map<Client::Id_t, Client *> m_Clients;
	mutex m_ClientMtx;

	boost::atomic<bool> m_IsLoggedIn;

	unsigned int m_ServerId = 0;


private: //constructor / deconstructor
	CServer() :
		m_IsLoggedIn(false)
	{}
	~CServer();


private: //functions (internal)
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
	Channel::Id_t FindChannel(string name);
	inline bool IsValidChannel(Channel::Id_t cid) const
	{
		return (m_Channels.find(cid) != m_Channels.end());
	}
	bool SetChannelName(Channel::Id_t cid, string name);
	inline string GetChannelName(Channel::Id_t cid) const
	{
		return IsValidChannel(cid) ? m_Channels.at(cid)->Name : string();
	}
	bool SetChannelDescription(Channel::Id_t cid, string desc);
	bool SetChannelType(Channel::Id_t cid, Channel::Types type);
	inline Channel::Types GetChannelType(Channel::Id_t cid) const
	{
		return IsValidChannel(cid) ? m_Channels.at(cid)->Type : Channel::Types::INVALID;
	}
	bool SetChannelPassword(Channel::Id_t cid, string password);
	inline bool HasChannelPassword(Channel::Id_t cid) const
	{
		return IsValidChannel(cid) ? m_Channels.at(cid)->HasPassword : false;
	}
	bool SetChannelRequiredTalkPower(Channel::Id_t cid, int talkpower);
	inline int GetChannelRequiredTalkPower(Channel::Id_t cid) const
	{
		return IsValidChannel(cid) ? m_Channels.at(cid)->RequiredTalkPower : 0;
	}
	bool SetChannelUserLimit(Channel::Id_t cid, int maxusers);
	inline int GetChannelUserLimit(Channel::Id_t cid) const
	{
		return IsValidChannel(cid) ? m_Channels.at(cid)->MaxClients : 0;
	}
	bool SetChannelParentId(Channel::Id_t cid, Channel::Id_t pcid);
	Channel::Id_t GetChannelParentId(Channel::Id_t cid) const
	{
		return IsValidChannel(cid) ? m_Channels.at(cid)->ParentId : Channel::Invalid;
	}
	bool SetChannelOrderId(Channel::Id_t cid, Channel::Id_t ocid);
	Channel::Id_t GetChannelOrderId(Channel::Id_t cid) const
	{
		return IsValidChannel(cid) ? m_Channels.at(cid)->OrderId : Channel::Invalid;
	}
	Channel::Id_t GetDefaultChannelId() const
	{
		return m_DefaultChannel;
	}


public: //client functions
	inline bool IsValidClient(Client::Id_t clid) const
	{
		return (m_Clients.find(clid) != m_Clients.end());
	}
	inline string GetClientUid(Client::Id_t clid)
	{
		return IsValidClient(clid) ? m_Clients.at(clid)->Uid : string();
	}
	inline Client::Id_t GetClientDatabaseId(Client::Id_t clid)
	{
		return IsValidClient(clid) ? m_Clients.at(clid)->DatabaseId : Client::Invalid;
	}
	inline Channel::Id_t GetClientChannelId(Client::Id_t clid)
	{
		return IsValidClient(clid) ? m_Clients.at(clid)->CurrentChannel : Channel::Invalid;
	}

	bool KickClient(Client::Id_t clid, Client::KickTypes type, string reasonmsg);
	bool BanClient(string uid, int seconds, string reasonmsg);
	bool MoveClient(Client::Id_t clid, Channel::Id_t cid);

	bool SetClientChannelGroup(Client::Id_t clid, int groupid, Channel::Id_t cid);
	bool AddClientToServerGroup(Client::Id_t clid, int groupid);
	bool RemoveClientFromServerGroup(Client::Id_t clid, int groupid);

	bool PokeClient(Client::Id_t clid, string msg);
	bool SendClientMessage(Client::Id_t clid, string msg);


public: //network callbacks
	void OnLogin(vector<string> &res);
	void OnChannelList(vector<string> &res);
	void OnClientList(vector<string> &res);

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
	void OnChannelRequiredTalkPowerChanged(boost::smatch &result);


	void OnClientConnect(boost::smatch &result);
	void OnClientDisconnect(boost::smatch &result);
	void OnClientMoved(boost::smatch &result);
	void OnClientServerText(boost::smatch &result);
	void OnClientPrivateText(boost::smatch &result);
};


#endif // INC_SERVER_H
