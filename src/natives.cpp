#include "main.h"

#include "CTeamspeak.h"

cell AMX_NATIVE_CALL native_TSC_Connect(AMX* amx, cell* params) {
	char 
		*IP = NULL,
		*VPort = NULL;
	amx_StrParam(amx, params[1], IP);
	amx_StrParam(amx, params[2], VPort);
	return TSServer.Connect(IP, VPort);
}

cell AMX_NATIVE_CALL native_TSC_Disconnect(AMX* amx, cell* params) {
	TSServer.Disconnect();
	return 1;
}

cell AMX_NATIVE_CALL native_TSC_Login(AMX* amx, cell* params) {
	char
		*Login = NULL,
		*Pass = NULL,
		*Nick = NULL;
	amx_StrParam(amx, params[1], Login);
	amx_StrParam(amx, params[2], Pass);
	amx_StrParam(amx, params[3], Nick);
	return TSServer.Login(Login, Pass, Nick);
}

//native TSC_CreateChannel(channelname[]);
cell AMX_NATIVE_CALL native_TSC_CreateChannel(AMX* amx, cell* params) {
	char *ChannelNameTmp = NULL;
	amx_StrParam(amx, params[1], ChannelNameTmp);

	string ChannelName(ChannelNameTmp);
	TSServer.EscapeString(ChannelName);


	CommandList *cmds = new CommandList;

	string CmdStr("channelcreate channel_name=");
	CmdStr.append(ChannelName);
	cmds->push(new CCommand(CmdStr));

	TSServer.AddCommandListToQueue(cmds);
	return 1;
}

//native TSC_DeleteChannel(channelname[]);
cell AMX_NATIVE_CALL native_TSC_DeleteChannel(AMX* amx, cell* params) {
	char *ChannelNameTmp = NULL;
	amx_StrParam(amx, params[1], ChannelNameTmp);

	string ChannelName(ChannelNameTmp);
	TSServer.EscapeString(ChannelName);
	

	CommandList *cmds = new CommandList;

	string CmdStr("channelfind pattern=");
	CmdStr.append(ChannelName);
	cmds->push(new CCommand(CmdStr, "cid"));

	cmds->push(new CCommand("channeldelete cid=<1> force=1"));

	TSServer.AddCommandListToQueue(cmds);
	return 1;
}

//native TSC_SetChannelName(channelname[], newname[]);
cell AMX_NATIVE_CALL native_TSC_SetChannelName(AMX* amx, cell* params) {
	char *TmpParam = NULL;

	amx_StrParam(amx, params[1], TmpParam);
	string ChannelName(TmpParam);
	TSServer.EscapeString(ChannelName);

	amx_StrParam(amx, params[2], TmpParam);
	string NewChannelName(TmpParam);
	TSServer.EscapeString(NewChannelName);


	CommandList *cmds = new CommandList;

	string CmdStr("channelfind pattern=");
	CmdStr.append(ChannelName);
	cmds->push(new CCommand(CmdStr, "cid"));

	CmdStr.assign("channeledit cid=<1> channel_name=");
	CmdStr.append(NewChannelName);
	cmds->push(new CCommand(CmdStr));

	TSServer.AddCommandListToQueue(cmds);
	return 1;
}

//native TSC_SetChannelDescription(channelname[], desc[]);
cell AMX_NATIVE_CALL native_TSC_SetChannelDescription(AMX* amx, cell* params) {
	char *TmpParam = NULL;

	amx_StrParam(amx, params[1], TmpParam);
	string ChannelName(TmpParam);
	TSServer.EscapeString(ChannelName);

	amx_StrParam(amx, params[2], TmpParam);
	string Desc(TmpParam);
	TSServer.EscapeString(Desc);


	CommandList *cmds = new CommandList;

	string CmdStr("channelfind pattern=");
	CmdStr.append(ChannelName);
	cmds->push(new CCommand(CmdStr, "cid"));

	CmdStr.assign("channeledit cid=<1> channel_description=");
	CmdStr.append(Desc);
	cmds->push(new CCommand(CmdStr));

	TSServer.AddCommandListToQueue(cmds);
	return 1;
}

