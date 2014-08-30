#include "main.h"
#include "natives.h"

#include "CNetwork.h"
#include "CServer.h"


//native TSC_Connect(user[], pass[], ip[], port = 9987, serverquery_port = 10011, bool:wait = true);
AMX_DECLARE_NATIVE(Native::TSC_Connect)
{
	char
		*login_tmp = NULL,
		*pass_tmp = NULL,
		*ip_tmp = NULL;

	amx_StrParam(amx, params[1], login_tmp);
	amx_StrParam(amx, params[2], pass_tmp);
	amx_StrParam(amx, params[3], ip_tmp);
	
	unsigned short
		server_port = static_cast<unsigned short>(params[4]),
		query_port = static_cast<unsigned short>(params[5]);

	bool wait = (params[6] != 0);


	if (login_tmp == NULL || pass_tmp == NULL 
		|| ip_tmp == NULL || server_port == 0 || query_port == 0)
		return 0;


	CNetwork::Get()->Connect(ip_tmp, server_port, query_port);
	bool ret_val = CServer::Get()->Login(login_tmp, pass_tmp);

	if (ret_val && wait)
	{
		while (CServer::Get()->IsLoggedIn() == false)
			boost::this_thread::sleep(boost::posix_time::milliseconds(100));
	}

	return ret_val;
}

//native TSC_Disconnect();
AMX_DECLARE_NATIVE(Native::TSC_Disconnect)
{
	CNetwork::Get()->Disconnect();
	return 1;
}

//native TSC_ChangeNickname(nickname[]);
AMX_DECLARE_NATIVE(Native::TSC_ChangeNickname)
{
	char *nick_tmp = NULL;
	amx_StrParam(amx, params[1], nick_tmp);

	if (nick_tmp == NULL)
		return 0;

	
	return CServer::Get()->ChangeNickname(nick_tmp);
}

//native TSC_SendServerMessage(msg[]);
AMX_DECLARE_NATIVE(Native::TSC_SendServerMessage)
{
	char *msg_tmp = NULL;
	amx_StrParam(amx, params[1], msg_tmp);

	if (msg_tmp == NULL)
		return 0;


	return CServer::Get()->SendServerMessage(msg_tmp);
}



//native TSC_CreateChannel(channelname[]);
AMX_DECLARE_NATIVE(Native::TSC_CreateChannel)
{
	char *channelname_tmp = NULL;
	amx_StrParam(amx, params[1], channelname_tmp);

	if (channelname_tmp == NULL)
		return 0;


	CServer::Get()->CreateChannel(channelname_tmp);
	return 1;
}

//native TSC_DeleteChannel(channelid);
AMX_DECLARE_NATIVE(Native::TSC_DeleteChannel)
{
	return CServer::Get()->DeleteChannel(static_cast<Channel::Id_t>(params[1]));
}

//native TSC_GetChannelIdByName(channelname[]);
AMX_DECLARE_NATIVE(Native::TSC_GetChannelIdByName)
{
	char *name = NULL;
	amx_StrParam(amx, params[1], name);
	return CServer::Get()->GetChannelIdByName(name == NULL ? string() : name);
}

//native TSC_IsValidChannel(channelid);
AMX_DECLARE_NATIVE(Native::TSC_IsValidChannel)
{
	return CServer::Get()->IsValidChannel(static_cast<Channel::Id_t>(params[1]));
}

//native TSC_SetChannelName(channelid, channelname[]);
AMX_DECLARE_NATIVE(Native::TSC_SetChannelName)
{
	char *channelname_tmp = NULL;
	amx_StrParam(amx, params[2], channelname_tmp);

	if (channelname_tmp == NULL)
		return 0;


	return CServer::Get()->SetChannelName(
		static_cast<Channel::Id_t>(params[1]), channelname_tmp);
}

//native TSC_GetChannelName(channelid, dest[], maxlen = sizeof(dest));
AMX_DECLARE_NATIVE(Native::TSC_GetChannelName)
{
	amx_SetCppString(amx, params[2],
		CServer::Get()->GetChannelName(static_cast<Channel::Id_t>(params[1])),
		static_cast<size_t>(params[3]));
	return 1;
}

