#include "CServer.hpp"
#include "CNetwork.hpp"
#include "CUtils.hpp"
#include "CCallback.hpp"

#include "main.hpp"
#include "format.h"


void CServer::Initialize()
{
	//register notify events
	CNetwork::Get()->RegisterEvent(
		boost::regex("notifychannelcreated cid=([0-9]+) cpid=([0-9]+) channel_name=([^ ]+)(.+)invokerid=[0-9]+ invokername=[^ ]+ invokeruid=[^ \n\r]+"),
		boost::bind(&CServer::OnChannelCreated, this, _1));

	CNetwork::Get()->RegisterEvent(
		boost::regex("notifychanneldeleted .+ cid=([0-9]+)"),
		boost::bind(&CServer::OnChannelDeleted, this, _1));
	
	CNetwork::Get()->RegisterEvent(
		boost::regex("notifychanneledited cid=([0-9]+) reasonid=10 invokerid=[0-9]+ invokername=[^ ]+ invokeruid=[^ ]+ channel_order=([0-9]+)"),
		boost::bind(&CServer::OnChannelReorder, this, _1));

	CNetwork::Get()->RegisterEvent(
		boost::regex("notifychannelmoved cid=([0-9]+) cpid=([0-9]+) order=([0-9]+) reasonid=1 invokerid=[0-9]+ invokername=[^ ]+ invokeruid=[^ \n\r]+"),
		boost::bind(&CServer::OnChannelMoved, this, _1));

	CNetwork::Get()->RegisterEvent(
		boost::regex("notifychanneledited cid=([0-9]+) reasonid=10 invokerid=[0-9]+ invokername=[^ ]+ invokeruid=[^ ]+ channel_name=([^ \n\r]+)"),
		boost::bind(&CServer::OnChannelRenamed, this, _1));

	CNetwork::Get()->RegisterEvent(
		boost::regex("notifychanneledited cid=([0-9]+) reasonid=10 invokerid=[0-9]+ invokername=[^ ]+ invokeruid=[^ ]+ channel_flag_password=([01])"),
		boost::bind(&CServer::OnChannelPasswordToggled, this, _1));

	CNetwork::Get()->RegisterEvent(
		boost::regex("notifychannelpasswordchanged cid=([0-9]+)"),
		boost::bind(&CServer::OnChannelPasswordChanged, this, _1));
	//NOTE: always called if something with password changed, check if channel_flag_password was changed before fireing "pass-changed" callback

	CNetwork::Get()->RegisterEvent(
		boost::regex("notifychanneledited cid=([0-9]+) reasonid=10 invokerid=[0-9]+ invokername=[^ ]+ invokeruid=[^ ]+ (?:channel_flag_(permanent|semi_permanent)=([01]))(?: channel_flag_(permanent|semi_permanent)=([01]))?.*"),
		boost::bind(&CServer::OnChannelTypeChanged, this, _1));
	//DATA:
	//	1: cid
	//	2: flag name ("permanent", "semi_permanent")
	//	3: flag status ("0", "1")
	//	4: second flag name (optional)
	//	5: second flag status (optional)

	CNetwork::Get()->RegisterEvent(
		boost::regex("notifychanneledited cid=([0-9]+) reasonid=10 invokerid=[0-9]+ invokername=[^ ]+ invokeruid=[^ ]+ channel_flag_default=1.*"),
		boost::bind(&CServer::OnChannelSetDefault, this, _1));

	CNetwork::Get()->RegisterEvent(
		boost::regex("notifychanneledited cid=([0-9]+) reasonid=10 invokerid=[0-9]+ invokername=[^ ]+ invokeruid=[^ ]+ channel_maxclients=([-0-9]+).*"),
		boost::bind(&CServer::OnChannelMaxClientsChanged, this, _1));

	CNetwork::Get()->RegisterEvent(
		boost::regex("notifychanneledited cid=([0-9]+) reasonid=10 invokerid=[0-9]+ .+ channel_needed_talk_power=([-0-9]+)"),
		boost::bind(&CServer::OnChannelRequiredTalkPowerChanged, this, _1));


	CNetwork::Get()->RegisterEvent(
		boost::regex("notifycliententerview cfid=0 ctid=([0-9]+) reasonid=0 clid=([0-9]+) client_unique_identifier=([^ ]+) client_nickname=([^ ]+) .+ client_database_id=([0-9]+) .+ client_type=([01]).*"),
		boost::bind(&CServer::OnClientConnect, this, _1));

	CNetwork::Get()->RegisterEvent(
		boost::regex("notifyclientleftview cfid=[0-9]+ ctid=0(?: reasonid=([0-9]+).* reasonmsg(?:=([^ ]+))?.*)? clid=([0-9]+).*"),
		boost::bind(&CServer::OnClientDisconnect, this, _1));

	CNetwork::Get()->RegisterEvent(
		boost::regex("notifyclientmoved ctid=([0-9]+) reasonid=([0-9]+)(?: invokerid=([0-9]+))?.* (clid=.*)"),
		boost::bind(&CServer::OnClientMoved, this, _1));

	CNetwork::Get()->RegisterEvent(
		boost::regex("notifytextmessage targetmode=3 msg=([^ ]+) invokerid=([0-9]+) invokername=([^ ]+).*"),
		boost::bind(&CServer::OnClientServerText, this, _1));

	CNetwork::Get()->RegisterEvent(
		boost::regex("notifytextmessage targetmode=1 msg=([^ ]+) target=([0-9]+) invokerid=([0-9]+) invokername=([^ ]+).*"),
		boost::bind(&CServer::OnClientPrivateText, this, _1));
	


	//register for all events
	CNetwork::Get()->Execute("servernotifyregister event=server");
	CNetwork::Get()->Execute("servernotifyregister event=channel id=0");
	CNetwork::Get()->Execute("servernotifyregister event=textserver");
	CNetwork::Get()->Execute("servernotifyregister event=textprivate");



	//fill up cache
	CNetwork::Get()->Execute("channellist -flags -limit -voice",
		boost::bind(&CServer::OnChannelList, this, _1));
	CNetwork::Get()->Execute("clientlist -uid -ip",
		boost::bind(&CServer::OnClientList, this, _1));



	//retrieve vserver-id
	CNetwork::Get()->Execute(
		fmt::format("serveridgetbyport virtualserver_port={}", CNetwork::Get()->GetServerPort()),
		[this](CNetwork::ResultSet_t &result)
		{
			if (result.empty() == false)
				CUtils::Get()->ParseField(result.at(0), "server_id", m_ServerId);
		});
}



