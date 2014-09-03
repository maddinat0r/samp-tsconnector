#include "main.h"
#include "natives.h"

#include "CNetwork.h"
#include "CServer.h"
#include "CCallback.h"


//native TSC_Connect(user[], pass[], host[], port = 9987, serverquery_port = 10011);
AMX_DECLARE_NATIVE(Native::TSC_Connect)
{
	string
		login = amx_GetCppString(amx, params[1]),
		pass = amx_GetCppString(amx, params[2]),
		host = amx_GetCppString(amx, params[3]);
	
	unsigned short
		server_port = static_cast<unsigned short>(params[4]),
		query_port = static_cast<unsigned short>(params[5]);


	if (login.empty() || pass.empty() || host.empty()
		|| server_port == 0 || query_port == 0)
		return 0;


	if (CNetwork::Get()->Connect(host, server_port, query_port))
	{
		while (CNetwork::Get()->IsConnected() == false)
			boost::this_thread::sleep(boost::posix_time::milliseconds(20));

		if (CServer::Get()->Login(login, pass))
		{
			while (CServer::Get()->IsLoggedIn() == false)
				boost::this_thread::sleep(boost::posix_time::milliseconds(20));
			return 1;
		}
	}
	return 0;
}

//native TSC_Disconnect();
AMX_DECLARE_NATIVE(Native::TSC_Disconnect)
{
	return CNetwork::Get()->Disconnect();
}

//native TSC_ChangeNickname(nickname[]);
AMX_DECLARE_NATIVE(Native::TSC_ChangeNickname)
{
	return CServer::Get()->ChangeNickname(
		amx_GetCppString(amx, params[1]));
}

//native TSC_SendServerMessage(msg[]);
AMX_DECLARE_NATIVE(Native::TSC_SendServerMessage)
{
	return CServer::Get()->SendServerMessage(
		amx_GetCppString(amx, params[1]));
}



//native TSC_QueryChannelData(channelid, TSC_CHANNEL_QUERYDATA:data, const callback[], const format[] = "", ...);
AMX_DECLARE_NATIVE(Native::TSC_QueryChannelData)
{
	auto *callback = CCallbackHandler::Get()->Create(
		amx_GetCppString(amx, params[3]),
		amx_GetCppString(amx, params[4]),
		amx,
		params,
		5);

	if (callback == nullptr)
		return 0;

	return CServer::Get()->QueryChannelData(
		static_cast<Channel::Id_t>(params[1]),
		static_cast<Channel::QueryData>(params[2]), 
		callback);
}

//native TSC_QueryClientData(clientid, TSC_CLIENT_QUERYDATA:data, const callback[], const format[] = "", ...);
AMX_DECLARE_NATIVE(Native::TSC_QueryClientData)
{
	auto *callback = CCallbackHandler::Get()->Create(
		amx_GetCppString(amx, params[3]),
		amx_GetCppString(amx, params[4]),
		amx,
		params,
		5);

	if (callback == nullptr)
		return 0;

	return CServer::Get()->QueryClientData(
		static_cast<Client::Id_t>(params[1]),
		static_cast<Client::QueryData>(params[2]),
		callback);
}

//native TSC_GetQueriedData(dest[], max_len = sizeof(dest));
AMX_DECLARE_NATIVE(Native::TSC_GetQueriedData)
{
	string dest;
	bool ret_val = CServer::Get()->GetQueriedData(dest);
	amx_SetCppString(amx, params[1], dest, params[2]);
	return ret_val;
}

//native TSC_GetQueriedDataAsInt();
AMX_DECLARE_NATIVE(Native::TSC_GetQueriedDataAsInt)
{
	int dest = 0;
	CServer::Get()->GetQueriedData(dest);
	return dest;
}



//native TSC_CreateChannel(channelname[], TSC_CHANNELTYPE:type = TEMPORARY, maxusers = -1, parentchannelid = -1, upperchannelid = -1, talkpower = 0);
AMX_DECLARE_NATIVE(Native::TSC_CreateChannel)
{
	return CServer::Get()->CreateChannel(
		amx_GetCppString(amx, params[1]),
		static_cast<Channel::Types>(params[2]),
		params[3],
		static_cast<Channel::Id_t>(params[4]),
		static_cast<Channel::Id_t>(params[5]),
		params[6]);
}

//native TSC_DeleteChannel(channelid);
AMX_DECLARE_NATIVE(Native::TSC_DeleteChannel)
{
	return CServer::Get()->DeleteChannel(
		static_cast<Channel::Id_t>(params[1]));
}

