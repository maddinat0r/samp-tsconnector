#include "main.h"
#include "natives.h"

#include "format.h"

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
	return CServer::Get()->SetChannelType(static_cast<Channel::Id_t>(params[1]), params[2]);
}

//native TSC_GetChannelType(channelid);
AMX_DECLARE_NATIVE(Native::TSC_GetChannelType)
{
	return CServer::Get()->GetChannelType(static_cast<Channel::Id_t>(params[1]));
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

////native TSC_SetChannelTalkPower(channelname[], talkpower);
//AMX_DECLARE_NATIVE(Native::TSC_SetChannelTalkPower)
//{
//	char *channelname_tmp = NULL;
//	int talkpower = static_cast<int>(params[2]);
//
//	amx_StrParam(amx, params[1], channelname_tmp);
//
//	if (channelname_tmp == NULL)
//		return 0;
//
//	string channelname(channelname_tmp);
//	CNetwork::Get()->EscapeString(channelname);
//
//
//	CommandList *cmd_list = new CommandList;
//
//	string chfind_cmd;
//	karma::generate(std::back_insert_iterator<string>(chfind_cmd),
//		lit("channelfind pattern=") << karma::string(channelname)
//	);
//	cmd_list->push(new CCommand(chfind_cmd, "cid"));
//
//	string chedit_cmd;
//	karma::generate(std::back_insert_iterator<string>(chedit_cmd),
//		lit("channeledit cid=<1> channel_needed_talk_power=") << karma::int_(talkpower)
//	);
//	cmd_list->push(new CCommand(chedit_cmd));
//
//	CNetwork::Get()->PushCommandList(cmd_list);
//	return 1;
//}

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
//
////native TSC_SetClientChannelGroup(uid[], groupid, channelname[]);
//AMX_DECLARE_NATIVE(Native::TSC_SetClientChannelGroup)
//{
//	char
//		*uid_tmp = NULL,
//		*channelname_tmp = NULL;
//
//	amx_StrParam(amx, params[1], uid_tmp);
//	int groupid = static_cast<int>(params[2]);
//	amx_StrParam(amx, params[3], channelname_tmp);
//	
//	if (uid_tmp == NULL || channelname_tmp == NULL)
//		return 0;
//
//	string 
//		uid(uid_tmp),
//		channelname(channelname_tmp);
//
//	CNetwork::Get()->EscapeString(uid);
//	CNetwork::Get()->EscapeString(channelname);
//	
//	
//	CommandList *cmd_list = new CommandList;
//
//	string chfind_cmd;
//	karma::generate(std::back_insert_iterator<string>(chfind_cmd),
//		lit("channelfind pattern=") << karma::string(channelname)
//	);
//	cmd_list->push(new CCommand(chfind_cmd, "cid"));
//
//	string cldbfind_cmd;
//	karma::generate(std::back_insert_iterator<string>(cldbfind_cmd),
//		lit("clientdbfind pattern=") << karma::string(uid) << lit(" -uid")
//	);
//	cmd_list->push(new CCommand(cldbfind_cmd, "cldbid"));
//
//	string clchgrp_cmd;
//	karma::generate(std::back_insert_iterator<string>(clchgrp_cmd),
//		lit("setclientchannelgroup cgid=") << karma::int_(groupid) << lit(" cid=<1> cldbid=<2>")
//	);
//	cmd_list->push(new CCommand(clchgrp_cmd));
//
//	CNetwork::Get()->PushCommandList(cmd_list);
//	return 1;
//}
//
////native TSC_AddClientToServerGroup(uid[], groupid);
//AMX_DECLARE_NATIVE(Native::TSC_AddClientToServerGroup)
//{
//	char *uid_tmp = NULL;
//
//	amx_StrParam(amx, params[1], uid_tmp);
//	int groupid = static_cast<int>(params[2]);
//
//	if (uid_tmp == NULL)
//		return 0;
//
//	string uid(uid_tmp);
//	CNetwork::Get()->EscapeString(uid);
//	
//
//	CommandList *cmd_list = new CommandList;
//
//	string cldbfind_cmd;
//	karma::generate(std::back_insert_iterator<string>(cldbfind_cmd),
//		lit("clientdbfind pattern=") << karma::string(uid) << lit(" -uid")
//	);
//	cmd_list->push(new CCommand(cldbfind_cmd, "cldbid"));
//
//	string srvgrpcl_cmd;
//	karma::generate(std::back_insert_iterator<string>(srvgrpcl_cmd),
//		lit("servergroupaddclient sgid=") << karma::int_(groupid) << lit(" cldbid=<1>")
//	);
//	cmd_list->push(new CCommand(srvgrpcl_cmd));
//
//	CNetwork::Get()->PushCommandList(cmd_list);
//	return 1;
//}
//
////native TSC_RemoveClientFromServerGroup(uid[], groupid);
//AMX_DECLARE_NATIVE(Native::TSC_RemoveClientFromServerGroup)
//{
//	char *uid_tmp = NULL;
//
//	amx_StrParam(amx, params[1], uid_tmp);
//	int groupid = static_cast<int>(params[2]);
//
//	if (uid_tmp == NULL)
//		return 0;
//
//	string uid(uid_tmp);
//	CNetwork::Get()->EscapeString(uid);
//
//	
//	CommandList *cmd_list = new CommandList;
//
//	string cldbfind_cmd;
//	karma::generate(std::back_insert_iterator<string>(cldbfind_cmd),
//		lit("clientdbfind pattern=") << karma::string(uid) << lit(" -uid")
//	);
//	cmd_list->push(new CCommand(cldbfind_cmd, "cldbid"));
//
//	string srvgrpcl_cmd;
//	karma::generate(std::back_insert_iterator<string>(srvgrpcl_cmd),
//		lit("servergroupdelclient sgid=") << karma::int_(groupid) << lit(" cldbid=<1>")
//	);
//	cmd_list->push(new CCommand(srvgrpcl_cmd));
//
//	CNetwork::Get()->PushCommandList(cmd_list);
//	return 1;
//}
//
//
////native TSC_KickClient(uid[], kicktype, reason[]);
//AMX_DECLARE_NATIVE(Native::TSC_KickClient)
//{
//	char
//		*uid_tmp = NULL,
//		*reason_msg_tmp = NULL;
//
//	amx_StrParam(amx, params[1], uid_tmp);
//	int kicktype = static_cast<int>(params[2]);
//	amx_StrParam(amx, params[3], reason_msg_tmp);
//
//	if (uid_tmp == NULL)
//		return 0;
//
//	string
//		uid(uid_tmp),
//		reason_msg(reason_msg_tmp == NULL ? string() : reason_msg_tmp);
//	int kickreason_id;
//
//	switch (kicktype) {
//		case KICK_TYPE_CHANNEL:
//			kickreason_id = 4;
//			break;
//		case KICK_TYPE_SERVER:
//			kickreason_id = 5;
//			break;
//		default:
//			return 0;
//	}
//
//	CNetwork::Get()->EscapeString(uid);
//	CNetwork::Get()->EscapeString(reason_msg);
//
//
//	CommandList *cmd_list = new CommandList;
//	
//	CCommand *cmd = new CCommand("clientlist -uid", "clid");
//	karma::generate(std::back_insert_iterator<string>(cmd->MFind),
//		lit("client_unique_identifier=") << karma::string(uid)
//	);
//	cmd_list->push(cmd);
//
//	string clkick_cmd;
//	karma::generate(std::back_insert_iterator<string>(clkick_cmd),
//		lit("clientkick clid=<1> reasonid=") << karma::int_(kickreason_id) << lit(" reasonmsg=") << karma::string(reason_msg)
//	);
//	cmd_list->push(new CCommand(clkick_cmd));
//
//	CNetwork::Get()->PushCommandList(cmd_list);
//	return 1;
//}
//
////native TSC_BanClient(uid[], seconds, reason[]);
//AMX_DECLARE_NATIVE(Native::TSC_BanClient)
//{
//	char
//		*uid_tmp = NULL,
//		*reason_msg_tmp = NULL;
//
//	amx_StrParam(amx, params[1], uid_tmp);
//	unsigned int seconds = static_cast<unsigned int>(params[2] < 0 ? 0 : params[2]);
//	amx_StrParam(amx, params[3], reason_msg_tmp);
//
//	if (uid_tmp == NULL)
//		return 0;
//
//	string
//		uid(uid_tmp),
//		reason_msg(reason_msg_tmp == NULL ? string() : reason_msg_tmp);
//
//	CNetwork::Get()->EscapeString(uid);
//	CNetwork::Get()->EscapeString(reason_msg);
//	
//
//	CommandList *cmd_list = new CommandList;
//	
//	string clban_cmd;
//	karma::generate(std::back_insert_iterator<string>(clban_cmd),
//		lit("banadd uid=") << karma::string(uid) << lit(" time=") << karma::uint_(seconds) << lit(" banreason=") << karma::string(reason_msg)
//	);
//	cmd_list->push(new CCommand(clban_cmd));
//
//	CNetwork::Get()->PushCommandList(cmd_list);
//	return 1;
//}
//
////native TSC_MoveClient(uid[], channelname[]);
//AMX_DECLARE_NATIVE(Native::TSC_MoveClient)
//{
//	char
//		*uid_tmp = NULL,
//		*channelname_tmp = NULL;
//
//	amx_StrParam(amx, params[1], uid_tmp);
//	amx_StrParam(amx, params[2], channelname_tmp);
//
//	if (uid_tmp == NULL || channelname_tmp == NULL)
//		return 0;
//
//	string
//		uid(uid_tmp),
//		channelname(channelname_tmp);
//
//	CNetwork::Get()->EscapeString(uid);
//	CNetwork::Get()->EscapeString(channelname);
//	
//
//	CommandList *cmd_list = new CommandList;
//	
//	CCommand *cmd = new CCommand("clientlist -uid", "clid");
//	karma::generate(std::back_insert_iterator<string>(cmd->MFind),
//		lit("client_unique_identifier=") << karma::string(uid)
//	);
//	cmd_list->push(cmd);
//
//	string chfind_cmd;
//	karma::generate(std::back_insert_iterator<string>(chfind_cmd),
//		lit("channelfind pattern=") << karma::string(channelname)
//	);
//	cmd_list->push(new CCommand(chfind_cmd, "cid"));
//
//	cmd_list->push(new CCommand("clientmove clid=<1> cid=<2>"));
//
//	CNetwork::Get()->PushCommandList(cmd_list);
//	return 1;
//}
//
////native TSC_ToggleClientTalkAbility(uid[], bool:toggle);
//AMX_DECLARE_NATIVE(Native::TSC_ToggleClientTalkAbility)
//{
//	char *uid_tmp = NULL;
//
//	amx_StrParam(amx, params[1], uid_tmp);
//	bool toggle = (params[2] != 0);
//
//	if (uid_tmp == NULL)
//		return 0;
//
//	string uid(uid_tmp);
//	CNetwork::Get()->EscapeString(uid);
//
//
//	CommandList *cmd_list = new CommandList;
//
//	CCommand *cmd = new CCommand("clientlist -uid", "clid");
//	karma::generate(std::back_insert_iterator<string>(cmd->MFind),
//		lit("client_unique_identifier=") << karma::string(uid)
//	);
//	cmd_list->push(cmd);
//
//	string cledit_cmd;
//	karma::generate(std::back_insert_iterator<string>(cledit_cmd),
//		lit("clientedit clid=<1> client_is_talker=") << karma::int_(toggle = true ? 1 : 0)
//	);
//	cmd_list->push(new CCommand(cledit_cmd));
//
//	CNetwork::Get()->PushCommandList(cmd_list);
//	return 1;
//}
//
//
////native TSC_PokeClient(uid[], msg[]);
//AMX_DECLARE_NATIVE(Native::TSC_PokeClient)
//{
//	char
//		*uid_tmp = NULL,
//		*msg_tmp = NULL;
//
//	amx_StrParam(amx, params[1], uid_tmp);
//	amx_StrParam(amx, params[2], msg_tmp);
//
//	if (uid_tmp == NULL || msg_tmp == NULL)
//		return 0;
//
//	string
//		uid(uid_tmp),
//		msg(msg_tmp);
//
//	CNetwork::Get()->EscapeString(uid);
//	CNetwork::Get()->EscapeString(msg);
//
//
//	CommandList *cmd_list = new CommandList;
//
//	CCommand *cmd = new CCommand("clientlist -uid", "clid");
//	karma::generate(std::back_insert_iterator<string>(cmd->MFind),
//		lit("client_unique_identifier=") << karma::string(uid)
//	);
//	cmd_list->push(cmd);
//
//	string clpoke_cmd;
//	karma::generate(std::back_insert_iterator<string>(clpoke_cmd),
//		lit("clientpoke msg=") << karma::string(msg) << lit(" clid=<1>")
//	);
//	cmd_list->push(new CCommand(clpoke_cmd));
//
//	CNetwork::Get()->PushCommandList(cmd_list);
//	return 1;
//}
//
//
////native TSC_SendClientMessage(uid[], msg[]);
//AMX_DECLARE_NATIVE(Native::TSC_SendClientMessage)
//{
//	char
//		*uid_tmp = NULL,
//		*msg_tmp = NULL;
//
//	amx_StrParam(amx, params[1], uid_tmp);
//	amx_StrParam(amx, params[2], msg_tmp);
//
//	if (uid_tmp == NULL || msg_tmp == NULL)
//		return 0;
//
//	string
//		uid(uid_tmp),
//		msg(msg_tmp);
//
//	CNetwork::Get()->EscapeString(uid);
//	CNetwork::Get()->EscapeString(msg);
//
//
//	CommandList *cmd_list = new CommandList;
//
//	CCommand *cmd = new CCommand("clientlist -uid", "clid");
//	karma::generate(std::back_insert_iterator<string>(cmd->MFind),
//		lit("client_unique_identifier=") << karma::string(uid)
//	);
//	cmd_list->push(cmd);
//
//	string msg_cmd;
//	karma::generate(std::back_insert_iterator<string>(msg_cmd),
//		lit("sendtextmessage targetmode=1 target=<1> msg=") << karma::string(msg)
//	);
//	cmd_list->push(new CCommand(msg_cmd));
//
//	CNetwork::Get()->PushCommandList(cmd_list);
//	return 1;
//}
//
////native TSC_SendChannelMessage(channelname[], msg[]);
//AMX_DECLARE_NATIVE(Native::TSC_SendChannelMessage)
//{
//	char
//		*channelname_tmp = NULL,
//		*msg_tmp = NULL;
//
//	amx_StrParam(amx, params[1], channelname_tmp);
//	amx_StrParam(amx, params[2], msg_tmp);
//
//	if (channelname_tmp == NULL || msg_tmp == NULL)
//		return 0;
//
//	string
//		channelname(channelname_tmp),
//		msg(msg_tmp);
//
//	CNetwork::Get()->EscapeString(channelname);
//	CNetwork::Get()->EscapeString(msg);
//
//
//	CommandList *cmd_list = new CommandList;
//
//	string chfind_cmd;
//	karma::generate(std::back_insert_iterator<string>(chfind_cmd),
//		lit("channelfind pattern=") << karma::string(channelname)
//	);
//	cmd_list->push(new CCommand(chfind_cmd, "cid"));
//
//	string msg_cmd;
//	karma::generate(std::back_insert_iterator<string>(msg_cmd),
//		lit("sendtextmessage targetmode=2 target=<1> msg=") << karma::string(msg)
//	);
//	cmd_list->push(new CCommand(msg_cmd));
//
//	CNetwork::Get()->PushCommandList(cmd_list);
//	return 1;
//}
//
//native TSC_SendServerMessage(msg[]);
AMX_DECLARE_NATIVE(Native::TSC_SendServerMessage)
{
	char *msg_tmp = NULL;
	amx_StrParam(amx, params[1], msg_tmp);

	if (msg_tmp == NULL)
		return 0;


	return CServer::Get()->SendServerMessage(msg_tmp);
}