bool CServer::Login(string login, string pass)
{
	CUtils::Get()->EscapeString(login);
	CUtils::Get()->EscapeString(pass);
	
	CNetwork::Get()->Execute(
		fmt::format("login client_login_name={} client_login_password={}", login, pass),
		boost::bind(&CServer::OnLogin, this, _1));
	return true;
}

bool CServer::ChangeNickname(string nickname)
{
	if (m_IsLoggedIn == false)
		return false;


	CUtils::Get()->EscapeString(nickname);
	CNetwork::Get()->Execute(fmt::format("clientupdate client_nickname={}", nickname));
	return true;
}

bool CServer::SendServerMessage(string msg)
{
	if (m_IsLoggedIn == false)
		return false;

	if (msg.empty())
		return false;

	if (m_ServerId == 0)
		return false;


	CUtils::Get()->EscapeString(msg);
	CNetwork::Get()->Execute(
		fmt::format("sendtextmessage targetmode=3 target={} msg={}", m_ServerId, msg));
	return true;
}




bool CServer::QueryChannelData(Channel::Id_t cid, Channel::QueryData data, Callback_t callback)
{
	if (IsValidChannel(cid) == false)
		return false;
	
	if (data == Channel::QueryData::INVALID || callback == nullptr)
		return false;

	
	static const unordered_map<Channel::QueryData, string> data_map{
		{Channel::QueryData::CHANNEL_TOPIC,					"channel_topic" },
		{Channel::QueryData::CHANNEL_DESCRIPTION,			"channel_description" },
		{Channel::QueryData::CHANNEL_CODEC,					"channel_codec" },
		{Channel::QueryData::CHANNEL_CODEC_QUALITY,			"channel_codec_quality" },
		{Channel::QueryData::CHANNEL_FORCED_SILENCE,		"channel_forced_silence" },
		{Channel::QueryData::CHANNEL_ICON_ID,				"channel_icon_id" },
		{Channel::QueryData::CHANNEL_CODEC_IS_UNENCRYPTED,	"channel_codec_is_unencrypted" },
		{Channel::QueryData::CHANNEL_SECONDS_EMPTY,			"seconds_empty" }
	};

	auto it = data_map.find(data);
	if (it == data_map.end())
		return false;


	CNetwork::Get()->Execute(fmt::format("channelinfo cid={}", cid),
		[=](CNetwork::ResultSet_t &result)
	{
		string
			&info = result.at(0),
			data_dest;

		CUtils::Get()->ParseField(info, it->second, data_dest);
		CUtils::Get()->UnEscapeString(data_dest);
		m_QueriedData.push(data_dest);

		callback->OnPreExecute([this]()
		{
			m_QueriedData.pop(m_ActiveQueriedData);
		});
		callback->OnPostExecute([this]()
		{
			m_ActiveQueriedData.clear();
		});
		CCallbackHandler::Get()->Call(callback);
	});
	return true;
}

bool CServer::QueryClientData(Client::Id_t clid, Client::QueryData data, Callback_t callback)
{
	if (IsValidClient(clid) == false)
		return false;

	if (data == Client::QueryData::INVALID || callback == nullptr)
		return false;


	static const unordered_map<Client::QueryData, string> data_map{
		{ Client::QueryData::CLIENT_NICKNAME,				"client_nickname" },
		{ Client::QueryData::CLIENT_VERSION,				"client_version" },
		{ Client::QueryData::CLIENT_PLATFORM,				"client_platform" },
		{ Client::QueryData::CLIENT_INPUT_MUTED,			"client_input_muted" },
		{ Client::QueryData::CLIENT_OUTPUT_MUTED,			"client_output_muted" },
		{ Client::QueryData::CLIENT_INPUT_HARDWARE,			"client_input_hardware" },
		{ Client::QueryData::CLIENT_OUTPUT_HARDWARE,		"client_output_hardware" },
		{ Client::QueryData::CLIENT_CHANNEL_GROUP_ID,		"client_channel_group_id" },
		{ Client::QueryData::CLIENT_SERVER_GROUPS,			"client_servergroups" },
		{ Client::QueryData::CLIENT_FIRSTCONNECTED,			"client_created" },
		{ Client::QueryData::CLIENT_LASTCONNECTED,			"client_lastconnected" },
		{ Client::QueryData::CLIENT_TOTALCONNECTIONS,		"client_totalconnections" },
		{ Client::QueryData::CLIENT_AWAY,					"client_away" },
		{ Client::QueryData::CLIENT_AWAY_MESSAGE,			"client_away_message" },
		{ Client::QueryData::CLIENT_AVATAR,					"client_flag_avatar" },
		{ Client::QueryData::CLIENT_TALK_POWER,				"client_talk_power" },
		{ Client::QueryData::CLIENT_TALK_REQUEST,			"client_talk_request" },
		{ Client::QueryData::CLIENT_TALK_REQUEST_MSG,		"client_talk_request_msg" },
		{ Client::QueryData::CLIENT_IS_TALKER,				"client_is_talker" },
		{ Client::QueryData::CLIENT_IS_PRIORITY_SPEAKER,	"client_is_priority_speaker" },
		{ Client::QueryData::CLIENT_DESCRIPTION,			"client_description" },
		{ Client::QueryData::CLIENT_IS_CHANNEL_COMMANDER,	"client_is_channel_commander" },
		{ Client::QueryData::CLIENT_ICON_ID,				"client_icon_id" },
		{ Client::QueryData::CLIENT_COUNTRY,				"client_country" },
		{ Client::QueryData::CLIENT_IDLE_TIME,				"client_idle_time" },
		{ Client::QueryData::CLIENT_IS_RECORDING,			"client_is_recording" }
	};

	auto it = data_map.find(data);
	if (it == data_map.end())
		return false;


	CNetwork::Get()->Execute(fmt::format("clientinfo clid={}", clid),
		[=](CNetwork::ResultSet_t &result)
	{
		string
			&info = result.at(0),
			data_dest;

		CUtils::Get()->ParseField(info, it->second, data_dest);
		CUtils::Get()->UnEscapeString(data_dest);
		m_QueriedData.push(data_dest);

		callback->OnPreExecute([this]()
		{
			m_QueriedData.pop(m_ActiveQueriedData);
		});
		callback->OnPostExecute([this]()
		{
			m_ActiveQueriedData.clear();
		});
		CCallbackHandler::Get()->Call(callback);
	});
	return true;
}

