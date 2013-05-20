#if defined TSConnector_included
	#endinput
#endif
#define TSConnector_included


enum {
	CHANNEL_TYPE_PERMANENT = 1,
	CHANNEL_TYPE_SEMI_PERMANENT,
	CHANNEL_TYPE_TEMPORARY
};

enum {
	KICK_TYPE_CHANNEL = 1,
	KICK_TYPE_SERVER
};


native TSC_Connect(ip[]);
native TSC_Disconnect();
native TSC_Login(user[], pass[]);

native TSC_SetActiveVServer(port[]);
native TSC_SetTimeoutTime(milliseconds);


native TSC_CreateChannel(channelname[]);
native TSC_DeleteChannel(channelid);
native TSC_GetChannelIDByName(channelname[]);
native TSC_SetChannelName(channelid, channelname[]);
native TSC_SetChannelDescription(channelid, channeldesc[]);
native TSC_SetChannelType(channelid, channeltype);
native TSC_SetChannelPassword(channelid, channelpasswd[]);
native TSC_SetChannelTalkPower(channelid, talkpower);
native TSC_SetChannelUserLimit(channelid, maxuser);
native TSC_SetChannelSubChannel(channelid, parentchannelid);
native TSC_MoveChannelBelowChannel(channelid, parentchannelid);

native TSC_GetChannelName(channelid, dest[]);
native TSC_GetChannelClientList(channelid, list[]);
native TSC_GetSubChannelListOnChannel(channelid, list[]);
	

native TSC_KickClient(clientid, kicktype, reason[]);
native TSC_BanClient(clientid, seconds, reason[]);
native TSC_MoveClient(clientid, channelid);

native TSC_SetClientChannelGroup(clientdbid, groupid, channelid);
native TSC_AddClientToServerGroup(clientdbid, groupid);
native TSC_RemoveClientFromServerGroup(clientdbid, groupid);

native TSC_GetClientName(clientid, dest[]);
native TSC_GetClientIDByName(name[]);
native TSC_GetClientCurrentChannelID(clientid);