//native TSC_SetChannelType(channelname[], type);
cell AMX_NATIVE_CALL native_TSC_SetChannelType(AMX* amx, cell* params) {
	char *TmpParam = NULL;

	amx_StrParam(amx, params[1], TmpParam);
	string 
		ChannelName(TmpParam),
		ChannelType;
	TSServer.EscapeString(ChannelName);

	switch(params[2]) {
	case CHANNEL_TYPE_PERMANENT:
		ChannelType = "channel_flag_permanent";
		break;
	case CHANNEL_TYPE_SEMI_PERMANENT:
		ChannelType = "channel_flag_semi_permanent";
		break;

	case CHANNEL_TYPE_TEMPORARY:
		ChannelType = "channel_flag_temporary";
		break;

	default:
		return 0;
	}

	CommandList *cmds = new CommandList;

	char CmdStr[64];
	sprintf(CmdStr, "channelfind pattern=%s", ChannelName.c_str());
	cmds->push(new CCommand(CmdStr, "cid"));

	sprintf(CmdStr, "channeledit cid=<1>  %s=1", ChannelType.c_str());
	cmds->push(new CCommand(CmdStr));

	TSServer.AddCommandListToQueue(cmds);
	return 1;
}

//native TSC_SetChannelPassword(channelname[], password[]);
cell AMX_NATIVE_CALL native_TSC_SetChannelPassword(AMX* amx, cell* params) {
	char *TmpParam = NULL;

	amx_StrParam(amx, params[1], TmpParam);
	string ChannelName(TmpParam);
	TSServer.EscapeString(ChannelName);

	amx_StrParam(amx, params[2], TmpParam);
	string ChannelPasswd(TmpParam);
	TSServer.EscapeString(ChannelPasswd);


	CommandList *cmds = new CommandList;

	string CmdStr("channelfind pattern=");
	CmdStr.append(ChannelName);
	cmds->push(new CCommand(CmdStr, "cid"));

	CmdStr.assign("channeledit cid=<1> channel_password=");
	CmdStr.append(ChannelPasswd);
	cmds->push(new CCommand(CmdStr));

	TSServer.AddCommandListToQueue(cmds);
	return 1;
}

//native TSC_SetChannelTalkPower(channelname[], talkpower);
cell AMX_NATIVE_CALL native_TSC_SetChannelTalkPower(AMX* amx, cell* params) {
	char *TmpParam = NULL;
	amx_StrParam(amx, params[1], TmpParam);
	string ChannelName(TmpParam);
	TSServer.EscapeString(ChannelName);


	CommandList *cmds = new CommandList;

	string CmdStr("channelfind pattern=");
	CmdStr.append(ChannelName);
	cmds->push(new CCommand(CmdStr, "cid"));

	char FormatTmp[64];
	sprintf(FormatTmp, "channeledit cid=<1> channel_needed_talk_power=%d", (int)params[2]);
	cmds->push(new CCommand(FormatTmp));

	TSServer.AddCommandListToQueue(cmds);
	return 1;
}

//native TSC_SetChannelUserLimit(channelname[], maxuser);
cell AMX_NATIVE_CALL native_TSC_SetChannelUserLimit(AMX* amx, cell* params) {
	if(params[2] < 0)
		params[2] = 0;

	char *TmpParam = NULL;
	amx_StrParam(amx, params[1], TmpParam);
	string ChannelName(TmpParam);
	TSServer.EscapeString(ChannelName);


	CommandList *cmds = new CommandList;

	string CmdStr("channelfind pattern=");
	CmdStr.append(ChannelName);
	cmds->push(new CCommand(CmdStr, "cid"));

	char FormatTmp[64];
	sprintf(FormatTmp, "channeledit cid=<1> channel_maxclients=%d", (int)params[2]);
	cmds->push(new CCommand(FormatTmp));

	TSServer.AddCommandListToQueue(cmds);
	return 1;
}

//native TSC_SetChannelSubChannel(channelname[], parentchannelname[]);
cell AMX_NATIVE_CALL native_TSC_SetChannelSubChannel(AMX* amx, cell* params) {
	char *TmpParam = NULL;

	amx_StrParam(amx, params[1], TmpParam);
	string ChannelName(TmpParam);
	TSServer.EscapeString(ChannelName);

	amx_StrParam(amx, params[2], TmpParam);
	string ParentChannelName(TmpParam);
	TSServer.EscapeString(ParentChannelName);

	
	CommandList *cmds = new CommandList;

	string CmdStr("channelfind pattern=");
	CmdStr.append(ChannelName);
	cmds->push(new CCommand(CmdStr, "cid"));

	CmdStr.assign("channelfind pattern=");
	CmdStr.append(ParentChannelName);
	cmds->push(new CCommand(CmdStr, "cid"));
	
	cmds->push(new CCommand("channelmove cid=<1> cpid=<2>"));

	TSServer.AddCommandListToQueue(cmds);
	return 1;
}

