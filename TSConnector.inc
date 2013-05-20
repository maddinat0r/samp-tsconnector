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
/*Parameters:
	ip[]: the IP of the Teamspeak server
	
Usage:
	Establishes a connection to the Teamspeak server.
	This native has to be called before all other natives.
	
Return: 
	1 on success,
	0 on failure
*/

native TSC_Disconnect();
/*Parameters:
	none
	
Usage:
	Disconnects from the current Teamspeak server.
	
Return:
	none
*/

native TSC_Login(user[], pass[]);
/*Parameters:
	user[]: the username of the Serverquery account
	pass[]: the password of the Serverquery account
	
Usage:
	Logs into the Teamspeak server with the Serverquery account details in.
	Most of the natives won't work without a valid login, because they need admin rights to get executed.
	
Return:
	0 on success,
	-1 on connection failure,
	otherwise a Teamspeak error ID
*/


native TSC_SetActiveVServer(port[]);
/*Parameters:
	port[]: the port of the virtual server
	
Usage:
	Sets the active virtual server. This has to be called after each TSC_Connect to select the 
	virtual server on which all commands are executed.	
	
Return:
	0 on success,
	-1 on connection failure,
	otherwise a Teamspeak error ID
*/

native TSC_SetTimeoutTime(milliseconds);
/*Parameters:
	milliseconds: timeout time in milliseconds
	
Usage:
	Sets the timeout time of the socket connection. If the Teamspeak server takes too long to perform a query,
	the execution of the command will be aborted if the time took longer than the allowed time (the timeout time).
	
Return:
	1 on success,
	0 on failure
*/



native TSC_CreateChannel(channelname[]);
/*Parameters:
	channelname[]: the name of the channel

Usage:
	Creates a new channel with the specified channel name.
	
Return:
	channel ID on success,
	-1 on connection failure
*/

native TSC_DeleteChannel(channelid);
/*Parameters:
	channelid: ID of a Teamspeak channel
	
Usage:
	Deletes a channel specified with the channel ID.
	
Return:
	0 on success,
	-1 on connection failure,
	otherwise a Teamspeak error ID
*/

native TSC_GetChannelIDByName(channelname[]);
/*Parameters:
	channelname[]: name of a Teamspeak channel
	
Usage:
	Returns the channel ID of a channel with the specified channel name. 
	The names have not to be exactly the same, if looking for channel "MyChannel1" and specifying 
	the channel name as "MyChannel", the channel will still be found.
	
Return:
	channel ID on success,
	-1 on connection failure, on Teamspeak error or if more than one channels found with the given name
*/


native TSC_SetChannelName(channelid, channelname[]);
/*Parameters:
	channelid: channel ID of a Teamspeak channel
	channelname[]: new channel name to set

Usage:
	Renames a channel.
	
Return:
	0 on success,
	-1 on connection failure,
	otherwise a Teamspeak error ID
*/

native TSC_SetChannelDescription(channelid, channeldesc[]);
/*Parameters:
	channelid: channel ID of a Teamspeak channel
	channeldesc[]: new channel description to set

Usage:
	Changes a channels description.
	
Return:
	0 on success,
	-1 on connection failure,
	otherwise a Teamspeak error ID
*/

native TSC_SetChannelType(channelid, channeltype);
/*Parameters:
	channelid: channel ID of a Teamspeak channel
	channeltype: type of channel

Usage:
	Sets a channels type. Available types are 
	'CHANNEL_TYPE_PERMANENT', 'CHANNEL_TYPE_SEMI_PERMANENT' and 'CHANNEL_TYPE_TEMPORARY'.
	
Return:
	0 on success,
	-1 on connection failure,
	otherwise a Teamspeak error ID
*/

native TSC_SetChannelPassword(channelid, channelpasswd[]);
/*Parameters:
	channelid: channel ID of a Teamspeak channel
	channelpasswd[]: new channel password to set

Usage:
	Sets a channels password. Use a empty string to remove the password.
	
Return:
	0 on success,
	-1 on connection failure,
	otherwise a Teamspeak error ID
*/

native TSC_SetChannelTalkPower(channelid, talkpower);
/*Parameters:
	channelid: channel ID of a Teamspeak channel
	talkpower: new needed talkpower

Usage:
	Sets the talkpower needed in this channel.
	
Return:
	0 on success,
	-1 on connection failure,
	otherwise a Teamspeak error ID
*/

native TSC_SetChannelUserLimit(channelid, maxuser);
/*Parameters:
	channelid: channel ID of a Teamspeak channel
	maxuser: maximal number of users

Usage:
	Sets the maximal number of users allowed in a channel.
	
Return:
	0 on success,
	-1 on connection failure,
	otherwise a Teamspeak error ID
*/

native TSC_SetChannelSubChannel(channelid, parentchannelid);
/*Parameters:
	channelid: channel ID of a Teamspeak channel
	parentchannelid: parent channel ID of the Teamspeak channel

Usage:
	Puts the channel specified with channelid in the channel specified by parentchannelid as sub-channel.
	
Return:
	0 on success,
	-1 on connection failure,
	otherwise a Teamspeak error ID
*/