//native TSC_SetChannelDescription(channelid, desc[]);
AMX_DECLARE_NATIVE(Native::TSC_SetChannelDescription)
{
	char *channeldesc_tmp = NULL;
	amx_StrParam(amx, params[2], channeldesc_tmp);

	return CServer::Get()->SetChannelDescription(static_cast<Channel::Id_t>(params[1]), 
		channeldesc_tmp == NULL ? string() : channeldesc_tmp);
}

//native TSC_SetChannelType(channelid, type);
AMX_DECLARE_NATIVE(Native::TSC_SetChannelType)
{
	return CServer::Get()->SetChannelType(
		static_cast<Channel::Id_t>(params[1]), static_cast<Channel::Types>(params[2]));
}

//native TSC_GetChannelType(channelid);
AMX_DECLARE_NATIVE(Native::TSC_GetChannelType)
{
	return static_cast<cell>(
		CServer::Get()->GetChannelType(static_cast<Channel::Id_t>(params[1])));
}

//native TSC_SetChannelPassword(channelid, password[]);
AMX_DECLARE_NATIVE(Native::TSC_SetChannelPassword)
{
	char *pwd_tmp = NULL;
	amx_StrParam(amx, params[2], pwd_tmp);

	return CServer::Get()->SetChannelPassword(
		static_cast<Channel::Id_t>(params[1]), pwd_tmp == NULL ? string() : pwd_tmp);
}

//native TSC_HasChannelPassword(channelid);
AMX_DECLARE_NATIVE(Native::TSC_HasChannelPassword)
{
	return CServer::Get()->HasChannelPassword(static_cast<Channel::Id_t>(params[1]));
}

//native TSC_SetChannelRequiredTP(channelid, talkpower);
AMX_DECLARE_NATIVE(Native::TSC_SetChannelRequiredTP)
{
	return CServer::Get()->SetChannelRequiredTalkPower(
		static_cast<Channel::Id_t>(params[1]), params[2]);
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
		static_cast<Channel::Id_t>(params[1]), params[2]);
}

//native TSC_GetChannelUserLimit(channelid);
AMX_DECLARE_NATIVE(Native::TSC_GetChannelUserLimit)
{
	return CServer::Get()->GetChannelUserLimit(static_cast<Channel::Id_t>(params[1]));
}

//native TSC_SetChannelParentId(channelid, parentchannelid);
AMX_DECLARE_NATIVE(Native::TSC_SetChannelParentId)
{
	return CServer::Get()->SetChannelParentId(
		static_cast<Channel::Id_t>(params[1]), static_cast<Channel::Id_t>(params[2]));
}

//native TSC_GetChannelParentId(channelid);
AMX_DECLARE_NATIVE(Native::TSC_GetChannelParentId)
{
	return CServer::Get()->GetChannelParentId(static_cast<Channel::Id_t>(params[1]));
}

//native TSC_SetChannelOrderId(channelid, upperchannelid);
AMX_DECLARE_NATIVE(Native::TSC_SetChannelOrderId)
{
	return CServer::Get()->SetChannelOrderId(
		static_cast<Channel::Id_t>(params[1]), static_cast<Channel::Id_t>(params[2]));
}

//native TSC_GetChannelOrderId(channelid);
AMX_DECLARE_NATIVE(Native::TSC_GetChannelOrderId)
{
	return CServer::Get()->GetChannelOrderId(static_cast<Channel::Id_t>(params[1]));
}

//native TSC_GetDefaultChannelId();
AMX_DECLARE_NATIVE(Native::TSC_GetDefaultChannelId)
{
	return CServer::Get()->GetDefaultChannelId();
}



//native TSC_GetClientIdByUid(uid[]);
AMX_DECLARE_NATIVE(Native::TSC_GetClientIdByUid)
{
	char *tmp_uid = NULL;
	amx_StrParam(amx, params[1], tmp_uid);
	return CServer::Get()->GetClientIdByUid(tmp_uid == NULL ? string() : tmp_uid);
}


