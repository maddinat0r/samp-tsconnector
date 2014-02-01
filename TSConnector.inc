#if defined TSConnector_included
	#endinput
#endif
#define TSConnector_included


enum E_KICK_TYPE
{
	KICK_TYPE_CHANNEL = 1,
	KICK_TYPE_SERVER
};

enum E_CHANNEL_TYPE 
{
	CHANNEL_TYPE_PERMANENT = 1,
	CHANNEL_TYPE_SEMI_PERMANENT,
	CHANNEL_TYPE_TEMPORARY
};


native TSC_Connect(ip[], port[]);
native TSC_Disconnect();
native TSC_Login(user[], pass[], nickname[]);
native TSC_ChangeNickname(nickname[]);

native TSC_CreateChannel(channelname[]);
native TSC_DeleteChannel(channelname[]);
native TSC_SetChannelName(channelname[], newchannelname[]);
native TSC_SetChannelDescription(channelname[], channeldesc[]);
native TSC_SetChannelType(channelname[], E_CHANNEL_TYPE:type);
native TSC_SetChannelPassword(channelname[], channelpasswd[]);
native TSC_SetChannelTalkPower(channelname[], talkpower);
native TSC_SetChannelSubChannel(channelname[], parentchannelname[]);
native TSC_MoveChannelBelowChannel(channelname[], parentchannelname[]);
	

native TSC_KickClient(uid[], E_KICK_TYPE:kicktype, reason[]);
native TSC_BanClient(uid[], seconds, reason[]);
native TSC_MoveClient(uid[], channelname[]);

native TSC_SetClientChannelGroup(uid[], groupid, channelname[]);
native TSC_AddClientToServerGroup(uid[], groupid);
native TSC_RemoveClientFromServerGroup(uid[], groupid);
native TSC_ToggleClientTalkAbility(uid[], bool:toggle);

native TSC_PokeClient(uid[], msg[]);


native TSC_SendClientMessage(uid[], msg[]);
native TSC_SendChannelMessage(channelname[], msg[]);
native TSC_SendServerMessage(msg[]);


forward TSC_OnClientConnect(nickname[], uid[]);
forward TSC_OnClientDisconnect(nickname[], uid[]);
forward TSC_OnClientChannelMove(nickname[], uid[], channelname[]);
forward TSC_OnClientChannelText(nickname[], uid[], channelname[], msg[]);
forward TSC_OnClientServerText(nickname[], uid[], msg[]);
