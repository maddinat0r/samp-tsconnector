/* Teamspeak3 Connector
 * Version 1.1
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

enum TSC_CHANNEL_QUERYDATA
{
	INVALID,
	CHANNEL_TOPIC, //channel_topic [string]
	CHANNEL_DESCRIPTION, //channel_description [string]
	CHANNEL_CODEC, //channel_codec [int]
	CHANNEL_CODEC_QUALITY, //channel_codec_quality [int]
	CHANNEL_FORCED_SILENCE, //channel_forced_silence [int]
	CHANNEL_ICON_ID, //channel_icon_id [int]
	CHANNEL_CODEC_IS_UNENCRYPTED, //channel_codec_is_unencrypted [int]
	CHANNEL_SECONDS_EMPTY //seconds_empty [int]
};

enum TSC_CLIENT_QUERYDATA
{
	INVALID,
	CLIENT_NICKNAME, //client_nickname [string]
	CLIENT_VERSION, //client_version [string]
	CLIENT_PLATFORM, //client_platform [string]
	CLIENT_INPUT_MUTED, //client_input_muted [int]
	CLIENT_OUTPUT_MUTED, //client_output_muted [int]
	CLIENT_INPUT_HARDWARE, //client_input_hardware [int]
	CLIENT_OUTPUT_HARDWARE, //client_output_hardware [int]
	CLIENT_CHANNEL_GROUP_ID, //client_channel_group_id [int]
	CLIENT_SERVER_GROUPS, //client_servergroups [string (list of numbers, separated by a comma)]
	CLIENT_FIRSTCONNECTED, //client_created [int (UTC timestamp of first connection)]
	CLIENT_LASTCONNECTED, //client_lastconnected [int (UTC timestamp of last connection)]
	CLIENT_TOTALCONNECTIONS, //client_totalconnections [int]
	CLIENT_AWAY, //client_away [int]
	CLIENT_AWAY_MESSAGE, //client_away_message [string]
	CLIENT_AVATAR, //client_flag_avatar [string] (CRC checksum?)
	CLIENT_TALK_POWER, //client_talk_power [int]
	CLIENT_TALK_REQUEST, //client_talk_request [int]
	CLIENT_TALK_REQUEST_MSG, //client_talk_request_msg [string]
	CLIENT_IS_TALKER, //client_is_talker [int]
	CLIENT_IS_PRIORITY_SPEAKER, //client_is_priority_speaker [int]
	CLIENT_DESCRIPTION, //client_description [string]
	CLIENT_IS_CHANNEL_COMMANDER, //client_is_channel_commander [int]
	CLIENT_ICON_ID, //client_icon_id [int]
	CLIENT_COUNTRY, //client_country [string]
	CLIENT_IDLE_TIME, //client_idle_time [int] (seconds)
	CLIENT_IS_RECORDING //client_is_recording [int]
};

enum /*teamspeak channel codecs*/ 
{
	CODEC_SPEEX_NARROWBAND = 0, // 0: speex narrowband (mono, 16bit, 8kHz)
	CODEC_SPEEX_WIDEBAND, // 1: speex wideband (mono, 16bit, 16kHz)
	CODEC_SPEEX_ULTRAWIDEBAND, // 2: speex ultra-wideband (mono, 16bit, 32kHz)
	CODEC_CELT_MONO, // 3: celt mono (mono, 16bit, 48kHz)
	CODEC_OPUS_VOICE, //4: opus voice
	CODEC_OPUS_MUSIC //5: opus music
};


//server functions
native TSC_Connect(user[], pass[], hostname[], port = 9987, serverquery_port = 10011);
native TSC_Disconnect();
native TSC_ChangeNickname(nickname[]);
native TSC_SendServerMessage(msg[]);


//data query functions
native TSC_QueryChannelData(channelid, TSC_CHANNEL_QUERYDATA:data, const callback[], const format[] = "", {Float, _}:...);
native TSC_QueryClientData(clientid, TSC_CLIENT_QUERYDATA:data, const callback[], const format[] = "", {Float, _}:...);
native TSC_GetQueriedData(dest[], max_len = sizeof(dest));
native TSC_GetQueriedDataAsInt();


//channel functions
native TSC_CreateChannel(channelname[], TSC_CHANNELTYPE:type = TEMPORARY, maxusers = -1, parentchannelid = -1, upperchannelid = -1, talkpower = 0);
native TSC_DeleteChannel(channelid);
native TSC_GetChannelIdByName(channelname[]);
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
native TSC_GetClientIdByUid(uid[]);
native TSC_GetClientIdByIpAddress(ip[]);

native TSC_GetClientUid(clientid, dest[], maxlen = sizeof(dest));
native TSC_GetClientDatabaseId(clientid);
native TSC_GetClientChannelId(clientid);
native TSC_GetClientIpAddress(clientid, dest[], maxlen = sizeof(dest));

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