//native TSC_GetClientUid(clientid, dest[], sizeof(dest[]));
AMX_DECLARE_NATIVE(Native::TSC_GetClientUid)
{
	string uid = CServer::Get()->GetClientUid(static_cast<Client::Id_t>(params[1]));
	amx_SetCppString(amx, params[2], uid, params[3]);
	return (uid.empty() == false);
}

//native TSC_GetClientDatabaseId(clientid);
AMX_DECLARE_NATIVE(Native::TSC_GetClientDatabaseId)
{
	return CServer::Get()->GetClientDatabaseId(static_cast<Client::Id_t>(params[1]));
}

//native TSC_GetClientChannelId(clientid);
AMX_DECLARE_NATIVE(Native::TSC_GetClientChannelId)
{
	return CServer::Get()->GetClientChannelId(static_cast<Client::Id_t>(params[1]));
}

//native TSC_GetClientIpAddress(clientid, dest[], sizeof(dest[]));
AMX_DECLARE_NATIVE(Native::TSC_GetClientIpAddress)
{
	string ip = CServer::Get()->GetClientIpAddress(static_cast<Client::Id_t>(params[1]));
	amx_SetCppString(amx, params[2], ip, params[3]);
	return (ip.empty() == false);
}


//native TSC_KickClient(clientid, TSC_KICKTYPE:kicktype, reasonmsg[]);
AMX_DECLARE_NATIVE(Native::TSC_KickClient)
{
	char *tmp_msg = NULL;
	amx_StrParam(amx, params[3], tmp_msg);
	return CServer::Get()->KickClient(static_cast<Client::Id_t>(params[1]),
		static_cast<Client::KickTypes>(params[2]), tmp_msg != NULL ? tmp_msg : string());
}

//native TSC_BanClient(uid[], seconds, reasonmsg[]);
AMX_DECLARE_NATIVE(Native::TSC_BanClient)
{
	char 
		*tmp_uid = NULL,
		*tmp_msg = NULL;
	amx_StrParam(amx, params[1], tmp_msg);
	amx_StrParam(amx, params[3], tmp_msg);

	if (tmp_uid == NULL)
		return 0;


	return CServer::Get()->BanClient(tmp_uid, params[2], 
		tmp_msg != NULL ? tmp_msg : string());
}

//native TSC_MoveClient(clientid, channelid);
AMX_DECLARE_NATIVE(Native::TSC_MoveClient)
{
	return CServer::Get()->MoveClient(static_cast<Client::Id_t>(params[1]),
		static_cast<Channel::Id_t>(params[2]));
}


//native TSC_SetClientChannelGroup(clientid, groupid, channelid);
AMX_DECLARE_NATIVE(Native::TSC_SetClientChannelGroup)
{
	return CServer::Get()->SetClientChannelGroup(static_cast<Client::Id_t>(params[1]),
		params[2], static_cast<Channel::Id_t>(params[3]));
}

//native TSC_AddClientToServerGroup(clientid, groupid);
AMX_DECLARE_NATIVE(Native::TSC_AddClientToServerGroup)
{
	return CServer::Get()->AddClientToServerGroup(
		static_cast<Client::Id_t>(params[1]), params[2]);
}

//native TSC_RemoveClientFromServerGroup(clientid, groupid);
AMX_DECLARE_NATIVE(Native::TSC_RemoveClientFromServerGroup)
{
	return CServer::Get()->RemoveClientFromServerGroup(
		static_cast<Client::Id_t>(params[1]), params[2]);
}


//native TSC_PokeClient(clientid, msg[]);
AMX_DECLARE_NATIVE(Native::TSC_PokeClient)
{
	char *tmp_msg = NULL;
	amx_StrParam(amx, params[2], tmp_msg);
	return CServer::Get()->PokeClient(static_cast<Client::Id_t>(params[1]), 
		tmp_msg != NULL ? tmp_msg : string());
}

//native TSC_SendClientMessage(clientid, msg[]);
AMX_DECLARE_NATIVE(Native::TSC_SendClientMessage)
{
	char *tmp_msg = NULL;
	amx_StrParam(amx, params[2], tmp_msg);
	return CServer::Get()->SendClientMessage(static_cast<Client::Id_t>(params[1]),
		tmp_msg != NULL ? tmp_msg : string());
}