//native TSC_GetChannelIdByName(channelname[]);
AMX_DECLARE_NATIVE(Native::TSC_GetChannelIdByName)
{
	return CServer::Get()->GetChannelIdByName(
		amx_GetCppString(amx, params[1]));
}

//native TSC_IsValidChannel(channelid);
AMX_DECLARE_NATIVE(Native::TSC_IsValidChannel)
{
	return CServer::Get()->IsValidChannel(
		static_cast<Channel::Id_t>(params[1]));
}

//native TSC_SetChannelName(channelid, channelname[]);
AMX_DECLARE_NATIVE(Native::TSC_SetChannelName)
{
	return CServer::Get()->SetChannelName(
		static_cast<Channel::Id_t>(params[1]), 
		amx_GetCppString(amx, params[2]));
}

//native TSC_GetChannelName(channelid, dest[], maxlen = sizeof(dest));
AMX_DECLARE_NATIVE(Native::TSC_GetChannelName)
{
	string channel_name = CServer::Get()->GetChannelName(static_cast<Channel::Id_t>(params[1]));
	amx_SetCppString(amx, params[2], channel_name, params[3]);
	return (channel_name.empty() == false);
}

//native TSC_SetChannelDescription(channelid, desc[]);
AMX_DECLARE_NATIVE(Native::TSC_SetChannelDescription)
{
	return CServer::Get()->SetChannelDescription(
		static_cast<Channel::Id_t>(params[1]), 
		amx_GetCppString(amx, params[2]));
}

//native TSC_SetChannelType(channelid, type);
AMX_DECLARE_NATIVE(Native::TSC_SetChannelType)
{
	return CServer::Get()->SetChannelType(
		static_cast<Channel::Id_t>(params[1]), 
		static_cast<Channel::Types>(params[2]));
}

//native TSC_GetChannelType(channelid);
AMX_DECLARE_NATIVE(Native::TSC_GetChannelType)
{
	return static_cast<cell>(CServer::Get()->GetChannelType(
		static_cast<Channel::Id_t>(params[1])));
}

//native TSC_SetChannelPassword(channelid, password[]);
AMX_DECLARE_NATIVE(Native::TSC_SetChannelPassword)
{
	return CServer::Get()->SetChannelPassword(
		static_cast<Channel::Id_t>(params[1]),
		amx_GetCppString(amx, params[2]));
}

//native TSC_HasChannelPassword(channelid);
AMX_DECLARE_NATIVE(Native::TSC_HasChannelPassword)
{
	return CServer::Get()->HasChannelPassword(
		static_cast<Channel::Id_t>(params[1]));
}

//native TSC_SetChannelRequiredTP(channelid, talkpower);
AMX_DECLARE_NATIVE(Native::TSC_SetChannelRequiredTP)
{
	return CServer::Get()->SetChannelRequiredTalkPower(
		static_cast<Channel::Id_t>(params[1]), 
		params[2]);
}

//native TSC_GetChannelRequiredTP(channelid);
AMX_DECLARE_NATIVE(Native::TSC_GetChannelRequiredTP)
{
	return CServer::Get()->GetChannelRequiredTalkPower(
		static_cast<Channel::Id_t>(params[1]));
}

//native TSC_SetChannelUserLimit(channelid, maxusers);
AMX_DECLARE_NATIVE(Native::TSC_SetChannelUserLimit)
{
	return CServer::Get()->SetChannelUserLimit(
		static_cast<Channel::Id_t>(params[1]), 
		params[2]);
}

//native TSC_GetChannelUserLimit(channelid);
AMX_DECLARE_NATIVE(Native::TSC_GetChannelUserLimit)
{
	return CServer::Get()->GetChannelUserLimit(
		static_cast<Channel::Id_t>(params[1]));
}

//native TSC_SetChannelParentId(channelid, parentchannelid);
AMX_DECLARE_NATIVE(Native::TSC_SetChannelParentId)
{
	return CServer::Get()->SetChannelParentId(
		static_cast<Channel::Id_t>(params[1]), 
		static_cast<Channel::Id_t>(params[2]));
}

//native TSC_GetChannelParentId(channelid);
AMX_DECLARE_NATIVE(Native::TSC_GetChannelParentId)
{
	return CServer::Get()->GetChannelParentId(
		static_cast<Channel::Id_t>(params[1]));
}

//native TSC_SetChannelOrderId(channelid, upperchannelid);
AMX_DECLARE_NATIVE(Native::TSC_SetChannelOrderId)
{
	return CServer::Get()->SetChannelOrderId(
		static_cast<Channel::Id_t>(params[1]), 
		static_cast<Channel::Id_t>(params[2]));
}