bool CServer::GetQueriedData(string &dest)
{
	dest = m_ActiveQueriedData;
	return true;
}

bool CServer::GetQueriedData(int &dest)
{
	return CUtils::Get()->ConvertStringToInt(m_ActiveQueriedData, dest);
}




bool CServer::CreateChannel(string name, Channel::Types type, int maxusers, 
	Channel::Id_t pcid, Channel::Id_t ocid, int talkpower)
{
	if (m_IsLoggedIn == false)
		return false;

	if (name.empty())
		return false;

	if (type == Channel::Types::INVALID)
		return false;

	if (maxusers < -1)
		return false;

	if (pcid != 0 && pcid != Channel::Invalid && IsValidChannel(pcid) == false)
		return false;

	if (ocid != 0 && ocid != Channel::Invalid && IsValidChannel(ocid) == false)
		return false;


	string type_flag_str;
	switch (type)
	{
	case Channel::Types::PERMANENT:
		type_flag_str = "channel_flag_permanent";
		break;

	case Channel::Types::SEMI_PERMANENT:
		type_flag_str = "channel_flag_semi_permanent";
		break;

	case Channel::Types::TEMPORARY:
		type_flag_str = "channel_flag_temporary";
		break;

	default:
		return false;
	}

	CUtils::Get()->EscapeString(name);
	

	string cmd = fmt::format("channelcreate channel_name={} {}=1", name, type_flag_str);

	if (maxusers != -1)
		cmd.append(fmt::format(" channel_maxclients={} channel_flag_maxclients_unlimited=0", maxusers));

	if (talkpower != 0)
		cmd.append(fmt::format(" channel_needed_talk_power={}", talkpower));

	if (pcid != Channel::Invalid)
		cmd.append(fmt::format(" cpid={}", pcid));
	
	if (ocid != Channel::Invalid)
		cmd.append(fmt::format(" channel_order={}", ocid));

	CNetwork::Get()->Execute(cmd);
	return true;
}

bool CServer::DeleteChannel(Channel::Id_t cid)
{
	if (m_IsLoggedIn == false)
		return false;

	if (IsValidChannel(cid) == false)
		return false;


	CNetwork::Get()->Execute(fmt::format("channeldelete cid={} force=1", cid),
		[this, cid](CNetwork::ResultSet_t &result)
		{
			boost::lock_guard<mutex> channel_mtx_guard(m_ChannelMtx);
			m_Channels.erase(cid);
		});
	return true;
}

bool CServer::SetChannelName(Channel::Id_t cid, string name)
{
	if (m_IsLoggedIn == false)
		return false;

	if (IsValidChannel(cid) == false)
		return false;

	if (name.empty())
		return false;
	

	string unescaped_name(name);

	CUtils::Get()->EscapeString(name);
	CNetwork::Get()->Execute(fmt::format("channeledit cid={} channel_name={}", cid, name),
		[this, cid, unescaped_name](CNetwork::ResultSet_t &result)
		{
			boost::lock_guard<mutex> channel_mtx_guard(m_ChannelMtx);
			m_Channels.at(cid)->Name = unescaped_name;
		});
	return true;
}

string CServer::GetChannelName(Channel::Id_t cid)
{
	if (IsValidChannel(cid))
	{
		boost::lock_guard<mutex> channel_mtx_guard(m_ChannelMtx);
		return m_Channels.at(cid)->Name;
	}
	else
		return string();
}

bool CServer::SetChannelDescription(Channel::Id_t cid, string desc)
{
	if (m_IsLoggedIn == false)
		return false;

	if (IsValidChannel(cid) == false)
		return false;


	CUtils::Get()->EscapeString(desc);
	CNetwork::Get()->Execute(fmt::format("channeledit cid={} channel_description={}", cid, desc));
	return true;
}

bool CServer::SetChannelType(Channel::Id_t cid, Channel::Types type)
{
	if (m_IsLoggedIn == false)
		return false;

	if (IsValidChannel(cid) == false)
		return false;


	string
		type_flag_str,
		old_type_flag_str;

	m_ChannelMtx.lock();
	auto current_type = m_Channels.at(cid)->Type;
	m_ChannelMtx.unlock();

	switch(type) 
	{
		case Channel::Types::PERMANENT:
			type_flag_str = "channel_flag_permanent";
		break;

		case Channel::Types::SEMI_PERMANENT:
			type_flag_str = "channel_flag_semi_permanent";
		break;

		case Channel::Types::TEMPORARY:
			type_flag_str = "channel_flag_temporary";
		break;

		default:
			return false;
	}

	switch (current_type)
	{
		case Channel::Types::PERMANENT:
			old_type_flag_str = "channel_flag_permanent";
			break;

		case Channel::Types::SEMI_PERMANENT:
			old_type_flag_str = "channel_flag_semi_permanent";
			break;

		case Channel::Types::TEMPORARY:
			old_type_flag_str = "channel_flag_temporary";
			break;

		default:
			return false;
	}

	CNetwork::Get()->Execute(fmt::format("channeledit cid={} {}=1 {}=0", cid, type_flag_str, old_type_flag_str),
		[this, cid, type](CNetwork::ResultSet_t &result)
		{
			boost::lock_guard<mutex> channel_mtx_guard(m_ChannelMtx);
			m_Channels.at(cid)->Type = type;
		});
	return true;
}

