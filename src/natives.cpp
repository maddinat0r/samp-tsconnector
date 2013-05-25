#include "main.h"

#include "CTeamspeak.h"

cell AMX_NATIVE_CALL native_TSC_Connect(AMX* amx, cell* params) {
	return CTeamspeak::Connect(AMX_GetString(amx, params[1]));;
}

cell AMX_NATIVE_CALL native_TSC_Disconnect(AMX* amx, cell* params) {
	CTeamspeak::Close();
	return 1;
}

cell AMX_NATIVE_CALL native_TSC_Login(AMX* amx, cell* params) {
	return CTeamspeak::Login(AMX_GetString(amx, params[1]), AMX_GetString(amx, params[2]));
}


cell AMX_NATIVE_CALL native_TSC_SetTimeoutTime(AMX* amx, cell* params) {
	if(params[1] < 0)
		return -1;

	return (cell)CTeamspeak::SetTimeoutTime(params[1]);
}


cell AMX_NATIVE_CALL native_TSC_CreateChannel(AMX* amx, cell* params) {
	stringstream StrBuf;
	string ChannelName = AMX_GetString(amx, params[1]);
	CTeamspeak::EscapeString(&ChannelName);
	StrBuf << "channelcreate channel_name=" << ChannelName << "\n";
	CTeamspeak::Send(StrBuf.str());
	StrBuf.str("");

	int ChannelID = -1, 
		ErrorID = -1;
	if(CTeamspeak::ExpectIntVal("cid", &ChannelID, &ErrorID) == false)
		return -1;
	return (cell)ChannelID;
}

cell AMX_NATIVE_CALL native_TSC_DeleteChannel(AMX* amx, cell* params) {
	stringstream StrBuf;
	StrBuf << "channeldelete cid=" << params[1] << " force=1";
	CTeamspeak::Send(StrBuf.str());
	StrBuf.str("");
	string SendRes;
	if(CTeamspeak::Recv(&SendRes) == SOCKET_ERROR)
		return -1;
	return (cell)CTeamspeak::ParseError(SendRes);
}

cell AMX_NATIVE_CALL native_TSC_SetChannelName(AMX* amx, cell* params) {
	stringstream StrBuf;
	string ChannelName = AMX_GetString(amx, params[2]);
	CTeamspeak::EscapeString(&ChannelName);
	StrBuf << "channeledit cid=" << params[1] << " channel_name=" << ChannelName;
	CTeamspeak::Send(StrBuf.str());
	StrBuf.str("");
	string SendRes;
	if(CTeamspeak::Recv(&SendRes) == SOCKET_ERROR)
		return -1;
	return (cell)CTeamspeak::ParseError(SendRes);
}

cell AMX_NATIVE_CALL native_TSC_SetChannelDescription(AMX* amx, cell* params) {
	stringstream StrBuf;
	string ChannelDesc = AMX_GetString(amx, params[2]);
	CTeamspeak::EscapeString(&ChannelDesc);
	StrBuf << "channeledit cid=" << params[1] << " channel_description=" << ChannelDesc;
	CTeamspeak::Send(StrBuf.str());
	StrBuf.str("");
	string SendRes;
	if(CTeamspeak::Recv(&SendRes) == SOCKET_ERROR)
		return -1;
	return (cell)CTeamspeak::ParseError(SendRes);
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

cell AMX_NATIVE_CALL native_TSC_SetChannelPassword(AMX* amx, cell* params) {
	stringstream StrBuf;
	string ChannelPasswd = AMX_GetString(amx, params[2]);
	CTeamspeak::EscapeString(&ChannelPasswd);
	StrBuf << "channeledit cid=" << params[1] << " channel_password=" << ChannelPasswd;
	CTeamspeak::Send(StrBuf.str());
	StrBuf.str("");
	string SendRes;
	if(CTeamspeak::Recv(&SendRes) == SOCKET_ERROR)
		return -1;
	return (cell)CTeamspeak::ParseError(SendRes);
}

cell AMX_NATIVE_CALL native_TSC_SetChannelTalkPower(AMX* amx, cell* params) {
	stringstream StrBuf;
	StrBuf << "channeledit cid=" << params[1] << " channel_needed_talk_power=" << params[2];
	CTeamspeak::Send(StrBuf.str());
	StrBuf.str("");
	string SendRes;
	if(CTeamspeak::Recv(&SendRes) == SOCKET_ERROR)
		return -1;
	return (cell)CTeamspeak::ParseError(SendRes);
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

	stringstream StrBuf;



	StrBuf.str("");


}

	





cell AMX_NATIVE_CALL native_TSC_SetClientChannelGroup(AMX* amx, cell* params) {
	if(params[1] <= 0 || params[2] <= 0 || params[3] <= 0)
		return -1;
	
	stringstream StrBuf;
	StrBuf << "setclientchannelgroup cgid=" << params[2] << " cid=" << params[3] << " cldbid=" << params[1];
	CTeamspeak::Send(StrBuf.str());
	StrBuf.str("");
	string SendRes;
	if(CTeamspeak::Recv(&SendRes) == SOCKET_ERROR)
		return -1;
	return (cell)CTeamspeak::ParseError(SendRes);
}

cell AMX_NATIVE_CALL native_TSC_AddClientToServerGroup(AMX* amx, cell* params) {
	if(params[1] <= 0 || params[2] <= 0)
		return -1;
	
	stringstream StrBuf;
	StrBuf << "servergroupaddclient sgid=" << params[2] << " cldbid=" << params[1];
	CTeamspeak::Send(StrBuf.str());
	StrBuf.str("");
	string SendRes;
	if(CTeamspeak::Recv(&SendRes) == SOCKET_ERROR)
		return -1;
	return (cell)CTeamspeak::ParseError(SendRes);
}

cell AMX_NATIVE_CALL native_TSC_RemoveClientFromServerGroup(AMX* amx, cell* params) {
	if(params[1] <= 0 || params[2] <= 0)
		return -1;
	
	stringstream StrBuf;
	StrBuf << "servergroupdelclient sgid=" << params[2] << " cldbid=" << params[1];
	CTeamspeak::Send(StrBuf.str());
	StrBuf.str("");
	string SendRes;
	if(CTeamspeak::Recv(&SendRes) == SOCKET_ERROR)
		return -1;
	return (cell)CTeamspeak::ParseError(SendRes);
}


	StrBuf.str("");


}

	stringstream StrBuf;
	StrBuf.str("");

}


	StrBuf.str("");

}


