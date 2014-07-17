#include "CServer.h"
#include "CNetwork.h"
#include "CUtils.h"

#include "main.h"


void CServer::Initialize()
{
	//register notify events
	CNetwork::Get()->RegisterEvent(
		boost::regex("notifychannelcreated cid=([0-9]+) cpid=([0-9]+) channel_name=([^ ]+) .* channel_order=([0-9]+)(?: channel_flag_([^=]+)=1)?(?: channel_flag_([^=]+)=1)? .*invokerid=([0-9]+) invokername=([^ ]+) invokeruid=([^ \n\r]+)"),
		boost::bind(&CServer::OnChannelCreated, this, _1));


	CNetwork::Get()->Execute("servernotifyregister event=server");
	CNetwork::Get()->Execute("servernotifyregister event=channel id=0");


	//fill up cache
	CNetwork::Get()->Execute(str(fmt::Writer() << "channellist -flags -limit"),
		boost::bind(&CServer::OnChannelList, this, _1));
}

void CServer::Login(string login, string pass, string nickname)
{
	CUtils::Get()->EscapeString(login);
	CUtils::Get()->EscapeString(pass);
	CUtils::Get()->EscapeString(nickname);
	
	CNetwork::Get()->Execute(str(fmt::Format("login client_login_name={} client_login_password={}", login, pass)));
	CNetwork::Get()->Execute(str(fmt::Format("clientupdate client_nickname={}", nickname)),
		boost::bind(&CServer::OnLogin, this, _1));
}

void CServer::CreateChannel(string name)
{
	CUtils::Get()->EscapeString(name);

	CNetwork::Get()->Execute(str(fmt::Writer() << "channelcreate channel_name=" << name));
}



void CServer::OnLogin(vector<string> &res)
{
	Initialize();
}

void CServer::OnChannelList(vector<string> &res)
{
	for (vector<string>::iterator i = res.begin(), end = res.end(); i != end; ++i)
	{
		unsigned int
			cid = 0,
			pid = 0,
			order = 0,
			is_default = 0,
			has_password = 0,
			is_permanent = 0,
			is_semi_perm = 0;
			//total_clients = 0;

		int max_clients = -1;

		string name;

		CUtils::Get()->ParseField(*i, "cid", cid);
		CUtils::Get()->ParseField(*i, "pid", pid);
		CUtils::Get()->ParseField(*i, "channel_order", order);
		CUtils::Get()->ParseField(*i, "channel_name", name);
		CUtils::Get()->ParseField(*i, "channel_flag_default", is_default);
		CUtils::Get()->ParseField(*i, "channel_flag_password", has_password);
		CUtils::Get()->ParseField(*i, "channel_flag_permanent", is_permanent);
		CUtils::Get()->ParseField(*i, "channel_flag_semi_permanent", is_semi_perm);
		//CUtils::Get()->ParseField(*i, "total_clients", total_clients);
		CUtils::Get()->ParseField(*i, "channel_maxclients", max_clients);

		CUtils::Get()->UnEscapeString(name);


		Channel *chan = new Channel;
		chan->Id = cid;
		chan->ParentId = pid;
		chan->OrderId = order;
		chan->Name = name;
		chan->HasPassword = has_password != 0;
		if (is_permanent != 0)
			chan->Type = CHANNEL_TYPE_PERMANENT;
		else if (is_semi_perm != 0)
			chan->Type = CHANNEL_TYPE_SEMI_PERMANENT;
		else
			chan->Type = CHANNEL_TYPE_TEMPORARY;
		chan->MaxClients = max_clients;

		if (is_default != 0)
			m_DefaultChannel = cid;
		m_Channels.insert(unordered_map<unsigned int, Channel *>::value_type(cid, chan));
	}
}



void CServer::OnChannelCreated(boost::smatch &result)
{
	unsigned int
		id = 0,
		parent_id = 0,
		order_id = 0;
	unsigned int type = CHANNEL_TYPE_INVALID;
	string name;

	//unsigned int creator_id = 0;
	//string
	//	creator_name,
	//	creator_uid;

	CUtils::Get()->ConvertStringToInt(result[1].str(), id);
	CUtils::Get()->ConvertStringToInt(result[2].str(), parent_id);
	name = result[3].str();
	CUtils::Get()->ConvertStringToInt(result[4].str(), order_id);
	string type_flag_str(result[5].str());
	if (type_flag_str.find("channel_flag_permanent") != string::npos)
		type = CHANNEL_TYPE_PERMANENT;
	else if (type_flag_str.find("channel_flag_semi_permanent") != string::npos)
		type = CHANNEL_TYPE_SEMI_PERMANENT;
	else
		type = CHANNEL_TYPE_TEMPORARY;
	string extra_flag_str(result[6].str());
	//CUtils::Get()->ConvertStringToInt(result[7].str(), creator_id);
	//creator_name = result[8].str();
	//creator_uid = result[9].str();


	Channel *chan = new Channel;
	chan->Id = id;
	chan->ParentId = parent_id;
	chan->OrderId = order_id;
	chan->Name = name;
	chan->Type = type;
	chan->HasPassword = (extra_flag_str.find("password") != string::npos);

	if (extra_flag_str.find("default") != string::npos)
		m_DefaultChannel = id;
	m_Channels.insert(unordered_map<unsigned int, Channel *>::value_type(id, chan));
}