Channel::Types CServer:: GetChannelType(Channel::Id_t cid)
{
	if (IsValidChannel(cid))
	{
		boost::lock_guard<mutex> channel_mtx_guard(m_ChannelMtx);
		return m_Channels.at(cid)->Type;
	}
	else
		return Channel::Types::INVALID;
}

bool CServer::SetChannelPassword(Channel::Id_t cid, string password)
{
	if (m_IsLoggedIn == false)
		return false;

	if (IsValidChannel(cid) == false)
		return false;


	bool password_empty = password.empty();
	CUtils::Get()->EscapeString(password);
	CNetwork::Get()->Execute(fmt::format("channeledit cid={} channel_password={}", cid, password),
		[this, cid, password_empty](CNetwork::ResultSet_t &result)
		{
			boost::lock_guard<mutex> channel_mtx_guard(m_ChannelMtx);
			m_Channels.at(cid)->HasPassword = (password_empty == false);
		});
	return true;
}

bool CServer::HasChannelPassword(Channel::Id_t cid)
{
	if (IsValidChannel(cid))
	{
		boost::lock_guard<mutex> channel_mtx_guard(m_ChannelMtx);
		return m_Channels.at(cid)->HasPassword;
	}
	else
		return false;
}

bool CServer::SetChannelRequiredTalkPower(Channel::Id_t cid, int talkpower)
{
	if (m_IsLoggedIn == false)
		return false;

	if (IsValidChannel(cid) == false)
		return false;


	CNetwork::Get()->Execute(fmt::format("channeledit cid={} channel_needed_talk_power={}", cid, talkpower),
		[this, cid, talkpower](CNetwork::ResultSet_t &result)
		{
			boost::lock_guard<mutex> channel_mtx_guard(m_ChannelMtx);
			m_Channels.at(cid)->RequiredTalkPower = talkpower;
		});
	return true;
}

int CServer::GetChannelRequiredTalkPower(Channel::Id_t cid)
{
	if (IsValidChannel(cid))
	{
		boost::lock_guard<mutex> channel_mtx_guard(m_ChannelMtx);
		return m_Channels.at(cid)->RequiredTalkPower;
	}
	else
		return 0;
}

bool CServer::SetChannelUserLimit(Channel::Id_t cid, int maxusers)
{
	if (m_IsLoggedIn == false)
		return false;

	if (IsValidChannel(cid) == false)
		return false;

	if (maxusers < -1)
		return false;


	CNetwork::Get()->Execute(fmt::format("channeledit cid={} channel_maxclients={} channel_flag_maxclients_unlimited={}", cid, maxusers, maxusers == -1 ? 1 : 0),
		[this, cid, maxusers](CNetwork::ResultSet_t &result)
		{
			boost::lock_guard<mutex> channel_mtx_guard(m_ChannelMtx);
			m_Channels.at(cid)->MaxClients = maxusers;
		});
	return true;
}

int CServer::GetChannelUserLimit(Channel::Id_t cid)
{
	if (IsValidChannel(cid))
	{
		boost::lock_guard<mutex> channel_mtx_guard(m_ChannelMtx);
		return m_Channels.at(cid)->MaxClients;
	}
	else
		return 0;
}

bool CServer::SetChannelParentId(Channel::Id_t cid, Channel::Id_t pcid)
{
	if (m_IsLoggedIn == false)
		return false;

	if (IsValidChannel(cid) == false)
		return false;

	if (pcid != 0 && IsValidChannel(pcid) == false)
		return false;

	boost::lock_guard<mutex> channel_mtx_guard(m_ChannelMtx);

	if (m_Channels.at(cid)->ParentId == pcid)
		return false;


	CNetwork::Get()->Execute(fmt::format("channelmove cid={} cpid={}", cid, pcid),
		[this, cid, pcid](CNetwork::ResultSet_t &result)
		{
			boost::lock_guard<mutex> channel_mtx_guard(m_ChannelMtx);
			m_Channels.at(cid)->ParentId = pcid;
		});
	return true;
}

Channel::Id_t CServer::GetChannelParentId(Channel::Id_t cid)
{
	if (IsValidChannel(cid))
	{
		boost::lock_guard<mutex> channel_mtx_guard(m_ChannelMtx);
		return m_Channels.at(cid)->ParentId;
	}
	else
		return Channel::Invalid;
}

bool CServer::SetChannelOrderId(Channel::Id_t cid, Channel::Id_t ocid)
{
	if (m_IsLoggedIn == false)
		return false;

	if (IsValidChannel(cid) == false)
		return false;

	if (ocid != 0 && IsValidChannel(ocid) == false)
		return false;

	boost::lock_guard<mutex> channel_mtx_guard(m_ChannelMtx);

	if (m_Channels.at(cid)->OrderId == ocid)
		return false;


	CNetwork::Get()->Execute(fmt::format("channeledit cid={} channel_order={}", cid, ocid),
		[this, cid, ocid](CNetwork::ResultSet_t &result)
		{
			boost::lock_guard<mutex> channel_mtx_guard(m_ChannelMtx);
			m_Channels.at(cid)->OrderId = ocid;
		});
	return true;
}

Channel::Id_t CServer::GetChannelOrderId(Channel::Id_t cid)
{
	if (IsValidChannel(cid))
	{
		boost::lock_guard<mutex> channel_mtx_guard(m_ChannelMtx);
		return m_Channels.at(cid)->OrderId;
	}
	else
		return Channel::Invalid;
}

Channel::Id_t CServer::GetChannelIdByName(string name)
{
	boost::lock_guard<mutex> channel_mtx_guard(m_ChannelMtx);
	if (name.empty() == false)
	{
		for (auto &i : m_Channels)
		{
			if (i.second->Name == name)
				return i.first;
		}
	}

	return Channel::Invalid;
}