cell AMX_NATIVE_CALL native_TSC_KickClient(AMX* amx, cell* params) {
	if(params[1] <= 0 || (params[2] != 1 && params[2] != 2))
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
		return -1;
	}
	string ReasonMsg = AMX_GetString(amx, params[3]);
	CTeamspeak::EscapeString(&ReasonMsg);
	stringstream StrBuf;
	StrBuf << "clientkick clid=" << params[1] << " reasonid=" << KickReasonID << " reasonmsg=" << ReasonMsg;
	CTeamspeak::Send(StrBuf.str());
	StrBuf.str("");
	string SendRes;
	if(CTeamspeak::Recv(&SendRes) == SOCKET_ERROR)
		return -1;
	return (cell)CTeamspeak::ParseError(SendRes);
}

cell AMX_NATIVE_CALL native_TSC_BanClient(AMX* amx, cell* params) {
	if(params[1] <= 0)
		return -1;

	string ReasonMsg = AMX_GetString(amx, params[3]);
	CTeamspeak::EscapeString(&ReasonMsg);
	stringstream StrBuf;
	StrBuf << "banclient clid=" << params[1] << " time=" << params[2] << " banreason=" << ReasonMsg;
	CTeamspeak::Send(StrBuf.str());
	StrBuf.str("");
	string SendRes;
	if(CTeamspeak::Recv(&SendRes) == SOCKET_ERROR)
		return -1;
	return (cell)CTeamspeak::ParseError(SendRes);
}

cell AMX_NATIVE_CALL native_TSC_MoveClient(AMX* amx, cell* params) {
	if(params[1] <= 0 || params[2] <= 0)
		return -1;
	
	stringstream StrBuf;
	StrBuf << "clientmove clid=" << params[1] << " cid=" << params[2];
	CTeamspeak::Send(StrBuf.str());
	StrBuf.str("");
	string SendRes;
	if(CTeamspeak::Recv(&SendRes) == SOCKET_ERROR)
		return -1;
	return (cell)CTeamspeak::ParseError(SendRes);
}

cell AMX_NATIVE_CALL native_TSC_SetChannelSubChannel(AMX* amx, cell* params) {
	int ChannelID = params[1],
		ParentChannelID = params[2];
	if(ChannelID <= 0 || ParentChannelID < 0)
		return -1;

	stringstream StrBuf;
	StrBuf << "channelmove cid=" << ChannelID << " cpid=" << ParentChannelID;
	CTeamspeak::Send(StrBuf.str());
	StrBuf.str("");
	string SendRes;
	if(CTeamspeak::Recv(&SendRes) == SOCKET_ERROR)
		return -1;
	return (cell)CTeamspeak::ParseError(SendRes);
}

cell AMX_NATIVE_CALL native_TSC_MoveChannelBelowChannel(AMX* amx, cell* params) {
	int ChannelID = params[1],
		ParentChannelID = params[2];
	if(ChannelID <= 0 || ParentChannelID < 0)
		return -1;
	
	stringstream StrBuf;
	StrBuf << "channeledit cid=" << ChannelID << " channel_order=" << ParentChannelID;
	CTeamspeak::Send(StrBuf.str());
	StrBuf.str("");
	string SendRes;
	if(CTeamspeak::Recv(&SendRes) == SOCKET_ERROR)
		return -1;
	return (cell)CTeamspeak::ParseError(SendRes);
}


	


	


	StrBuf.str("");

