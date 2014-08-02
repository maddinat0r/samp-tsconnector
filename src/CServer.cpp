#include "CServer.h"
#include "CNetwork.h"
#include "CUtils.h"
#include "CCallback.h"

#include "main.h"
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
	

	CNetwork::Get()->Execute("servernotifyregister event=server");
	CNetwork::Get()->Execute("servernotifyregister event=channel id=0");


	//fill up cache
	CNetwork::Get()->Execute("channellist -flags -limit -voice",
		boost::bind(&CServer::OnChannelList, this, _1));

	//retrieve vserver-id
	CNetwork::Get()->Execute(fmt::format("serveridgetbyport virtualserver_port={}", CNetwork::Get()->GetServerPort()),
		[this](CNetwork::ResultSet_t &result)
		{
			if (result.empty() == false)
				CUtils::Get()->ParseField(result.at(0), "server_id", m_ServerId);
		});
}

CServer::~CServer()
{
	for (auto &c : m_Channels)
		delete c.second;

	for (auto &c : m_Clients)
		delete c.second;
}



bool CServer::Login(string login, string pass)
{
	CUtils::Get()->EscapeString(login);
	CUtils::Get()->EscapeString(pass);
	
	CNetwork::Get()->Execute(fmt::format("login client_login_name={} client_login_password={}", login, pass),
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
	CNetwork::Get()->Execute(fmt::format("sendtextmessage targetmode=3 target={} msg={}", m_ServerId, msg));
	return true;
}



bool CServer::CreateChannel(string name)
{
	if (m_IsLoggedIn == false)
		return false;

	if (name.empty())
		return false;


	CUtils::Get()->EscapeString(name);
	CNetwork::Get()->Execute(fmt::format("channelcreate channel_name={}", name));
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
			delete m_Channels.at(cid);
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
			m_Channels.at(cid)->Name = unescaped_name;
		});
	return true;
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

	switch (m_Channels.at(cid)->Type)
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
			m_Channels.at(cid)->Type = type;
		});
	return true;
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
			m_Channels.at(cid)->HasPassword = (password_empty == false);
		});
	return true;
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
			m_Channels.at(cid)->RequiredTalkPower = talkpower;
		});
	return true;
}

bool CServer::SetChannelUserLimit(Channel::Id_t cid, int maxusers)
{
	if (m_IsLoggedIn == false)
		return false;

	if (IsValidChannel(cid) == false)
		return false;

	if (maxusers < -1)
		return false;


	CNetwork::Get()->Execute(fmt::format("channeledit cid={} channel_maxclients={}", cid, maxusers),
		[this, cid, maxusers](CNetwork::ResultSet_t &result)
		{
			m_Channels.at(cid)->MaxClients = maxusers;
		});
	return true;
}

bool CServer::SetChannelParentId(Channel::Id_t cid, Channel::Id_t pcid)
{
	if (m_IsLoggedIn == false)
		return false;

	if (IsValidChannel(cid) == false)
		return false;

	if (pcid != 0 && IsValidChannel(pcid) == false)
		return false;

	if (m_Channels.at(cid)->ParentId == pcid)
		return false;


	CNetwork::Get()->Execute(fmt::format("channelmove cid={} cpid={}", cid, pcid),
		[this, cid, pcid](CNetwork::ResultSet_t &result)
		{
			m_Channels.at(cid)->ParentId = pcid;
		});
	return true;
}

bool CServer::SetChannelOrderId(Channel::Id_t cid, Channel::Id_t ocid)
{
	if (m_IsLoggedIn == false)
		return false;

	if (IsValidChannel(cid) == false)
		return false;

	if (ocid != 0 && IsValidChannel(ocid) == false)
		return false;

	if (m_Channels.at(cid)->OrderId == ocid)
		return false;


	CNetwork::Get()->Execute(fmt::format("channeledit cid={} channel_order={}", cid, ocid),
		[this, cid, ocid](CNetwork::ResultSet_t &result)
		{
			m_Channels.at(cid)->OrderId = ocid;
		});
	return true;
}

Channel::Id_t CServer::FindChannel(string name)
{
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


		Channel *chan = new Channel;
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

	Channel *chan = new Channel;
	chan->ParentId = parent_id;
	chan->OrderId = order_id;
	chan->Name = name;
	chan->Type = type;
	chan->HasPassword = (extra_data.find("channel_flag_password") != string::npos);
	chan->MaxClients = maxclients;
	chan->RequiredTalkPower = needed_talkpower;

	if (extra_data.find("channel_flag_default") != string::npos)
		m_DefaultChannel = id;
	m_Channels.emplace(id, chan);


	CCallbackHandler::Get()->Call("TSC_OnChannelCreated", id);
}

void CServer::OnChannelDeleted(boost::smatch &result)
{
	unsigned int cid = 0;
	CUtils::Get()->ConvertStringToInt(result[1].str(), cid);

	if (IsValidChannel(cid) == false)
		return;


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
	

	Channel *channel = m_Channels.at(cid);
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


	Channel *channel = m_Channels.at(cid);
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


	Channel *channel = m_Channels.at(cid);
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


	Channel *channel = m_Channels.at(cid);
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


	m_Channels.at(cid)->RequiredTalkPower = talkpower;


	CCallbackHandler::Get()->Call("TSC_OnChannelRequiredTPChanged", cid, talkpower);
}
