#include "main.h"
#include "natives.h"

#include "CTeamspeak.h"


//native TSC_Connect(ip[], port[]);
AMX_DECLARE_NATIVE(Native::TSC_Connect)
{
	char 
		*ip = NULL,
		*port = NULL;

	amx_StrParam(amx, params[1], ip);
	amx_StrParam(amx, params[2], port);

	if (ip == NULL || port == NULL)
		return 0;

	CTeamspeak::Get()->Connect(ip, port);
	return 1;
}

//native TSC_Disconnect();
AMX_DECLARE_NATIVE(Native::TSC_Disconnect)
{
	CTeamspeak::Get()->Disconnect();
	return 1;
}

//native TSC_Login(user[], pass[], nickname[]);
AMX_DECLARE_NATIVE(Native::TSC_Login)
{
	char
		*login_tmp = NULL,
		*pass_tmp = NULL,
		*nick_tmp = NULL;

	amx_StrParam(amx, params[1], login_tmp);
	amx_StrParam(amx, params[2], pass_tmp);
	amx_StrParam(amx, params[3], nick_tmp);

	if (login_tmp == NULL || pass_tmp == NULL || nick_tmp == NULL)
		return 0;

	string
		login(login_tmp),
		pass(pass_tmp),
		nickname(nick_tmp);

	CTeamspeak::Get()->EscapeString(login);
	CTeamspeak::Get()->EscapeString(pass);
	CTeamspeak::Get()->EscapeString(nickname);


	CommandList *cmd_list = new CommandList;

	string login_cmd;
	karma::generate(std::back_insert_iterator<string>(login_cmd),
		lit("login client_login_name=") << karma::string(login) << lit(" client_login_password=") << karma::string(pass) << lit("\n") <<
		lit("clientupdate client_nickname=") << karma::string(nickname)
	);
	cmd_list->push(new CCommand(login_cmd));
	
	CTeamspeak::Get()->PushCommandList(cmd_list);
	return 1;
}

//native TSC_ChangeNickname(nickname[]);
AMX_DECLARE_NATIVE(Native::TSC_ChangeNickname)
{
	char *nick_tmp = NULL;
	amx_StrParam(amx, params[1], nick_tmp);

	if (nick_tmp == NULL)
		return 0;

	string nickname(nick_tmp);
	CTeamspeak::Get()->EscapeString(nickname);


	CommandList *cmd_list = new CommandList;

	string nick_cmd;
	karma::generate(std::back_insert_iterator<string>(nick_cmd),
		lit("clientupdate client_nickname=") << karma::string(nickname)
	);
	cmd_list->push(new CCommand(nick_cmd));
	
	CTeamspeak::Get()->PushCommandList(cmd_list);
	return 1;
}

//native TSC_CreateChannel(channelname[]);
AMX_DECLARE_NATIVE(Native::TSC_CreateChannel)
{
	char *channelname_tmp = NULL;
	amx_StrParam(amx, params[1], channelname_tmp);

	if (channelname_tmp == NULL)
		return 0;

	string channelname(channelname_tmp);
	CTeamspeak::Get()->EscapeString(channelname);


	CommandList *cmd_list = new CommandList;

	string chcreate_cmd;
	karma::generate(std::back_insert_iterator<string>(chcreate_cmd),
		lit("channelcreate channel_name=") << karma::string(channelname)
	);
	cmd_list->push(new CCommand(chcreate_cmd));

	CTeamspeak::Get()->PushCommandList(cmd_list);
	return 1;
}

//native TSC_DeleteChannel(channelname[]);
AMX_DECLARE_NATIVE(Native::TSC_DeleteChannel)
{
	char *channelname_tmp = NULL;
	amx_StrParam(amx, params[1], channelname_tmp);

	if (channelname_tmp == NULL)
		return 0;

	string channelname(channelname_tmp);
	CTeamspeak::Get()->EscapeString(channelname);
	

	CommandList *cmd_list = new CommandList;
	
	string chfind_cmd;
	karma::generate(std::back_insert_iterator<string>(chfind_cmd),
		lit("channelfind pattern=") << karma::string(channelname)
	);
	cmd_list->push(new CCommand(chfind_cmd, "cid"));
	cmd_list->push(new CCommand("channeldelete cid=<1> force=1"));

	CTeamspeak::Get()->PushCommandList(cmd_list);
	return 1;
}