Client::Id_t CServer::GetClientIdByUid(string uid)
{
	boost::lock_guard<mutex> client_mtx_guard(m_ClientMtx);
	if (uid.empty() == false)
	{
		for (auto &i : m_Clients)
		{
			if (i.second->Uid == uid)
				return i.first;
		}
	}

	return Client::Invalid;
}

Client::Id_t CServer::GetClientIdByIpAddress(string ip)
{
	if (ip.empty())
		return Client::Invalid;

	boost::lock_guard<mutex> client_mtx_guard(m_ClientMtx);
	for (auto &i : m_Clients)
	{
		if (i.second->IpAddress.compare(ip) == 0)
			return i.first;
	}

	return Client::Invalid;
}

string CServer::GetClientUid(Client::Id_t clid)
{
	if (IsValidClient(clid))
	{
		boost::lock_guard<mutex> client_mtx_guard(m_ClientMtx);
		return m_Clients.at(clid)->Uid;
	}
	else
		return string();
}

Client::Id_t CServer::GetClientDatabaseId(Client::Id_t clid)
{
	if (IsValidClient(clid))
	{
		boost::lock_guard<mutex> client_mtx_guard(m_ClientMtx);
		return m_Clients.at(clid)->DatabaseId;
	}
	else
		return Client::Invalid;
}

Channel::Id_t CServer::GetClientChannelId(Client::Id_t clid)
{
	if (IsValidClient(clid))
	{
		boost::lock_guard<mutex> client_mtx_guard(m_ClientMtx);
		return m_Clients.at(clid)->CurrentChannel;
	}
	else
		return Channel::Invalid;
}

string CServer::GetClientIpAddress(Client::Id_t clid)
{
	if (IsValidClient(clid))
	{
		boost::lock_guard<mutex> client_mtx_guard(m_ClientMtx);
		return m_Clients.at(clid)->IpAddress;
	}
	else
		return string();
}

bool CServer::KickClient(Client::Id_t clid, Client::KickTypes type, string reasonmsg)
{
	if (IsValidClient(clid) == false)
		return false;

	if (type == Client::KickTypes::INVALID)
		return false;


	int kicktype_id;
	switch (type)
	{
		case Client::KickTypes::CHANNEL:
			kicktype_id = 4;
			break;

		case Client::KickTypes::SERVER:
			kicktype_id = 5;
			break;

		default:
			return false;
	}


	CUtils::Get()->EscapeString(reasonmsg);
	if (reasonmsg.length() > 40)
	{
		reasonmsg.resize(40);
		if (reasonmsg.back() == '\\')
			reasonmsg.pop_back();
	}

	CNetwork::Get()->Execute(fmt::format("clientkick clid={} reasonid={} reasonmsg={}", clid, kicktype_id, reasonmsg));
	return true;
}

bool CServer::BanClient(string uid, int seconds, string reasonmsg)
{
	if (uid.empty())
		return false;


	CUtils::Get()->EscapeString(uid);
	CUtils::Get()->EscapeString(reasonmsg);
	CNetwork::Get()->Execute(fmt::format("banadd uid={} time={} banreason={}", uid, seconds, reasonmsg));
	return true;
}

bool CServer::MoveClient(Client::Id_t clid, Channel::Id_t cid)
{
	if (IsValidClient(clid) == false)
		return false;

	if (IsValidChannel(cid) == false)
		return false;


	CNetwork::Get()->Execute(fmt::format("clientmove clid={} cid={}", clid, cid));
	return true;
}

bool CServer::SetClientChannelGroup(Client::Id_t clid, int groupid, Channel::Id_t cid)
{
	if (IsValidClient(clid) == false)
		return false;

	if (IsValidChannel(cid) == false)
		return false;


	boost::lock_guard<mutex> client_mtx_guard(m_ClientMtx);
	Client::Id_t dbid = m_Clients.at(clid)->DatabaseId;
	CNetwork::Get()->Execute(fmt::format("setclientchannelgroup cgid={} cid={} cldbid={}", groupid, cid, dbid));
	return true;
}

bool CServer::AddClientToServerGroup(Client::Id_t clid, int groupid)
{
	if (IsValidClient(clid) == false)
		return false;


	boost::lock_guard<mutex> client_mtx_guard(m_ClientMtx);
	Client::Id_t dbid = m_Clients.at(clid)->DatabaseId;
	CNetwork::Get()->Execute(fmt::format("servergroupaddclient sgid={} cldbid={}", groupid, dbid));
	return true;
}

bool CServer::RemoveClientFromServerGroup(Client::Id_t clid, int groupid)
{
	if (IsValidClient(clid) == false)
		return false;


	boost::lock_guard<mutex> client_mtx_guard(m_ClientMtx);
	Client::Id_t dbid = m_Clients.at(clid)->DatabaseId;
	CNetwork::Get()->Execute(fmt::format("servergroupdelclient sgid={} cldbid={}", groupid, dbid));
	return true;
}

bool CServer::SetClientTalkerStatus(Client::Id_t clid, bool status)
{
	if (IsValidClient(clid) == false)
		return false;


	boost::lock_guard<mutex> client_mtx_guard(m_ClientMtx);
	boost::lock_guard<mutex> channel_mtx_guard(m_ChannelMtx);

	Channel::Id_t cid = m_Clients.at(clid)->CurrentChannel;
	if (m_Channels.at(cid)->RequiredTalkPower == 0)
		return false;

	CNetwork::Get()->Execute(fmt::format(
		"clientedit clid={} client_is_talker={}", clid, status ? 1 : 0));
	return true;
}

bool CServer::SetClientDescription(Client::Id_t clid, string desc)
{
	if (IsValidClient(clid) == false)
		return false;


	CUtils::Get()->EscapeString(desc);
	CNetwork::Get()->Execute(fmt::format(
		"clientedit clid={} client_description={}", clid, desc));
	return true;
}

bool CServer::PokeClient(Client::Id_t clid, string msg)
{
	if (IsValidClient(clid) == false)
		return false;


	CUtils::Get()->EscapeString(msg);
	CNetwork::Get()->Execute(fmt::format("clientpoke clid={} msg={}", clid, msg));
	return true;
}

