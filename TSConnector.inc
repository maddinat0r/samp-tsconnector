/* Teamspeak3 Connector
 * Version 1.0
 * made by maddinat0r
 */

#if defined TSConnector_included
	#endinput
#endif
#define TSConnector_included


enum E_CHANNEL_TYPE
{
	CHANNEL_TYPE_INVALID,
	CHANNEL_TYPE_TEMPORARY,
	CHANNEL_TYPE_SEMI_PERMANENT,
	CHANNEL_TYPE_PERMANENT
};

enum E_KICK_TYPE
{
	KICK_TYPE_INVALID,
	KICK_TYPE_CHANNEL,
	KICK_TYPE_SERVER
};


native TSC_Connect(user[], pass[], ip[], port = 9987, serverquery_port = 10011, bool:wait = true);
native TSC_Disconnect();
native TSC_ChangeNickname(nickname[]);

native TSC_CreateChannel(channelname[]);
native TSC_DeleteChannel(channelid);
native TSC_SetChannelName(channelid, channelname[]);
native TSC_GetChannelName(channelid, dest[], maxlen = sizeof(dest));
native TSC_SetChannelDescription(channelid, desc[]);
native TSC_SetChannelType(channelid, type);
native TSC_GetChannelType(channelid);
native TSC_SetChannelPassword(channelid, password[]);
native TSC_HasChannelPassword(channelid);
//native TSC_SetChannelTalkPower(channelname[], talkpower);
native TSC_SetChannelUserLimit(channelid, maxusers);
native TSC_GetChannelUserLimit(channelid);
native TSC_SetChannelParentId(channelid, parentchannelid);
native TSC_GetChannelParentId(channelid);
native TSC_SetChannelOrderId(channelid, upperchannelid);
native TSC_GetChannelOrderId(channelid);
	
/*
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
*/

forward TSC_OnConnect();

forward TSC_OnChannelCreated(channelid);
forward TSC_OnChannelDeleted(channelid);
forward TSC_OnChannelReorder(channelid, orderid);
forward TSC_OnChannelMoved(channelid, parentid, orderid);
forward TSC_OnChannelRenamed(channelid, name[]);
forward TSC_OnChannelPasswordEdited(channelid, bool:ispassworded, bool:passwordchanged);
forward TSC_OnChannelTypeChanged(channelid, type);
forward TSC_OnChannelSetDefault(channelid);
forward TSC_OnChannelMaxClientsChanged(channelid, maxclients);