native TSC_MoveChannelBelowChannel(channelid, parentchannelid);
/*Parameters:
	channelid: channel ID of a Teamspeak channel
	parentchannelid: parent channel ID of the Teamspeak channel

Usage:
	Moves a channel specified with channelid under a channel specified with parentchannelid.
	
Return:
	0 on success,
	-1 on connection failure,
	otherwise a Teamspeak error ID
*/


native TSC_GetChannelName(channelid, dest[]);
/*Parameters:
	channelid: channel ID of a Teamspeak channel
	dest[]: destination value for the channel name

Usage:
	Gets the channel name of a channel.
	
Return:
	0 on success,
	-1 on connection failure,
	otherwise a Teamspeak error ID
*/

native TSC_GetChannelClientList(channelid, list[]);
/*Parameters:
	channelid: channel ID of a Teamspeak channel
	list[]: destination variable for a array of client ids

Usage:
	Gets a list of client ids currently in a channel.
	
Return:
	-1 on connection failure,
	number of clients on success
*/

native TSC_GetSubChannelListOnChannel(channelid, list[]);
/*Parameters:
	channelid: channel ID of a Teamspeak channel
	list[]: destination variable for a array of channel ids

Usage:
	Gets a list of channel ids which are currently a sub-channel of 
	the channel specified with channelid.
	
Return:
	-1 on connection failure,
	number of channels on success
*/

	

native TSC_KickClient(clientid, kicktype, reason[]);
/*Parameters:
	clientid: client ID of a Teamspeak user
	kicktype: type of kick
	reason[]: reason message

Usage:
	Kicks a client from a channel if kicktype = 'KICK_TYPE_CHANNEL' or
	from the server if kicktype = 'KICK_TYPE_SERVER'.
	
Return:
	0 on success,
	-1 on connection failure,
	otherwise a Teamspeak error ID
*/

native TSC_BanClient(clientid, seconds, reason[]);
/*Parameters:
	clientid: client ID of a Teamspeak user
	seconds: amount of seconds the ban will last
	reason[]: reason message

Usage:
	Bans a client from the server.
	
Return:
	0 on success,
	-1 on connection failure,
	otherwise a Teamspeak error ID
*/

native TSC_MoveClient(clientid, channelid);
/*Parameters:
	clientid: client ID of a Teamspeak user
	channelid: channel ID of a Teamspeak channel

Usage:
	Moves a client to the specified channel.
	
Return:
	0 on success,
	-1 on connection failure,
	otherwise a Teamspeak error ID
*/


native TSC_SetClientChannelGroup(clientdbid, groupid, channelid);
/*Parameters:
	clientdbid: client database ID of a Teamspeak user
	groupid: group ID of a Teamspeak channel group
	channelid: channel ID of a Teamspeak channel

Usage:
	Sets a clients channel group for the desired channel.
	
Return:
	0 on success,
	-1 on connection failure,
	otherwise a Teamspeak error ID
*/

native TSC_AddClientToServerGroup(clientdbid, groupid);
/*Parameters:
	clientdbid: client database ID of a Teamspeak user
	groupid: group ID of a Teamspeak server group

Usage:
	Adds a client to a server group.
	
Return:
	0 on success,
	-1 on connection failure,
	otherwise a Teamspeak error ID
*/

native TSC_RemoveClientFromServerGroup(clientdbid, groupid);
/*Parameters:
	clientdbid: client database ID of a Teamspeak user
	groupid: group ID of a Teamspeak server group

Usage:
	Removes a client from a server group.
	
Return:
	0 on success,
	-1 on connection failure,
	otherwise a Teamspeak error ID
*/


native TSC_GetClientDBIDByUID(uid[]);
/*Parameters:
	uid[]: unique ID of Teamspeak user

Usage:
	Gets a clients database ID by his unique ID.
	
Return:
	-1 on failure,
	database ID on success
*/

native TSC_GetClientDBIDByID(clientid);
/*Parameters:
	clientid: client ID of Teamspeak user

Usage:
	Gets a clients database ID by his ID.
	
Return:
	-1 on failure,
	database ID on success
*/

native TSC_GetClientName(clientid, dest[]);
/*Parameters:
	clientid: client ID of a Teamspeak user
	dest[]: destination variable for the clients username

Usage:
	Gets a clients username.
	
Return:
	0 on success,
	-1 on connection failure,
	otherwise a Teamspeak error ID
*/

native TSC_GetClientIDByName(name[]);
/*Parameters:
	name[]: name of a Teamspeak user

Usage:
	Returns the client ID of a Teamspeak user specified by the name.
	
Return:
	client ID on success,
	-1 on connection failure, on Teamspeak error or if more than one clients found with the given name
*/

native TSC_GetClientCurrentChannelID(clientid);
/*Parameters:
	clientid: client ID of a Teamspeak user

Usage:
	Returns the channel ID which the client is currently in.
	
Return:
	-1 on connection failure,
	channel ID on success
*/