//native TSC_GetChannelOrderId(channelid);
AMX_DECLARE_NATIVE(Native::TSC_GetChannelOrderId)
{
	return CServer::Get()->GetChannelOrderId(
		static_cast<Channel::Id_t>(params[1]));
}

//native TSC_GetDefaultChannelId();
AMX_DECLARE_NATIVE(Native::TSC_GetDefaultChannelId)
{
	return CServer::Get()->GetDefaultChannelId();
}



//native TSC_GetClientIdByUid(uid[]);
AMX_DECLARE_NATIVE(Native::TSC_GetClientIdByUid)
{
	return CServer::Get()->GetClientIdByUid(
		amx_GetCppString(amx, params[1]));
}

//native TSC_GetClientIdByIpAddress(ip[]);
AMX_DECLARE_NATIVE(Native::TSC_GetClientIdByIpAddress)
{
	return CServer::Get()->GetClientIdByUid(
		amx_GetCppString(amx, params[1]));
}


//native TSC_GetClientUid(clientid, dest[], maxlen = sizeof(dest));
AMX_DECLARE_NATIVE(Native::TSC_GetClientUid)
{
	string uid = CServer::Get()->GetClientUid(static_cast<Client::Id_t>(params[1]));
	amx_SetCppString(amx, params[2], uid, params[3]);
	return (uid.empty() == false);
}

//native TSC_GetClientDatabaseId(clientid);
AMX_DECLARE_NATIVE(Native::TSC_GetClientDatabaseId)
{
	return CServer::Get()->GetClientDatabaseId(
		static_cast<Client::Id_t>(params[1]));
}

//native TSC_GetClientChannelId(clientid);
AMX_DECLARE_NATIVE(Native::TSC_GetClientChannelId)
{
	return CServer::Get()->GetClientChannelId(
		static_cast<Client::Id_t>(params[1]));
}

//native TSC_GetClientIpAddress(clientid, dest[], maxlen = sizeof(dest));
AMX_DECLARE_NATIVE(Native::TSC_GetClientIpAddress)
{
	string ip = CServer::Get()->GetClientIpAddress(static_cast<Client::Id_t>(params[1]));
	amx_SetCppString(amx, params[2], ip, params[3]);
	return (ip.empty() == false);
}


//native TSC_KickClient(clientid, TSC_KICKTYPE:kicktype, reasonmsg[]);
AMX_DECLARE_NATIVE(Native::TSC_KickClient)
{
	return CServer::Get()->KickClient(
		static_cast<Client::Id_t>(params[1]),
		static_cast<Client::KickTypes>(params[2]), 
		amx_GetCppString(amx, params[3]));
}

//native TSC_BanClient(uid[], seconds, reasonmsg[]);
AMX_DECLARE_NATIVE(Native::TSC_BanClient)
{
	return CServer::Get()->BanClient(
		amx_GetCppString(amx, params[1]), 
		params[2],
		amx_GetCppString(amx, params[3]));
}

//native TSC_MoveClient(clientid, channelid);
AMX_DECLARE_NATIVE(Native::TSC_MoveClient)
{
	return CServer::Get()->MoveClient(
		static_cast<Client::Id_t>(params[1]),
		static_cast<Channel::Id_t>(params[2]));
}


//native TSC_SetClientChannelGroup(clientid, groupid, channelid);
AMX_DECLARE_NATIVE(Native::TSC_SetClientChannelGroup)
{
	return CServer::Get()->SetClientChannelGroup(
		static_cast<Client::Id_t>(params[1]),
		params[2], 
		static_cast<Channel::Id_t>(params[3]));
}

//native TSC_AddClientToServerGroup(clientid, groupid);
AMX_DECLARE_NATIVE(Native::TSC_AddClientToServerGroup)
{
	return CServer::Get()->AddClientToServerGroup(
		static_cast<Client::Id_t>(params[1]), 
		params[2]);
}

//native TSC_RemoveClientFromServerGroup(clientid, groupid);
AMX_DECLARE_NATIVE(Native::TSC_RemoveClientFromServerGroup)
{
	return CServer::Get()->RemoveClientFromServerGroup(
		static_cast<Client::Id_t>(params[1]), 
		params[2]);
}


//native TSC_PokeClient(clientid, msg[]);
AMX_DECLARE_NATIVE(Native::TSC_PokeClient)
{
	return CServer::Get()->PokeClient(
		static_cast<Client::Id_t>(params[1]),
		amx_GetCppString(amx, params[2]));
}

//native TSC_SendClientMessage(clientid, msg[]);
AMX_DECLARE_NATIVE(Native::TSC_SendClientMessage)
{
	return CServer::Get()->SendClientMessage(
		static_cast<Client::Id_t>(params[1]),
		amx_GetCppString(amx, params[2]));
}