//native TSC_SetChannelName(channelname[], newname[]);
AMX_DECLARE_NATIVE(Native::TSC_SetChannelName)
{
	char
		*channelname_tmp = NULL,
		*newname_tmp = NULL;
	amx_StrParam(amx, params[1], channelname_tmp);
	amx_StrParam(amx, params[2], newname_tmp);

	if (channelname_tmp == NULL || newname_tmp == NULL)
		return 0;

	string 
		channelname(channelname_tmp),
		new_channelname(newname_tmp);
	
	CTeamspeak::Get()->EscapeString(channelname);
	CTeamspeak::Get()->EscapeString(new_channelname);


	CommandList *cmd_list = new CommandList;

	string chfind_cmd;
	karma::generate(std::back_insert_iterator<string>(chfind_cmd),
		lit("channelfind pattern=") << karma::string(channelname)
	);
	cmd_list->push(new CCommand(chfind_cmd, "cid"));

	string chedit_cmd;
	karma::generate(std::back_insert_iterator<string>(chedit_cmd),
		lit("channeledit cid=<1> channel_name=") << karma::string(new_channelname)
	);
	cmd_list->push(new CCommand(chedit_cmd));

	CTeamspeak::Get()->PushCommandList(cmd_list);
	return 1;
}

//native TSC_SetChannelDescription(channelname[], desc[]);
AMX_DECLARE_NATIVE(Native::TSC_SetChannelDescription)
{
	char
		*channelname_tmp = NULL,
		*channeldesc_tmp = NULL;

	amx_StrParam(amx, params[1], channelname_tmp);
	amx_StrParam(amx, params[2], channeldesc_tmp);

	if (channelname_tmp == NULL)
		return 0;

	string
		channelname(channelname_tmp),
		channeldesc(channeldesc_tmp == NULL ? string() : channeldesc_tmp);

	CTeamspeak::Get()->EscapeString(channelname);
	CTeamspeak::Get()->EscapeString(channeldesc);
	

	CommandList *cmd_list = new CommandList;

	string chfind_cmd;
	karma::generate(std::back_insert_iterator<string>(chfind_cmd),
		lit("channelfind pattern=") << karma::string(channelname)
	);
	cmd_list->push(new CCommand(chfind_cmd, "cid"));

	string chedit_cmd;
	karma::generate(std::back_insert_iterator<string>(chedit_cmd),
		lit("channeledit cid=<1> channel_description=") << karma::string(channeldesc)
	);
	cmd_list->push(new CCommand(chedit_cmd));

	CTeamspeak::Get()->PushCommandList(cmd_list);
	return 1;
}

//native TSC_SetChannelType(channelname[], type);
AMX_DECLARE_NATIVE(Native::TSC_SetChannelType)
{
	char *channelname_tmp = NULL;
	amx_StrParam(amx, params[1], channelname_tmp);

	if (channelname_tmp == NULL)
		return 0;

	string 
		channelname(channelname_tmp),
		channeltype;

	CTeamspeak::Get()->EscapeString(channelname);

	switch(params[2]) {
		case CHANNEL_TYPE_PERMANENT:
			channeltype = "channel_flag_permanent";
			break;

		case CHANNEL_TYPE_SEMI_PERMANENT:
			channeltype = "channel_flag_semi_permanent";
			break;

		case CHANNEL_TYPE_TEMPORARY:
			channeltype = "channel_flag_temporary";
			break;

		default:
			return 0;
	}


	CommandList *cmd_list = new CommandList;

	string chfind_cmd;
	karma::generate(std::back_insert_iterator<string>(chfind_cmd),
		lit("channelfind pattern=") << karma::string(channelname)
	);
	cmd_list->push(new CCommand(chfind_cmd, "cid"));

	string chedit_cmd;
	karma::generate(std::back_insert_iterator<string>(chedit_cmd),
		lit("channeledit cid=<1> ") << karma::string(channeltype) << lit("=1")
	);
	cmd_list->push(new CCommand(chedit_cmd));

	CTeamspeak::Get()->PushCommandList(cmd_list);
	return 1;
}

