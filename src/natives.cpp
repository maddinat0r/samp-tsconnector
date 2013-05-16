#include "main.h"

#include "CTeamspeak.h"




cell AMX_NATIVE_CALL native_TSC_Connect(AMX* amx, cell* params) {
	CTeamspeak::Connect(AMX_GetString(amx, params[1]).c_str(), "10011");
	return 1;
}

cell AMX_NATIVE_CALL native_TSC_Disconnect(AMX* amx, cell* params) {
	CTeamspeak::Close();
	return 1;
}

cell AMX_NATIVE_CALL native_TSC_Login(AMX* amx, cell* params) {
	stringstream StrBuf;
	StrBuf << "login client_login_name=" << AMX_GetString(amx, params[1]) << " client_login_password=" << AMX_GetString(amx, params[2]) <<"\n";
	string SendResult;
	CTeamspeak::Send(StrBuf.str());
	CTeamspeak::Recv(&SendResult);
	return CTeamspeak::ParseError(SendResult);
}


cell AMX_NATIVE_CALL native_TSC_SetActiveVServer(AMX* amx, cell* params) {
	stringstream StrBuf;
	StrBuf << "serveridgetbyport virtualserver_port=" << AMX_GetString(amx, params[1]);
	CTeamspeak::Send(StrBuf.str());
	StrBuf.str("");

	int ServerID, ErrorID;
	CTeamspeak::ExpectIntVal("server_id", &ServerID, &ErrorID);

	if(ServerID && ErrorID >= 0) {
		StrBuf << "use sid=" << ServerID;
		CTeamspeak::Send(StrBuf.str());
		StrBuf.str("");
		string SendRes;
		CTeamspeak::Recv(&SendRes);
		ErrorID = CTeamspeak::ParseError(SendRes);
	}
	return ErrorID;
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
	CTeamspeak::ExpectIntVal("cid", &ChannelID, &ErrorID);
	return (cell)ChannelID;
}

cell AMX_NATIVE_CALL native_TSC_DeleteChannel(AMX* amx, cell* params) {
	stringstream StrBuf;
	StrBuf << "channeldelete cid=" << params[1] << " force=1";
	CTeamspeak::Send(StrBuf.str());
	StrBuf.str("");
	string SendRes;
	CTeamspeak::Recv(&SendRes);
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
	CTeamspeak::Recv(&SendRes);
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
	CTeamspeak::Recv(&SendRes);
	return (cell)CTeamspeak::ParseError(SendRes);
}

cell AMX_NATIVE_CALL native_TSC_SetChannelType(AMX* amx, cell* params) {
	stringstream StrBuf;
	string ChannelType;
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
		return -1;
	}
	StrBuf << "channeledit cid=" << params[1] << " " << ChannelType << "=1";
	CTeamspeak::Send(StrBuf.str());
	StrBuf.str("");
	string SendRes;
	CTeamspeak::Recv(&SendRes);
	return (cell)CTeamspeak::ParseError(SendRes);
}

cell AMX_NATIVE_CALL native_TSC_SetChannelPassword(AMX* amx, cell* params) {
	stringstream StrBuf;
	string ChannelPasswd = AMX_GetString(amx, params[2]);
	CTeamspeak::EscapeString(&ChannelPasswd);
	StrBuf << "channeledit cid=" << params[1] << " channel_password=" << ChannelPasswd;
	CTeamspeak::Send(StrBuf.str());
	StrBuf.str("");
	string SendRes;
	CTeamspeak::Recv(&SendRes);
	return (cell)CTeamspeak::ParseError(SendRes);
}

cell AMX_NATIVE_CALL native_TSC_SetChannelTalkPower(AMX* amx, cell* params) {
	stringstream StrBuf;
	StrBuf << "channeledit cid=" << params[1] << " channel_needed_talk_power=" << params[2];
	CTeamspeak::Send(StrBuf.str());
	StrBuf.str("");
	string SendRes;
	CTeamspeak::Recv(&SendRes);
	return (cell)CTeamspeak::ParseError(SendRes);
}

cell AMX_NATIVE_CALL native_TSC_SetChannelUserLimit(AMX* amx, cell* params) {
	if(params[2] < 0)
		params[2] = 0;
	stringstream StrBuf;
	StrBuf << "channeledit cid=" << params[1] << " channel_maxclients=" << params[2];
	CTeamspeak::Send(StrBuf.str());
	StrBuf.str("");
	string SendRes;
	CTeamspeak::Recv(&SendRes);
	return (cell)CTeamspeak::ParseError(SendRes);
}


cell AMX_NATIVE_CALL native_TSC_FindChannel(AMX* amx, cell* params) {
	stringstream StrBuf;
	string ChannelName = AMX_GetString(amx, params[1]);
	CTeamspeak::EscapeString(&ChannelName);
	StrBuf << "channelfind pattern=" << ChannelName << "\n";
	string tmp = StrBuf.str();
	CTeamspeak::Send(StrBuf.str());
	StrBuf.str("");

	int ChannelID = -1, 
		ErrorID = -1;

	string SendRes;
	CTeamspeak::Recv(&SendRes);
	if(SendRes.find('|') != -1) //more than one channel in result
		return -1;
	if(SendRes.find("error") != -1)
		return -1;//CTeamspeak::ParseError(SendRes);
	ChannelID = CTeamspeak::ParseInteger(SendRes, "cid");
	return (cell)ChannelID;
}