bool CServer::SendClientMessage(Client::Id_t clid, string msg)
{
	if (IsValidClient(clid) == false)
		return false;


	CUtils::Get()->EscapeString(msg);
	CNetwork::Get()->Execute(fmt::format("sendtextmessage targetmode=1 target={} msg={}", clid, msg));
	return true;
}




void CServer::OnLogin(vector<string> &res)
{
	Initialize();
	m_IsLoggedIn = true;


	CCallbackHandler::Get()->Call("TSC_OnConnect");
}

void CServer::OnChannelList(vector<string> &res)
{
	/*
	data (newline = space):
		cid=1
		pid=0
		channel_order=0
		channel_name=Default\sChannel
		channel_flag_default=1
		channel_flag_password=0
		channel_flag_permanent=1
		channel_flag_semi_permanent=0
		channel_needed_subscribe_power=0
		....
		channel_maxclients=-1
	*/
	boost::lock_guard<mutex> channel_mtx_guard(m_ChannelMtx);
	for (auto &res_row : res)
	{
		unsigned int
			cid = 0,
			pid = 0,
			order = 0,
			is_default = 0,
			has_password = 0,
			is_permanent = 0,
			is_semi_perm = 0;

		int
			max_clients = -1,
			needed_talkpower = 0;

		string name;

		CUtils::Get()->ParseField(res_row, "cid", cid);
		CUtils::Get()->ParseField(res_row, "pid", pid);
		CUtils::Get()->ParseField(res_row, "channel_order", order);
		CUtils::Get()->ParseField(res_row, "channel_name", name);
		CUtils::Get()->ParseField(res_row, "channel_flag_default", is_default);
		CUtils::Get()->ParseField(res_row, "channel_flag_password", has_password);
		CUtils::Get()->ParseField(res_row, "channel_flag_permanent", is_permanent);
		CUtils::Get()->ParseField(res_row, "channel_flag_semi_permanent", is_semi_perm);
		CUtils::Get()->ParseField(res_row, "channel_maxclients", max_clients);
		CUtils::Get()->ParseField(res_row, "channel_needed_talk_power", needed_talkpower);

		CUtils::Get()->UnEscapeString(name);


		Channel_t chan(new Channel);
		chan->ParentId = pid;
		chan->OrderId = order;
		chan->Name = name;
		chan->HasPassword = has_password != 0;
		if (is_permanent != 0)
			chan->Type = Channel::Types::PERMANENT;
		else if (is_semi_perm != 0)
			chan->Type = Channel::Types::SEMI_PERMANENT;
		else
			chan->Type = Channel::Types::TEMPORARY;
		chan->MaxClients = max_clients;
		chan->RequiredTalkPower = needed_talkpower;

		if (is_default != 0)
			m_DefaultChannel = cid;
		m_Channels.emplace(cid, chan);
	}
}

void CServer::OnClientList(vector<string> &res)
{
	boost::lock_guard<mutex> client_mtx_guard(m_ClientMtx);
	for (auto &r : res)
	{
		Client::Id_t
			id = Client::Invalid,
			dbid = Client::Invalid;
		Channel::Id_t cid = Channel::Invalid;
		string uid, ip;
		int type = 0;

		CUtils::Get()->ParseField(r, "clid", id);
		CUtils::Get()->ParseField(r, "cid", cid);
		CUtils::Get()->ParseField(r, "client_database_id", dbid);
		CUtils::Get()->ParseField(r, "client_unique_identifier", uid);
		CUtils::Get()->ParseField(r, "client_type", type);
		CUtils::Get()->ParseField(r, "connection_client_ip", ip);

		CUtils::Get()->UnEscapeString(uid);

		Client_t client(new Client);
		client->DatabaseId = dbid;
		client->Uid = uid;
		client->IpAddress = ip;
		client->CurrentChannel = cid;

		m_Clients.emplace(id, client);
	}
}



void CServer::OnChannelCreated(boost::smatch &result)
{
	unsigned int
		id = 0,
		parent_id = 0,
		order_id = 0;
	int
		maxclients = -1,
		needed_talkpower = 0;
	Channel::Types type = Channel::Types::INVALID;
	string name;

	CUtils::Get()->ConvertStringToInt(result[1].str(), id);
	CUtils::Get()->ConvertStringToInt(result[2].str(), parent_id);
	name = result[3].str();

	string extra_data(result[4].str());

	CUtils::Get()->ParseField(extra_data, "channel_order", order_id);
	CUtils::Get()->ParseField(extra_data, "channel_maxclients", maxclients);
	CUtils::Get()->ParseField(extra_data, "channel_needed_talk_power", needed_talkpower);

	int
		is_permanent = 0,
		is_semi_perm = 0;
	CUtils::Get()->ParseField(extra_data, "channel_flag_permanent", is_permanent);
	CUtils::Get()->ParseField(extra_data, "channel_flag_semi_permanent", is_semi_perm);
	if (is_permanent != 0)
		type = Channel::Types::PERMANENT;
	else if (is_semi_perm != 0)
		type = Channel::Types::SEMI_PERMANENT;
	else
		type = Channel::Types::TEMPORARY;


	CUtils::Get()->UnEscapeString(name);

	Channel_t chan(new Channel);
	chan->ParentId = parent_id;
	chan->OrderId = order_id;
	chan->Name = name;
	chan->Type = type;
	chan->HasPassword = (extra_data.find("channel_flag_password") != string::npos);
	chan->MaxClients = maxclients;
	chan->RequiredTalkPower = needed_talkpower;

	if (extra_data.find("channel_flag_default") != string::npos)
		m_DefaultChannel = id;

	boost::lock_guard<mutex> channel_mtx_guard(m_ChannelMtx);
	m_Channels.emplace(id, chan);


	CCallbackHandler::Get()->Call("TSC_OnChannelCreated", id);
}