//native TSC_MoveChannelBelowChannel(channelname[], parentchannelname[]);
cell AMX_NATIVE_CALL native_TSC_MoveChannelBelowChannel(AMX* amx, cell* params) {
	char *TmpParam = NULL;

	amx_StrParam(amx, params[1], TmpParam);
	string ChannelName(TmpParam);
	TSServer.EscapeString(ChannelName);

	amx_StrParam(amx, params[2], TmpParam);
	string ParentChannelName(TmpParam);
	TSServer.EscapeString(ParentChannelName);

	
	CommandList *cmds = new CommandList;

	string CmdStr("channelfind pattern=");
	CmdStr.append(ChannelName);
	cmds->push(new CCommand(CmdStr, "cid"));

	CmdStr.assign("channelfind pattern=");
	CmdStr.append(ParentChannelName);
	cmds->push(new CCommand(CmdStr, "cid"));

	cmds->push(new CCommand("channeledit cid=<1> channel_order=<2>"));

	TSServer.AddCommandListToQueue(cmds);
	return 1;
}

//native TSC_SetClientChannelGroup(uid[], groupid, channelname[]);
cell AMX_NATIVE_CALL native_TSC_SetClientChannelGroup(AMX* amx, cell* params) {
	char *TmpParam = NULL;

	amx_StrParam(amx, params[1], TmpParam);
	string UID(TmpParam);
	TSServer.EscapeString(UID);

	amx_StrParam(amx, params[3], TmpParam);
	string ChannelName(TmpParam);
	TSServer.EscapeString(ChannelName);
	
	
	CommandList *cmds = new CommandList;

	string CmdStr("channelfind pattern=");
	CmdStr.append(ChannelName);
	cmds->push(new CCommand(CmdStr, "cid"));

	char FormatTmp[256];
	sprintf(FormatTmp, "clientdbfind pattern=%s -uid", UID.c_str());
	cmds->push(new CCommand(FormatTmp, "cldbid"));

	sprintf(FormatTmp, "setclientchannelgroup cgid=%d cid=<1> cldbid=<2>", (int)params[2]);
	cmds->push(new CCommand(FormatTmp));

	TSServer.AddCommandListToQueue(cmds);
	return 1;
}

//native TSC_AddClientToServerGroup(uid[], groupid);
cell AMX_NATIVE_CALL native_TSC_AddClientToServerGroup(AMX* amx, cell* params) {
	char *TmpParam = NULL;
	amx_StrParam(amx, params[1], TmpParam);
	string UID(TmpParam);
	TSServer.EscapeString(UID);
	

	CommandList *cmds = new CommandList;

	char FormatTmp[256];
	sprintf(FormatTmp, "clientdbfind pattern=%s -uid", UID.c_str());
	cmds->push(new CCommand(FormatTmp, "cldbid"));

	sprintf(FormatTmp, "servergroupaddclient sgid=%d cldbid=<1>", (int)params[2]);
	cmds->push(new CCommand(FormatTmp));

	TSServer.AddCommandListToQueue(cmds);
	return 1;
}

//native TSC_RemoveClientFromServerGroup(uid[], groupid);
cell AMX_NATIVE_CALL native_TSC_RemoveClientFromServerGroup(AMX* amx, cell* params) {
	char *TmpParam = NULL;
	amx_StrParam(amx, params[1], TmpParam);
	string UID(TmpParam);
	TSServer.EscapeString(UID);

	
	CommandList *cmds = new CommandList;

	char FormatTmp[256];
	sprintf(FormatTmp, "clientdbfind pattern=%s -uid", UID.c_str());
	cmds->push(new CCommand(FormatTmp, "cldbid"));

	sprintf(FormatTmp, "servergroupdelclient sgid=%d cldbid=<1>", (int)params[2]);
	cmds->push(new CCommand(FormatTmp));

	TSServer.AddCommandListToQueue(cmds);
	return 1;
}