//native TSC_SetChannelPassword(channelname[], password[]);
AMX_DECLARE_NATIVE(Native::TSC_SetChannelPassword)
{
	char
		*channelname_tmp = NULL,
		*channelpwd_tmp = NULL;

	amx_StrParam(amx, params[1], channelname_tmp);
	amx_StrParam(amx, params[2], channelpwd_tmp);

	if (channelname_tmp == NULL)
		return 0;

	string 
		channelname(channelname_tmp),
		channelpwd(channelpwd_tmp == NULL ? string() : channelpwd_tmp);

	CTeamspeak::Get()->EscapeString(channelname);
	CTeamspeak::Get()->EscapeString(channelpwd);


	CommandList *cmd_list = new CommandList;

	string chfind_cmd;
	karma::generate(std::back_insert_iterator<string>(chfind_cmd),
		lit("channelfind pattern=") << karma::string(channelname)
	);
	cmd_list->push(new CCommand(chfind_cmd, "cid"));

	string chedit_cmd;
	karma::generate(std::back_insert_iterator<string>(chedit_cmd),
		lit("channeledit cid=<1> channel_password=") << karma::string(channelpwd)
	);
	cmd_list->push(new CCommand(chedit_cmd));

	CTeamspeak::Get()->PushCommandList(cmd_list);
	return 1;
}

//native TSC_SetChannelTalkPower(channelname[], talkpower);
AMX_DECLARE_NATIVE(Native::TSC_SetChannelTalkPower)
{
	char *channelname_tmp = NULL;
	int talkpower = static_cast<int>(params[2]);

	amx_StrParam(amx, params[1], channelname_tmp);

	if (channelname_tmp == NULL)
		return 0;

	string channelname(channelname_tmp);
	CTeamspeak::Get()->EscapeString(channelname);


	CommandList *cmd_list = new CommandList;

	string chfind_cmd;
	karma::generate(std::back_insert_iterator<string>(chfind_cmd),
		lit("channelfind pattern=") << karma::string(channelname)
	);
	cmd_list->push(new CCommand(chfind_cmd, "cid"));

	string chedit_cmd;
	karma::generate(std::back_insert_iterator<string>(chedit_cmd),
		lit("channeledit cid=<1> channel_needed_talk_power=") << karma::int_(talkpower)
	);
	cmd_list->push(new CCommand(chedit_cmd));

	CTeamspeak::Get()->PushCommandList(cmd_list);
	return 1;
}

//native TSC_SetChannelUserLimit(channelname[], maxuser);
AMX_DECLARE_NATIVE(Native::TSC_SetChannelUserLimit)
{
	char *channelname_tmp = NULL;
	unsigned int maxusers = static_cast<unsigned int>(params[2] < 0 ? 0 : params[2]);

	amx_StrParam(amx, params[1], channelname_tmp);

	if (channelname_tmp == NULL)
		return 0;

	string channelname(channelname_tmp);
	CTeamspeak::Get()->EscapeString(channelname);


	CommandList *cmd_list = new CommandList;

	string chfind_cmd;
	karma::generate(std::back_insert_iterator<string>(chfind_cmd),
		lit("channelfind pattern=") << karma::string(channelname)
	);
	cmd_list->push(new CCommand(chfind_cmd, "cid"));

	string chedit_cmd;
	karma::generate(std::back_insert_iterator<string>(chedit_cmd),
		lit("channeledit cid=<1> channel_maxclients=") << karma::uint_(maxusers)
	);
	cmd_list->push(new CCommand(chedit_cmd));

	CTeamspeak::Get()->PushCommandList(cmd_list);
	return 1;
}