void CServer::OnChannelDeleted(boost::smatch &result)
{
	unsigned int cid = 0;
	CUtils::Get()->ConvertStringToInt(result[1].str(), cid);

	if (IsValidChannel(cid) == false)
		return;


	boost::lock_guard<mutex> channel_mtx_guard(m_ChannelMtx);
	m_Channels.erase(cid);


	CCallbackHandler::Get()->Call("TSC_OnChannelDeleted", cid);
}

void CServer::OnChannelReorder(boost::smatch &result)
{
	unsigned int
		cid = 0,
		orderid = 0;

	CUtils::Get()->ConvertStringToInt(result[1].str(), cid);
	CUtils::Get()->ConvertStringToInt(result[2].str(), orderid);

	if (orderid != 0 && IsValidChannel(orderid) == false)
		return;

	if (IsValidChannel(cid) == false)
		return;
	

	boost::lock_guard<mutex> channel_mtx_guard(m_ChannelMtx);
	m_Channels.at(cid)->OrderId = orderid;


	CCallbackHandler::Get()->Call("TSC_OnChannelReorder", cid, orderid);
}

void CServer::OnChannelMoved(boost::smatch &result)
{
	unsigned int
		cid = 0,
		parentid = 0,
		orderid = 0;

	CUtils::Get()->ConvertStringToInt(result[1].str(), cid);
	CUtils::Get()->ConvertStringToInt(result[2].str(), parentid);
	CUtils::Get()->ConvertStringToInt(result[3].str(), orderid);

	if (orderid != 0 && IsValidChannel(orderid) == false)
		return;

	if (parentid != 0 && IsValidChannel(parentid) == false)
		return;

	if (IsValidChannel(cid) == false)
		return;
	

	boost::lock_guard<mutex> channel_mtx_guard(m_ChannelMtx);
	Channel_t &channel = m_Channels.at(cid);
	channel->ParentId = parentid;
	channel->OrderId = orderid;

	
	CCallbackHandler::Get()->Call("TSC_OnChannelMoved", cid, parentid, orderid);
}

void CServer::OnChannelRenamed(boost::smatch &result)
{
	unsigned int cid = 0;
	string name;

	CUtils::Get()->ConvertStringToInt(result[1].str(), cid);
	name = result[2].str();
	CUtils::Get()->UnEscapeString(name);

	if (IsValidChannel(cid) == false)
		return;


	boost::lock_guard<mutex> channel_mtx_guard(m_ChannelMtx);
	m_Channels.at(cid)->Name = name;

	
	CCallbackHandler::Get()->Call("TSC_OnChannelRenamed", cid, name);
}

void CServer::OnChannelPasswordToggled(boost::smatch &result)
{
	unsigned int
		cid = 0,
		toggle_password = 0;
	
	CUtils::Get()->ConvertStringToInt(result[1].str(), cid);
	CUtils::Get()->ConvertStringToInt(result[2].str(), toggle_password);

	if (IsValidChannel(cid) == false)
		return;


	boost::lock_guard<mutex> channel_mtx_guard(m_ChannelMtx);
	Channel_t &channel = m_Channels.at(cid);
	channel->HasPassword = (toggle_password != 0);
	channel->WasPasswordToggled = true;

	
	//forward TSC_OnChannelPasswordEdited(channelid, bool:ispassworded, bool:passwordchanged);
	CCallbackHandler::Get()->Call("TSC_OnChannelPasswordEdited", cid, toggle_password, 0);
}

void CServer::OnChannelPasswordChanged(boost::smatch &result)
{
	unsigned int cid = 0;
	CUtils::Get()->ConvertStringToInt(result[1].str(), cid);

	if (IsValidChannel(cid) == false)
		return;


	boost::lock_guard<mutex> channel_mtx_guard(m_ChannelMtx);
	Channel_t &channel = m_Channels.at(cid);
	if (channel->WasPasswordToggled == false)
	{
		//forward TSC_OnChannelPasswordEdited(channelid, bool:ispassworded, bool:passwordchanged);
		CCallbackHandler::Get()->Call("TSC_OnChannelPasswordEdited", cid, 1, 1);
	}
	else
		channel->WasPasswordToggled = false;
}

void CServer::OnChannelTypeChanged(boost::smatch &result)
{
	unsigned int cid = 0;
	string
		flag,
		sec_flag;
	unsigned int
		flag_data = 0,
		sec_flag_data = 0;
	
	CUtils::Get()->ConvertStringToInt(result[1].str(), cid);
	flag = result[2].str();
	CUtils::Get()->ConvertStringToInt(result[3].str(), flag_data);
	sec_flag = result[4].str();
	CUtils::Get()->ConvertStringToInt(result[5].str(), sec_flag_data);

	if (IsValidChannel(cid) == false)
		return;


	boost::lock_guard<mutex> channel_mtx_guard(m_ChannelMtx);
	Channel_t &channel = m_Channels.at(cid);
	if (flag_data != 0)
	{
		channel->Type = (flag.find("semi_") == 0)
			? Channel::Types::SEMI_PERMANENT
			: Channel::Types::PERMANENT;
	}
	else if (sec_flag_data != 0)
	{
		channel->Type = (sec_flag.find("semi_") == 0)
			? Channel::Types::SEMI_PERMANENT
			: Channel::Types::PERMANENT;
	}
	else
		channel->Type = Channel::Types::TEMPORARY;

	
	CCallbackHandler::Get()->Call("TSC_OnChannelTypeChanged", cid, static_cast<int>(channel->Type));
}

void CServer::OnChannelSetDefault(boost::smatch &result)
{
	unsigned int cid = 0;
	CUtils::Get()->ConvertStringToInt(result[1].str(), cid);

	if (IsValidChannel(cid) == false)
		return;


	boost::lock_guard<mutex> channel_mtx_guard(m_ChannelMtx);
	m_DefaultChannel = cid;

	
	CCallbackHandler::Get()->Call("TSC_OnChannelSetDefault", cid);
}