//native TSC_KickClient(uid[], kicktype, reason[]);
cell AMX_NATIVE_CALL native_TSC_KickClient(AMX* amx, cell* params) {
	if(params[2] != 1 && params[2] != 2)
		return -1;

	int KickReasonID;
	switch(params[2]) {
	case KICK_TYPE_CHANNEL:
		KickReasonID = 4;
		break;
	case KICK_TYPE_SERVER:
		KickReasonID = 5;
		break;
	default:
		return 0;
	}

	char *TmpParam = NULL;

	amx_StrParam(amx, params[1], TmpParam);
	string UID(TmpParam);
	TSServer.EscapeString(UID);

	amx_StrParam(amx, params[3], TmpParam);
	string ReasonMsg(TmpParam);
	TSServer.EscapeString(ReasonMsg);
	

	CommandList *cmds = new CommandList;
	
	CCommand *cmd1 = new CCommand("clientlist -uid", "clid");
	cmd1->MFind = "client_unique_identifier=";
	cmd1->MFind.append(UID);
	cmds->push(cmd1);

	char FormatTmp[256];
	sprintf(FormatTmp, "clientkick clid=<1> reasonid=%d reasonmsg=%s", KickReasonID, ReasonMsg.c_str());
	cmds->push(new CCommand(FormatTmp));

	TSServer.AddCommandListToQueue(cmds);
	return 1;
}

//native TSC_BanClient(uid[], seconds, reason[]);
cell AMX_NATIVE_CALL native_TSC_BanClient(AMX* amx, cell* params) {
	char *TmpParam = NULL;

	amx_StrParam(amx, params[1], TmpParam);
	string UID(TmpParam);
	TSServer.EscapeString(UID);

	amx_StrParam(amx, params[3], TmpParam);
	string ReasonMsg(TmpParam);
	TSServer.EscapeString(ReasonMsg);
	

	CommandList *cmds = new CommandList;
	
	char FormatTmp[256];
	sprintf(FormatTmp, "banadd uid=%s time=%d banreason=%s", UID.c_str(), (int)params[2], ReasonMsg.c_str());
	cmds->push(new CCommand(FormatTmp));

	TSServer.AddCommandListToQueue(cmds);
	return 1;
}

//native TSC_MoveClient(uid[], channelname[]);
cell AMX_NATIVE_CALL native_TSC_MoveClient(AMX* amx, cell* params) {
	char *TmpParam = NULL;

	amx_StrParam(amx, params[1], TmpParam);
	string UID(TmpParam);
	TSServer.EscapeString(UID);

	amx_StrParam(amx, params[2], TmpParam);
	string ChannelName(TmpParam);
	TSServer.EscapeString(ChannelName);
	

	CommandList *cmds = new CommandList;
	
	CCommand *cmd1 = new CCommand("clientlist -uid", "clid");
	cmd1->MFind = "client_unique_identifier=";
	cmd1->MFind.append(UID);
	cmds->push(cmd1);

	char FormatTmp[128];
	sprintf(FormatTmp, "channelfind pattern=%s", ChannelName.c_str());
	cmds->push(new CCommand(FormatTmp, "cid"));

	cmds->push(new CCommand("clientmove clid=<1> cid=<2>"));

	TSServer.AddCommandListToQueue(cmds);
	return 1;
}

//native TSC_ToggleClientTalkAbility(uid[], bool:toggle);
cell AMX_NATIVE_CALL native_TSC_ToggleClientTalkAbility(AMX* amx, cell* params) {
	char *TmpParam = NULL;
	amx_StrParam(amx, params[1], TmpParam);
	string UID(TmpParam);
	TSServer.EscapeString(UID);


	CommandList *cmds = new CommandList;

	CCommand *cmd1 = new CCommand("clientlist -uid", "clid");
	cmd1->MFind = "client_unique_identifier=";
	cmd1->MFind.append(UID);
	cmds->push(cmd1);

	char FormatTmp[128];
	sprintf(FormatTmp, "clientedit clid=<1> client_is_talker=%d", (int)params[2]);
	cmds->push(new CCommand(FormatTmp, "cid"));

	TSServer.AddCommandListToQueue(cmds);
	return 1;
}