//native TSC_SetChannelSubChannel(channelname[], parentchannelname[]);
AMX_DECLARE_NATIVE(Native::TSC_SetChannelSubChannel)
{
	char
		*channelname_tmp = NULL,
		*parent_channelname_tmp = NULL;

	amx_StrParam(amx, params[1], channelname_tmp);
	amx_StrParam(amx, params[2], parent_channelname_tmp);

	if (channelname_tmp == NULL || parent_channelname_tmp == NULL)
		return 0;

	string
		channelname(channelname_tmp),
		parent_channelname(parent_channelname_tmp);

	CTeamspeak::Get()->EscapeString(channelname);
	CTeamspeak::Get()->EscapeString(parent_channelname);

	
	CommandList *cmd_list = new CommandList;

	string chfind_cmd;
	karma::generate(std::back_insert_iterator<string>(chfind_cmd),
		lit("channelfind pattern=") << karma::string(channelname)
	);
	cmd_list->push(new CCommand(chfind_cmd, "cid"));

	string chfind2_cmd;
	karma::generate(std::back_insert_iterator<string>(chfind2_cmd),
		lit("channelfind pattern=") << karma::string(parent_channelname)
	);
	cmd_list->push(new CCommand(chfind2_cmd, "cid"));

	cmd_list->push(new CCommand("channelmove cid=<1> cpid=<2>"));

	CTeamspeak::Get()->PushCommandList(cmd_list);
	return 1;
}

//native TSC_MoveChannelBelowChannel(channelname[], parentchannelname[]);
AMX_DECLARE_NATIVE(Native::TSC_MoveChannelBelowChannel)
{
	char
		*channelname_tmp = NULL,
		*parent_channelname_tmp = NULL;

	amx_StrParam(amx, params[1], channelname_tmp);
	amx_StrParam(amx, params[2], parent_channelname_tmp);

	if (channelname_tmp == NULL || parent_channelname_tmp == NULL)
		return 0;

	string
		channelname(channelname_tmp),
		parent_channelname(parent_channelname_tmp);

	CTeamspeak::Get()->EscapeString(channelname);
	CTeamspeak::Get()->EscapeString(parent_channelname);

	
	CommandList *cmd_list = new CommandList;

	string chfind_cmd;
	karma::generate(std::back_insert_iterator<string>(chfind_cmd),
		lit("channelfind pattern=") << karma::string(channelname)
	);
	cmd_list->push(new CCommand(chfind_cmd, "cid"));

	string chfind2_cmd;
	karma::generate(std::back_insert_iterator<string>(chfind2_cmd),
		lit("channelfind pattern=") << karma::string(parent_channelname)
	);
	cmd_list->push(new CCommand(chfind2_cmd, "cid"));

	cmd_list->push(new CCommand("channeledit cid=<1> channel_order=<2>"));

	CTeamspeak::Get()->PushCommandList(cmd_list);
	return 1;
}

//native TSC_SetClientChannelGroup(uid[], groupid, channelname[]);
AMX_DECLARE_NATIVE(Native::TSC_SetClientChannelGroup)
{
	char
		*uid_tmp = NULL,
		*channelname_tmp = NULL;

	amx_StrParam(amx, params[1], uid_tmp);
	int groupid = static_cast<int>(params[2]);
	amx_StrParam(amx, params[3], channelname_tmp);
	
	if (uid_tmp == NULL || channelname_tmp == NULL)
		return 0;

	string 
		uid(uid_tmp),
		channelname(channelname_tmp);

	CTeamspeak::Get()->EscapeString(uid);
	CTeamspeak::Get()->EscapeString(channelname);
	
	
	CommandList *cmd_list = new CommandList;

	string chfind_cmd;
	karma::generate(std::back_insert_iterator<string>(chfind_cmd),
		lit("channelfind pattern=") << karma::string(channelname)
	);
	cmd_list->push(new CCommand(chfind_cmd, "cid"));

	string cldbfind_cmd;
	karma::generate(std::back_insert_iterator<string>(cldbfind_cmd),
		lit("clientdbfind pattern=") << karma::string(uid) << lit(" -uid")
	);
	cmd_list->push(new CCommand(cldbfind_cmd, "cldbid"));

	string clchgrp_cmd;
	karma::generate(std::back_insert_iterator<string>(clchgrp_cmd),
		lit("setclientchannelgroup cgid=") << karma::int_(groupid) << lit(" cid=<1> cldbid=<2>")
	);
	cmd_list->push(new CCommand(clchgrp_cmd));

	CTeamspeak::Get()->PushCommandList(cmd_list);
	return 1;
}