void CServer::OnChannelMaxClientsChanged(boost::smatch &result)
{
	unsigned int cid = 0;
	int maxclients = 0;

	CUtils::Get()->ConvertStringToInt(result[1].str(), cid);
	CUtils::Get()->ConvertStringToInt(result[2].str(), maxclients);

	if (IsValidChannel(cid) == false)
		return;


	boost::lock_guard<mutex> channel_mtx_guard(m_ChannelMtx);
	m_Channels.at(cid)->MaxClients = maxclients;

	
	CCallbackHandler::Get()->Call("TSC_OnChannelMaxClientsChanged", cid, maxclients);
}

void CServer::OnChannelRequiredTalkPowerChanged(boost::smatch &result)
{
	unsigned int cid = 0;
	int talkpower = 0;

	CUtils::Get()->ConvertStringToInt(result[1].str(), cid);
	CUtils::Get()->ConvertStringToInt(result[2].str(), talkpower);

	if (IsValidChannel(cid) == false)
		return;


	boost::lock_guard<mutex> channel_mtx_guard(m_ChannelMtx);
	m_Channels.at(cid)->RequiredTalkPower = talkpower;


	CCallbackHandler::Get()->Call("TSC_OnChannelRequiredTPChanged", cid, talkpower);
}



void CServer::OnClientConnect(boost::smatch &result)
{
	Client::Id_t
		clid = Client::Invalid,
		dbid = Client::Invalid;
	string
		uid,
		nickname;
	Channel::Id_t cid = Channel::Invalid;
	int type = 0;

	CUtils::Get()->ConvertStringToInt(result[1].str(), cid);
	CUtils::Get()->ConvertStringToInt(result[2].str(), clid);
	uid = result[3].str();
	nickname = result[4].str();
	CUtils::Get()->ConvertStringToInt(result[5].str(), dbid);
	CUtils::Get()->ConvertStringToInt(result[6].str(), type);

	CUtils::Get()->UnEscapeString(uid);
	CUtils::Get()->UnEscapeString(nickname);

	Client_t client(new Client);
	client->DatabaseId = dbid;
	client->Uid = uid;
	client->CurrentChannel = cid;



	CNetwork::Get()->Execute(
		fmt::format("clientinfo clid={}", clid), 
		[=](CNetwork::ResultSet_t &result)
		{
			string ip;
			CUtils::Get()->ParseField(result.at(0), "connection_client_ip", ip);
			client->IpAddress = ip;

			boost::lock_guard<mutex> client_mtx_guard(m_ClientMtx);
			m_Clients.emplace(clid, client);

			CCallbackHandler::Get()->Call("TSC_OnClientConnect", clid, nickname);
		}
	);
}

void CServer::OnClientDisconnect(boost::smatch &result)
{
	Client::Id_t clid = Client::Invalid;
	int reasonid;
	string reasonmsg;

	CUtils::Get()->ConvertStringToInt(result[1].str(), reasonid);
	reasonmsg = result[2].str();
	CUtils::Get()->ConvertStringToInt(result[3].str(), clid);

	if (IsValidClient(clid) == false)
		return;


	boost::lock_guard<mutex> client_mtx_guard(m_ClientMtx);
	m_Clients.erase(clid);


	CUtils::Get()->UnEscapeString(reasonmsg);
	CCallbackHandler::Get()->Call("TSC_OnClientDisconnect", clid, reasonid, reasonmsg);
}

void CServer::OnClientMoved(boost::smatch &result)
{
	Channel::Id_t to_cid;
	int reasonid;
	Client::Id_t invokerid = Client::Invalid;

	CUtils::Get()->ConvertStringToInt(result[1].str(), to_cid);
	CUtils::Get()->ConvertStringToInt(result[2].str(), reasonid);
	CUtils::Get()->ConvertStringToInt(result[3].str(), invokerid);
	string client_list(result[4].str());


	if (IsValidChannel(to_cid) == false)
		return;

	if (invokerid != Client::Invalid && IsValidClient(invokerid) == false)
		return;

	size_t
		delim_pos = 0,
		old_delim_pos = 0;
	do
	{
		delim_pos = client_list.find('|', delim_pos + 1);

		Client::Id_t clid = Client::Invalid;
		CUtils::Get()->ParseField(client_list.substr(old_delim_pos, delim_pos - old_delim_pos), "clid", clid);
		old_delim_pos = delim_pos;

		if (IsValidClient(clid))
		{
			boost::lock_guard<mutex> client_mtx_guard(m_ClientMtx);
			m_Clients.at(clid)->CurrentChannel = to_cid;

			CCallbackHandler::Get()->Call("TSC_OnClientMoved", clid, to_cid, invokerid);
		}
	} while (delim_pos != string::npos);

}

void CServer::OnClientServerText(boost::smatch &result)
{
	Client::Id_t clid = Client::Invalid;
	string
		nickname,
		msg;

	msg = result[1].str();
	CUtils::Get()->ConvertStringToInt(result[2].str(), clid);
	nickname = result[3].str();

	if (clid != Client::Invalid && IsValidClient(clid) == false)
		return;


	CUtils::Get()->UnEscapeString(nickname);
	CUtils::Get()->UnEscapeString(msg);
	CCallbackHandler::Get()->Call("TSC_OnClientServerText", clid, nickname, msg);
}

void CServer::OnClientPrivateText(boost::smatch &result)
{
	Client::Id_t
		from_clid = Client::Invalid,
		to_clid = Client::Invalid;
	string
		from_nickname,
		msg;

	msg = result[1].str();
	CUtils::Get()->ConvertStringToInt(result[2].str(), to_clid);
	CUtils::Get()->ConvertStringToInt(result[3].str(), from_clid);
	from_nickname = result[4].str();
	
	//one of both clid's has to be invalid because it's our ServerQuery client
	if (IsValidClient(from_clid) == false && IsValidClient(to_clid) == false)
		return;


	CUtils::Get()->UnEscapeString(from_nickname);
	CUtils::Get()->UnEscapeString(msg);
	CCallbackHandler::Get()->Call("TSC_OnClientPrivateText", from_clid, from_nickname, to_clid, msg);
}

