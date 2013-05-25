#include "main.h"

#include "CTeamspeak.h"

cell AMX_NATIVE_CALL native_TSC_Connect(AMX* amx, cell* params) {
	return TSServer.Connect(AMX_GetString(amx, params[1]), AMX_GetString(amx, params[2]));
}

cell AMX_NATIVE_CALL native_TSC_Disconnect(AMX* amx, cell* params) {
	TSServer.Disconnect();
	return 1;
}

cell AMX_NATIVE_CALL native_TSC_Login(AMX* amx, cell* params) {
	return TSServer.Login(AMX_GetString(amx, params[1]), AMX_GetString(amx, params[2]), AMX_GetString(amx, params[3]));
}


cell AMX_NATIVE_CALL native_TSC_SetTimeoutTime(AMX* amx, cell* params) {
	if(params[1] <= 0)
		return -1;

	return (cell)TSServer.SetTimeoutTime(params[1]);
}


//native TSC_CreateChannel(channelname[]);
cell AMX_NATIVE_CALL native_TSC_CreateChannel(AMX* amx, cell* params) {
	stringstream StrBuf;
	string ChannelName = AMX_GetString(amx, params[1]);
	TSServer.EscapeString(&ChannelName);

	CommandList *cmds = new CommandList;
	StrBuf << "channelcreate channel_name=" << ChannelName;
	cmds->push(new CCommand(StrBuf.str()));

	TSServer.AddCommandListToQueue(cmds);

	return 1;
}

//native TSC_DeleteChannel(channelname[]);
cell AMX_NATIVE_CALL native_TSC_DeleteChannel(AMX* amx, cell* params) {
	string ChannelName = AMX_GetString(amx, params[1]);
	TSServer.EscapeString(&ChannelName);
	stringstream StrBuf;
	CommandList *cmds = new CommandList;

	StrBuf << "channelfind pattern=" << ChannelName; 
	cmds->push(new CCommand(StrBuf.str(), "cid"));
	StrBuf.clear();
	StrBuf.str("");

	StrBuf << "channeldelete cid=<1> force=1";
	cmds->push(new CCommand(StrBuf.str()));

	TSServer.AddCommandListToQueue(cmds);

	return 1;
}

//native TSC_SetChannelName(channelname[], newname[]);
cell AMX_NATIVE_CALL native_TSC_SetChannelName(AMX* amx, cell* params) {
	stringstream StrBuf;
	string ChannelName = AMX_GetString(amx, params[1]);
	string NewChannelName = AMX_GetString(amx, params[2]);
	TSServer.EscapeString(&ChannelName);
	TSServer.EscapeString(&NewChannelName);

	CommandList *cmds = new CommandList;

	StrBuf << "channelfind pattern=" << ChannelName;
	cmds->push(new CCommand(StrBuf.str(), "cid"));
	StrBuf.clear();
	StrBuf.str("");

	StrBuf << "channeledit cid=<1> channel_name=" << NewChannelName;
	cmds->push(new CCommand(StrBuf.str()));

	TSServer.AddCommandListToQueue(cmds);
	return 1;
}

//native TSC_SetChannelDescription(channelname[], desc[]);
cell AMX_NATIVE_CALL native_TSC_SetChannelDescription(AMX* amx, cell* params) {
	stringstream StrBuf;
	string ChannelName = AMX_GetString(amx, params[1]);
	string Desc = AMX_GetString(amx, params[2]);
	TSServer.EscapeString(&ChannelName);
	TSServer.EscapeString(&Desc);

	CommandList *cmds = new CommandList;

	StrBuf << "channelfind pattern=" << ChannelName;
	cmds->push(new CCommand(StrBuf.str(), "cid"));
	StrBuf.clear();
	StrBuf.str("");

	StrBuf << "channeledit cid=<1> channel_description=" << Desc;
	cmds->push(new CCommand(StrBuf.str()));

	TSServer.AddCommandListToQueue(cmds);
	return 1;
}

//native TSC_SetChannelType(channelname[], type);
/*cell AMX_NATIVE_CALL native_TSC_SetChannelType(AMX* amx, cell* params) {
	stringstream StrBuf;
	string 
		ChannelName = AMX_GetString(amx, params[1]),
		ChannelType;
	TSServer.EscapeString(&ChannelName);

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

	StrBuf << "channelfind pattern=" << ChannelName;
	cmds->push(new CCommand(StrBuf.str(), "cid"));
	StrBuf.clear();
	StrBuf.str("");

	StrBuf << "channeledit cid=<>  " << ChannelType << "=1";
	cmds->push(new CCommand(StrBuf.str()));

	TSServer.AddCommandListToQueue(cmds);
	return 1;
}*/