//native TSC_AddClientToServerGroup(uid[], groupid);
AMX_DECLARE_NATIVE(Native::TSC_AddClientToServerGroup)
{
	char *uid_tmp = NULL;

	amx_StrParam(amx, params[1], uid_tmp);
	int groupid = static_cast<int>(params[2]);

	if (uid_tmp == NULL)
		return 0;

	string uid(uid_tmp);
	CTeamspeak::Get()->EscapeString(uid);
	

	CommandList *cmd_list = new CommandList;

	string cldbfind_cmd;
	karma::generate(std::back_insert_iterator<string>(cldbfind_cmd),
		lit("clientdbfind pattern=") << karma::string(uid) << lit(" -uid")
	);
	cmd_list->push(new CCommand(cldbfind_cmd, "cldbid"));

	string srvgrpcl_cmd;
	karma::generate(std::back_insert_iterator<string>(srvgrpcl_cmd),
		lit("servergroupaddclient sgid=") << karma::int_(groupid) << lit(" cldbid=<1>")
	);
	cmd_list->push(new CCommand(srvgrpcl_cmd));

	CTeamspeak::Get()->PushCommandList(cmd_list);
	return 1;
}

//native TSC_RemoveClientFromServerGroup(uid[], groupid);
AMX_DECLARE_NATIVE(Native::TSC_RemoveClientFromServerGroup)
{
	char *uid_tmp = NULL;

	amx_StrParam(amx, params[1], uid_tmp);
	int groupid = static_cast<int>(params[2]);

	if (uid_tmp == NULL)
		return 0;

	string uid(uid_tmp);
	CTeamspeak::Get()->EscapeString(uid);

	
	CommandList *cmd_list = new CommandList;

	string cldbfind_cmd;
	karma::generate(std::back_insert_iterator<string>(cldbfind_cmd),
		lit("clientdbfind pattern=") << karma::string(uid) << lit(" -uid")
	);
	cmd_list->push(new CCommand(cldbfind_cmd, "cldbid"));

	string srvgrpcl_cmd;
	karma::generate(std::back_insert_iterator<string>(srvgrpcl_cmd),
		lit("servergroupdelclient sgid=") << karma::int_(groupid) << lit(" cldbid=<1>")
	);
	cmd_list->push(new CCommand(srvgrpcl_cmd));

	CTeamspeak::Get()->PushCommandList(cmd_list);
	return 1;
}


//native TSC_KickClient(uid[], kicktype, reason[]);
AMX_DECLARE_NATIVE(Native::TSC_KickClient)
{
	char
		*uid_tmp = NULL,
		*reason_msg_tmp = NULL;

	amx_StrParam(amx, params[1], uid_tmp);
	int kicktype = static_cast<int>(params[2]);
	amx_StrParam(amx, params[3], reason_msg_tmp);

	if (uid_tmp == NULL)
		return 0;

	string
		uid(uid_tmp),
		reason_msg(reason_msg_tmp == NULL ? string() : reason_msg_tmp);
	int kickreason_id;

	switch (kicktype) {
		case KICK_TYPE_CHANNEL:
			kickreason_id = 4;
			break;
		case KICK_TYPE_SERVER:
			kickreason_id = 5;
			break;
		default:
			return 0;
	}

	CTeamspeak::Get()->EscapeString(uid);
	CTeamspeak::Get()->EscapeString(reason_msg);


	CommandList *cmd_list = new CommandList;
	
	CCommand *cmd = new CCommand("clientlist -uid", "clid");
	karma::generate(std::back_insert_iterator<string>(cmd->MFind),
		lit("client_unique_identifier=") << karma::string(uid)
	);
	cmd_list->push(cmd);

	string clkick_cmd;
	karma::generate(std::back_insert_iterator<string>(clkick_cmd),
		lit("clientkick clid=<1> reasonid=") << karma::int_(kickreason_id) << lit(" reasonmsg=") << karma::string(reason_msg)
	);
	cmd_list->push(new CCommand(clkick_cmd));

	CTeamspeak::Get()->PushCommandList(cmd_list);
	return 1;
}

