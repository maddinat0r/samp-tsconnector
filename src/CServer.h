#pragma once
#ifndef INC_SERVER_H
#define INC_SERVER_H


#include <string>
#include <list>
#include <vector>
#include <queue>
#include <boost/unordered_map.hpp>
#include <boost/regex.hpp>
#include <boost/atomic.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/lock_guard.hpp>
#include <boost/lockfree/spsc_queue.hpp>

#include "CSingleton.h"

using std::string;
using std::list;
using std::vector;
using std::queue;
using boost::atomic;
using boost::unordered_map;
using boost::mutex;

class CCallback;


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

	enum class QueryData
	{
		INVALID,
		CHANNEL_TOPIC, //channel_topic [string]
		CHANNEL_DESCRIPTION, //channel_description [string]
		CHANNEL_CODEC, //channel_codec [int]
		CHANNEL_CODEC_QUALITY, //channel_codec_quality [int]
		CHANNEL_FORCED_SILENCE, //channel_forced_silence [int]
		CHANNEL_ICON_ID, //channel_icon_id [int]
		CHANNEL_CODEC_IS_UNENCRYPTED, //channel_codec_is_unencrypted [int]
		CHANNEL_SECONDS_EMPTY //seconds_empty [int]
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

	enum class QueryData
	{
		INVALID,
		CLIENT_NICKNAME, //client_nickname [string]
		CLIENT_VERSION, //client_version [string]
		CLIENT_PLATFORM, //client_platform [string]
		CLIENT_INPUT_MUTED, //client_input_muted [int]
		CLIENT_OUTPUT_MUTED, //client_output_muted [int]
		CLIENT_INPUT_HARDWARE, //client_input_hardware [int]
		CLIENT_OUTPUT_HARDWARE, //client_output_hardware [int]
		CLIENT_CHANNEL_GROUP_ID, //client_channel_group_id [int]
		CLIENT_SERVER_GROUPS, //client_servergroups [string (list of numbers, separated by a comma)]
		CLIENT_FIRSTCONNECTED, //client_created [int (UTC timestamp of first connection)]
		CLIENT_LASTCONNECTED, //client_lastconnected [int (UTC timestamp of last connection)]
		CLIENT_TOTALCONNECTIONS, //client_totalconnections [int]
		CLIENT_AWAY, //client_away [int]
		CLIENT_AWAY_MESSAGE, //client_away_message [string]
		CLIENT_AVATAR, //client_flag_avatar [string] (CRC checksum?)
		CLIENT_TALK_POWER, //client_talk_power [int]
		CLIENT_TALK_REQUEST, //client_talk_request [int]
		CLIENT_TALK_REQUEST_MSG, //client_talk_request_msg [string]
		CLIENT_IS_TALKER, //client_is_talker [int]
		CLIENT_IS_PRIORITY_SPEAKER, //client_is_priority_speaker [int]
		CLIENT_DESCRIPTION, //client_description [string]
		CLIENT_IS_CHANNEL_COMMANDER, //client_is_channel_commander [int]
		CLIENT_ICON_ID, //client_icon_id [int]
		CLIENT_COUNTRY, //client_country [string]
		CLIENT_IDLE_TIME, //client_idle_time [int] (seconds)
		CLIENT_IS_RECORDING, //client_is_recording [int]
	};


	Id_t DatabaseId = Invalid;
	string
		Uid,
		IpAddress;

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

	atomic<bool> m_IsLoggedIn;

	unsigned int m_ServerId = 0;

	boost::lockfree::spsc_queue<
			string,
			boost::lockfree::fixed_sized<true>,
			boost::lockfree::capacity<32678>
		> m_QueriedData;
	string m_ActiveQueriedData;


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


public: //data query functions
	bool QueryChannelData(Channel::Id_t cid, Channel::QueryData data, CCallback *callback);
	bool QueryClientData(Client::Id_t clid, Client::QueryData data, CCallback *callback);
	bool GetQueriedData(string &dest);
	bool GetQueriedData(int &dest);


public: //channel functions
	bool CreateChannel(string name, Channel::Types type = Channel::Types::TEMPORARY,
		int maxusers = -1, Channel::Id_t pcid = Channel::Invalid, Channel::Id_t ocid = Channel::Invalid,
		int talkpower = 0);
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
	Client::Id_t GetClientIdByIpAddress(string ip);
	inline bool IsValidClient(Client::Id_t clid)
	{
		boost::lock_guard<mutex> client_mtx_guard(m_ClientMtx);
		return (m_Clients.find(clid) != m_Clients.end());
	}
	string GetClientUid(Client::Id_t clid);
	Client::Id_t GetClientDatabaseId(Client::Id_t clid);
	Channel::Id_t GetClientChannelId(Client::Id_t clid);
	string GetClientIpAddress(Client::Id_t clid);

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