//native TSC_SetChannelPassword(channelname[], password[]);
cell AMX_NATIVE_CALL native_TSC_SetChannelPassword(AMX* amx, cell* params) {
	stringstream StrBuf;
	string 
		ChannelName = AMX_GetString(amx, params[1]),
		ChannelPasswd = AMX_GetString(amx, params[2]);
	TSServer.EscapeString(&ChannelName);
	TSServer.EscapeString(&ChannelPasswd);

	CommandList *cmds = new CommandList;

	StrBuf << "channelfind pattern=" << ChannelName;
	cmds->push(new CCommand(StrBuf.str(), "cid"));
	StrBuf.clear();
	StrBuf.str("");

	StrBuf << "channeledit cid=<1> channel_password=" << ChannelPasswd;
	cmds->push(new CCommand(StrBuf.str()));

	TSServer.AddCommandListToQueue(cmds);
	return 1;
}

//native TSC_SetChannelTalkPower(channelname[], talkpower);
cell AMX_NATIVE_CALL native_TSC_SetChannelTalkPower(AMX* amx, cell* params) {
	stringstream StrBuf;
	string ChannelName = AMX_GetString(amx, params[1]);
	TSServer.EscapeString(&ChannelName);

	CommandList *cmds = new CommandList;

	StrBuf << "channelfind pattern=" << ChannelName;
	cmds->push(new CCommand(StrBuf.str(), "cid"));
	StrBuf.clear();
	StrBuf.str("");

	StrBuf << "channeledit cid=<1> channel_needed_talk_power=" << params[2];
	cmds->push(new CCommand(StrBuf.str()));

	TSServer.AddCommandListToQueue(cmds);
	return 1;
}

//native TSC_SetChannelUserLimit(channelname[], maxuser);
/*cell AMX_NATIVE_CALL native_TSC_SetChannelUserLimit(AMX* amx, cell* params) {
	if(params[2] < 0)
		params[2] = 0;
	stringstream StrBuf;
	StrBuf << "channeledit cid=" << params[1] << " channel_maxclients=" << params[2];
	TSServer.Send(StrBuf.str());
	StrBuf.str("");
	string SendRes;
	if(TSServer.Recv(&SendRes) == SOCKET_ERROR)
		return -1;
	return (cell)TSServer.ParseError(SendRes);
}*/

//native TSC_SetChannelSubChannel(channelname[], parentchannelname[]);
cell AMX_NATIVE_CALL native_TSC_SetChannelSubChannel(AMX* amx, cell* params) {
	stringstream StrBuf;
	string
		ChannelName = AMX_GetString(amx, params[1]),
		ParentChannelName = AMX_GetString(amx, params[2]);
	TSServer.EscapeString(&ChannelName);
	TSServer.EscapeString(&ParentChannelName);

	
	CommandList *cmds = new CommandList;

	StrBuf << "channelfind pattern=" << ChannelName;
	cmds->push(new CCommand(StrBuf.str(), "cid"));
	StrBuf.clear();
	StrBuf.str("");

	StrBuf << "channelfind pattern=" << ParentChannelName;
	cmds->push(new CCommand(StrBuf.str(), "cid"));
	StrBuf.clear();
	StrBuf.str("");

	StrBuf << "channelmove cid=<1> cpid=<2>";
	cmds->push(new CCommand(StrBuf.str()));

	TSServer.AddCommandListToQueue(cmds);
	return 1;
}

//native TSC_MoveChannelBelowChannel(channelname[], parentchannelname[]);
cell AMX_NATIVE_CALL native_TSC_MoveChannelBelowChannel(AMX* amx, cell* params) {
	stringstream StrBuf;
	string
		ChannelName = AMX_GetString(amx, params[1]),
		ParentChannelName = AMX_GetString(amx, params[2]);
	TSServer.EscapeString(&ChannelName);
	TSServer.EscapeString(&ParentChannelName);
	
	CommandList *cmds = new CommandList;

	StrBuf << "channelfind pattern=" << ChannelName;
	cmds->push(new CCommand(StrBuf.str(), "cid"));
	StrBuf.clear();
	StrBuf.str("");

	StrBuf << "channelfind pattern=" << ParentChannelName;
	cmds->push(new CCommand(StrBuf.str(), "cid"));
	StrBuf.clear();
	StrBuf.str("");

	StrBuf << "channeledit cid=<1> channel_order=<2>";
	cmds->push(new CCommand(StrBuf.str()));

	TSServer.AddCommandListToQueue(cmds);
	return 1;
}