//native TSC_BanClient(uid[], seconds, reason[]);
AMX_DECLARE_NATIVE(Native::TSC_BanClient)
{
	char
		*uid_tmp = NULL,
		*reason_msg_tmp = NULL;

	amx_StrParam(amx, params[1], uid_tmp);
	unsigned int seconds = static_cast<unsigned int>(params[2] < 0 ? 0 : params[2]);
	amx_StrParam(amx, params[3], reason_msg_tmp);

	if (uid_tmp == NULL)
		return 0;

	string
		uid(uid_tmp),
		reason_msg(reason_msg_tmp == NULL ? string() : reason_msg_tmp);

	CTeamspeak::Get()->EscapeString(uid);
	CTeamspeak::Get()->EscapeString(reason_msg);
	

	CommandList *cmd_list = new CommandList;
	
	string clban_cmd;
	karma::generate(std::back_insert_iterator<string>(clban_cmd),
		lit("banadd uid=") << karma::string(uid) << lit(" time=") << karma::uint_(seconds) << lit(" banreason=") << karma::string(reason_msg)
	);
	cmd_list->push(new CCommand(clban_cmd));

	CTeamspeak::Get()->PushCommandList(cmd_list);
	return 1;
}

//native TSC_MoveClient(uid[], channelname[]);
AMX_DECLARE_NATIVE(Native::TSC_MoveClient)
{
	char
		*uid_tmp = NULL,
		*channelname_tmp = NULL;

	amx_StrParam(amx, params[1], uid_tmp);
	amx_StrParam(amx, params[2], channelname_tmp);

	if (uid_tmp == NULL || channelname_tmp == NULL)
		return 0;

	string
		uid(uid_tmp),
		channelname(channelname_tmp);

	CTeamspeak::Get()->EscapeString(uid);
	CTeamspeak::Get()->EscapeString(channelname);
	

	CommandList *cmd_list = new CommandList;
	
	CCommand *cmd = new CCommand("clientlist -uid", "clid");
	karma::generate(std::back_insert_iterator<string>(cmd->MFind),
		lit("client_unique_identifier=") << karma::string(uid)
	);
	cmd_list->push(cmd);

	string chfind_cmd;
	karma::generate(std::back_insert_iterator<string>(chfind_cmd),
		lit("channelfind pattern=") << karma::string(channelname)
	);
	cmd_list->push(new CCommand(chfind_cmd, "cid"));

	cmd_list->push(new CCommand("clientmove clid=<1> cid=<2>"));

	CTeamspeak::Get()->PushCommandList(cmd_list);
	return 1;
}

//native TSC_ToggleClientTalkAbility(uid[], bool:toggle);
AMX_DECLARE_NATIVE(Native::TSC_ToggleClientTalkAbility)
{
	char *uid_tmp = NULL;

	amx_StrParam(amx, params[1], uid_tmp);
	bool toggle = (params[2] != 0);

	if (uid_tmp == NULL)
		return 0;

	string uid(uid_tmp);
	CTeamspeak::Get()->EscapeString(uid);


	CommandList *cmd_list = new CommandList;

	CCommand *cmd = new CCommand("clientlist -uid", "clid");
	karma::generate(std::back_insert_iterator<string>(cmd->MFind),
		lit("client_unique_identifier=") << karma::string(uid)
	);
	cmd_list->push(cmd);

	string cledit_cmd;
	karma::generate(std::back_insert_iterator<string>(cledit_cmd),
		lit("clientedit clid=<1> client_is_talker=") << karma::int_(toggle = true ? 1 : 0)
	);
	cmd_list->push(new CCommand(cledit_cmd));

	CTeamspeak::Get()->PushCommandList(cmd_list);
	return 1;
}


//native TSC_PokeClient(uid[], msg[]);
AMX_DECLARE_NATIVE(Native::TSC_PokeClient)
{
	char
		*uid_tmp = NULL,
		*msg_tmp = NULL;

	amx_StrParam(amx, params[1], uid_tmp);
	amx_StrParam(amx, params[2], msg_tmp);

	if (uid_tmp == NULL || msg_tmp == NULL)
		return 0;

	string
		uid(uid_tmp),
		msg(msg_tmp);

	CTeamspeak::Get()->EscapeString(uid);
	CTeamspeak::Get()->EscapeString(msg);


	CommandList *cmd_list = new CommandList;

	CCommand *cmd = new CCommand("clientlist -uid", "clid");
	karma::generate(std::back_insert_iterator<string>(cmd->MFind),
		lit("client_unique_identifier=") << karma::string(uid)
	);
	cmd_list->push(cmd);

	string clpoke_cmd;
	karma::generate(std::back_insert_iterator<string>(clpoke_cmd),
		lit("clientpoke msg=") << karma::string(msg) << lit(" clid=<1>")
	);
	cmd_list->push(new CCommand(clpoke_cmd));

	CTeamspeak::Get()->PushCommandList(cmd_list);
	return 1;
}


