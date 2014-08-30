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
#include <boost/thread/lock_guard.hpp>

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
	Channel::Id_t GetChannelIdByName(string name);
	inline bool IsValidChannel(Channel::Id_t cid)
	{
		boost::lock_guard<mutex> channel_mtx_guard(m_ChannelMtx);
		return (m_Channels.find(cid) != m_Channels.end());
	}
	bool SetChannelName(Channel::Id_t cid, string name);
	string GetChannelName(Channel::Id_t cid);
	bool SetChannelDescription(Channel::Id_t cid, string desc);
	bool SetChannelType(Channel::Id_t cid, Channel::Types type);
	Channel::Types GetChannelType(Channel::Id_t cid);
	bool SetChannelPassword(Channel::Id_t cid, string password);
	bool HasChannelPassword(Channel::Id_t cid);
	bool SetChannelRequiredTalkPower(Channel::Id_t cid, int talkpower);
	int GetChannelRequiredTalkPower(Channel::Id_t cid);
	bool SetChannelUserLimit(Channel::Id_t cid, int maxusers);
	int GetChannelUserLimit(Channel::Id_t cid);
	bool SetChannelParentId(Channel::Id_t cid, Channel::Id_t pcid);
	Channel::Id_t GetChannelParentId(Channel::Id_t cid);
	bool SetChannelOrderId(Channel::Id_t cid, Channel::Id_t ocid);
	Channel::Id_t GetChannelOrderId(Channel::Id_t cid);
	inline Channel::Id_t GetDefaultChannelId()
	{
		boost::lock_guard<mutex> channel_mtx_guard(m_ChannelMtx);
		return m_DefaultChannel;
	}


public: //client functions
	Client::Id_t GetClientIdByUid(string uid);
	inline bool IsValidClient(Client::Id_t clid)
	{
		boost::lock_guard<mutex> client_mtx_guard(m_ClientMtx);
		return (m_Clients.find(clid) != m_Clients.end());
	}
	string GetClientUid(Client::Id_t clid);
	Client::Id_t GetClientDatabaseId(Client::Id_t clid);
	Channel::Id_t GetClientChannelId(Client::Id_t clid);

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