//native TSC_SetClientChannelGroup(uid[], groupid, channelname[]);
cell AMX_NATIVE_CALL native_TSC_SetClientChannelGroup(AMX* amx, cell* params) {
	stringstream StrBuf;
	string
		UID = AMX_GetString(amx, params[1]),
		ChannelName = AMX_GetString(amx, params[3]);
	TSServer.EscapeString(&UID);
	TSServer.EscapeString(&ChannelName);
	
	CommandList *cmds = new CommandList;

	StrBuf << "channelfind pattern=" << ChannelName;
	cmds->push(new CCommand(StrBuf.str(), "cid"));
	StrBuf.clear();
	StrBuf.str("");

	StrBuf << "clientdbfind pattern=" << UID << " -uid";
	cmds->push(new CCommand(StrBuf.str(), "cldbid"));
	StrBuf.clear();
	StrBuf.str("");

	StrBuf << "setclientchannelgroup cgid=" << params[2] << " cid=<1> cldbid=<2>";
	cmds->push(new CCommand(StrBuf.str()));

	TSServer.AddCommandListToQueue(cmds);
	return 1;
}

//native TSC_AddClientToServerGroup(uid[], groupid);
cell AMX_NATIVE_CALL native_TSC_AddClientToServerGroup(AMX* amx, cell* params) {
	stringstream StrBuf;
	string
		UID = AMX_GetString(amx, params[1]);
	TSServer.EscapeString(&UID);
	
	CommandList *cmds = new CommandList;

	StrBuf << "clientdbfind pattern=" << UID << " -uid";
	cmds->push(new CCommand(StrBuf.str(), "cldbid"));
	StrBuf.clear();
	StrBuf.str("");

	StrBuf << "servergroupaddclient sgid=" << params[2] << " cldbid=<1>";
	cmds->push(new CCommand(StrBuf.str()));

	TSServer.AddCommandListToQueue(cmds);
	return 1;
}

//native TSC_RemoveClientFromServerGroup(uid[], groupid);
cell AMX_NATIVE_CALL native_TSC_RemoveClientFromServerGroup(AMX* amx, cell* params) {
	stringstream StrBuf;
	string
		UID = AMX_GetString(amx, params[1]);
	TSServer.EscapeString(&UID);
	
	CommandList *cmds = new CommandList;

	StrBuf << "clientdbfind pattern=" << UID << " -uid";
	cmds->push(new CCommand(StrBuf.str(), "cldbid"));
	StrBuf.clear();
	StrBuf.str("");

	StrBuf << "servergroupdelclient sgid=" << params[2] << " cldbid=<1>";
	cmds->push(new CCommand(StrBuf.str()));

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

	stringstream StrBuf;
	string 
		UID = AMX_GetString(amx, params[1]),
		ReasonMsg = AMX_GetString(amx, params[3]);
	TSServer.EscapeString(&UID);
	TSServer.EscapeString(&ReasonMsg);
	

	CommandList *cmds = new CommandList;
	
	CCommand *cmd1 = new CCommand("clientlist -uid", "clid");
	cmd1->MFind = "client_unique_identifier=";
	cmd1->MFind.append(UID);
	cmds->push(cmd1);

	StrBuf << "clientkick clid=<1> reasonid=" << KickReasonID << " reasonmsg=" << ReasonMsg;
	cmds->push(new CCommand(StrBuf.str()));

	TSServer.AddCommandListToQueue(cmds);
	return 1;
}

//native TSC_BanClient(uid[], seconds, reason[]);
cell AMX_NATIVE_CALL native_TSC_BanClient(AMX* amx, cell* params) {
	stringstream StrBuf;
	string 
		UID = AMX_GetString(amx, params[1]),
		ReasonMsg = AMX_GetString(amx, params[3]);
	TSServer.EscapeString(&UID);
	TSServer.EscapeString(&ReasonMsg);
	

	CommandList *cmds = new CommandList;
	
	StrBuf << "banadd uid=" << UID << " time=" << params[2] << " banreason=" << ReasonMsg;
	cmds->push(new CCommand(StrBuf.str()));

	TSServer.AddCommandListToQueue(cmds);
	return 1;
}

//native TSC_MoveClient(uid[], channelname[]);
cell AMX_NATIVE_CALL native_TSC_MoveClient(AMX* amx, cell* params) {
	stringstream StrBuf;
	string 
		UID = AMX_GetString(amx, params[1]),
		ChannelName = AMX_GetString(amx, params[2]);
	TSServer.EscapeString(&UID);
	TSServer.EscapeString(&ChannelName);
	

	CommandList *cmds = new CommandList;
	
	CCommand *cmd1 = new CCommand("clientlist -uid", "clid");
	cmd1->MFind = "client_unique_identifier=";
	cmd1->MFind.append(UID);
	cmds->push(cmd1);

	StrBuf << "channelfind pattern=" << ChannelName;
	cmds->push(new CCommand(StrBuf.str(), "cid"));
	StrBuf.clear();
	StrBuf.str("");

	StrBuf << "clientmove clid=<1> cid=<2>";
	cmds->push(new CCommand(StrBuf.str()));

	TSServer.AddCommandListToQueue(cmds);
	return 1;
}