//native TSC_SendClientMessage(uid[], msg[]);
AMX_DECLARE_NATIVE(Native::TSC_SendClientMessage)
{
	char
		*uid_tmp = NULL,
		*msg_tmp = NULL;

	amx_StrParam(amx, params[1], uid_tmp);
	amx_StrParam(amx, params[2], msg_tmp);

	if (uid_tmp == NULL || msg_tmp == NULL)
		return 0;

	string
		uid(uid_tmp),
		msg(msg_tmp);

	CTeamspeak::Get()->EscapeString(uid);
	CTeamspeak::Get()->EscapeString(msg);


	CommandList *cmd_list = new CommandList;

	CCommand *cmd = new CCommand("clientlist -uid", "clid");
	karma::generate(std::back_insert_iterator<string>(cmd->MFind),
		lit("client_unique_identifier=") << karma::string(uid)
	);
	cmd_list->push(cmd);

	string msg_cmd;
	karma::generate(std::back_insert_iterator<string>(msg_cmd),
		lit("sendtextmessage targetmode=1 target=<1> msg=") << karma::string(msg)
	);
	cmd_list->push(new CCommand(msg_cmd));

	CTeamspeak::Get()->PushCommandList(cmd_list);
	return 1;
}

//native TSC_SendChannelMessage(channelname[], msg[]);
AMX_DECLARE_NATIVE(Native::TSC_SendChannelMessage)
{
	char
		*channelname_tmp = NULL,
		*msg_tmp = NULL;

	amx_StrParam(amx, params[1], channelname_tmp);
	amx_StrParam(amx, params[2], msg_tmp);

	if (channelname_tmp == NULL || msg_tmp == NULL)
		return 0;

	string
		channelname(channelname_tmp),
		msg(msg_tmp);

	CTeamspeak::Get()->EscapeString(channelname);
	CTeamspeak::Get()->EscapeString(msg);


	CommandList *cmd_list = new CommandList;

	string chfind_cmd;
	karma::generate(std::back_insert_iterator<string>(chfind_cmd),
		lit("channelfind pattern=") << karma::string(channelname)
	);
	cmd_list->push(new CCommand(chfind_cmd, "cid"));

	string msg_cmd;
	karma::generate(std::back_insert_iterator<string>(msg_cmd),
		lit("sendtextmessage targetmode=2 target=<1> msg=") << karma::string(msg)
	);
	cmd_list->push(new CCommand(msg_cmd));

	CTeamspeak::Get()->PushCommandList(cmd_list);
	return 1;
}

//native TSC_SendServerMessage(msg[]);
AMX_DECLARE_NATIVE(Native::TSC_SendServerMessage)
{
	char *msg_tmp = NULL;
	amx_StrParam(amx, params[1], msg_tmp);

	if (msg_tmp == NULL)
		return 0;

	string msg(msg_tmp);
	CTeamspeak::Get()->EscapeString(msg);


	CommandList *cmd_list = new CommandList;

	string getid_cmd;
	karma::generate(std::back_inserter(getid_cmd),
		lit("serveridgetbyport virtualserver_port=") << karma::string(CTeamspeak::Get()->GetPort())
	);
	cmd_list->push(new CCommand(getid_cmd, "server_id"));

	string msg_cmd;
	karma::generate(std::back_insert_iterator<string>(msg_cmd),
		lit("sendtextmessage targetmode=3 target=<1> msg=") << karma::string(msg)
	);
	cmd_list->push(new CCommand(msg_cmd));

	CTeamspeak::Get()->PushCommandList(cmd_list);
	return 1;
}
