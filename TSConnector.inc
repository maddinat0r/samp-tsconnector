/* Teamspeak3 Connector
 * Version 1.0
 * made by maddinat0r
 */

#if defined tsconnector_included
	#endinput
#endif
#define tsconnector_included


enum TSC_CHANNELTYPE
{
	INVALID,
	TEMPORARY,
	SEMI_PERMANENT,
	PERMANENT
};

enum TSC_KICKTYPE
{
	INVALID,
	CHANNEL,
	SERVER
};


//server functions
native TSC_Connect(user[], pass[], ip[], port = 9987, serverquery_port = 10011, bool:wait = true);
native TSC_Disconnect();
native TSC_ChangeNickname(nickname[]);
native TSC_SendServerMessage(msg[]);


//channel functions
native TSC_CreateChannel(channelname[]);
native TSC_DeleteChannel(channelid);
native TSC_FindChannel(channelname[]);
native TSC_IsValidChannel(channelid);
native TSC_SetChannelName(channelid, channelname[]);
native TSC_GetChannelName(channelid, dest[], maxlen = sizeof(dest));
native TSC_SetChannelDescription(channelid, desc[]);
native TSC_SetChannelType(channelid, TSC_CHANNELTYPE:type);
native TSC_CHANNELTYPE:TSC_GetChannelType(channelid);
native TSC_SetChannelPassword(channelid, password[]);
native TSC_HasChannelPassword(channelid);
native TSC_SetChannelRequiredTP(channelid, talkpower);
native TSC_GetChannelRequiredTP(channelid);
native TSC_SetChannelUserLimit(channelid, maxusers);
native TSC_GetChannelUserLimit(channelid);
native TSC_SetChannelParentId(channelid, parentchannelid);
native TSC_GetChannelParentId(channelid);
native TSC_SetChannelOrderId(channelid, upperchannelid);
native TSC_GetChannelOrderId(channelid);
native TSC_GetDefaultChannelId();


//client functions
native TSC_FindClient(uid[]);

native TSC_GetClientUid(clientid, dest[], maxlen = sizeof(dest));
native TSC_GetClientDatabaseId(clientid);
native TSC_GetClientChannelId(clientid);

native TSC_KickClient(clientid, TSC_KICKTYPE:kicktype, reasonmsg[] = "");
native TSC_BanClient(clientuid[], seconds, reasonmsg[]);
native TSC_MoveClient(clientid, channelid);

native TSC_SetClientChannelGroup(clientid, groupid, channelid);
native TSC_AddClientToServerGroup(clientid, groupid);
native TSC_RemoveClientFromServerGroup(clientid, groupid);

native TSC_PokeClient(clientid, msg[]);
native TSC_SendClientMessage(clientid, msg[]);




//server callbacks
forward TSC_OnConnect();

//channel callbacks
forward TSC_OnChannelCreated(channelid);
forward TSC_OnChannelDeleted(channelid);
forward TSC_OnChannelReorder(channelid, orderid);
forward TSC_OnChannelMoved(channelid, parentid, orderid);
forward TSC_OnChannelRenamed(channelid, name[]);
forward TSC_OnChannelPasswordEdited(channelid, bool:ispassworded, bool:passwordchanged);
forward TSC_OnChannelTypeChanged(channelid, TSC_CHANNELTYPE:type);
forward TSC_OnChannelSetDefault(channelid);
forward TSC_OnChannelMaxClientsChanged(channelid, maxclients);
forward TSC_OnChannelRequiredTPChanged(channelid, talkpower);

//client callbacks
forward TSC_OnClientConnect(clientid, nickname[]);
forward TSC_OnClientDisconnect(clientid, reasonid, reasonmsg[]);
forward TSC_OnClientMoved(clientid, to_channelid, invokerid);
forward TSC_OnClientServerText(clientid, nickname[], msg[]);
forward TSC_OnClientPrivateText(from_clid, from_nickname[], to_clid, msg[]);
