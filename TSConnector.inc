#if defined TSConnector_included
	#endinput
#endif
#define TSConnector_included


enum {
	KICK_TYPE_CHANNEL = 1,
	KICK_TYPE_SERVER
};


native TSC_Connect(ip[], port[]);
/*Parameters:
	ip[]: the IP of the Teamspeak server
	port[]: port of the virtual server
	
Usage:
	Establishes a connection to the Teamspeak server.
	This native has to be called before all other natives.
	
Return: 
	0 on success,
	-1 on failure
*/

native TSC_Disconnect();
/*Parameters:
	none
	
Usage:
	Disconnects from the current Teamspeak server.
	
Return:
	none
*/

native TSC_Login(user[], pass[], nickname[]);
/*Parameters:
	user[]: the username of the Serverquery account
	pass[]: the password of the Serverquery account
	nickname[]: the nickname of the Serverquery account
	
Usage:
	Logs into the Teamspeak server with the Serverquery account details in.
	All of the natives won't work without a valid login, because they need admin rights to get executed.
	
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
	none
*/

native TSC_DeleteChannel(channelname[]);
/*Parameters:
	channelname[]: name of a Teamspeak channel
	
Usage:
	Deletes a channel and kicks all clients out of the deleted channel if there were any.
	
Return:
	none
*/


native TSC_SetChannelName(channelname[], newchannelname[]);
/*Parameters:
	channelname[]: channel name of a Teamspeak channel
	channelname[]: new channel name to set

Usage:
	Renames a channel.
	
Return:
	none
*/

native TSC_SetChannelDescription(channelname[], channeldesc[]);
/*Parameters:
	channelname[]: channel name of a Teamspeak channel
	channeldesc[]: new channel description to set

Usage:
	Changes a channels description.
	
Return:
	none
*/

native TSC_SetChannelPassword(channelname[], channelpasswd[]);
/*Parameters:
	channelname[]: channel name of a Teamspeak channel
	channelpasswd[]: new channel password to set

Usage:
	Sets a channels password. Use a empty string to remove the password.
	
Return:
	none
*/

native TSC_SetChannelTalkPower(channelname[], talkpower);
/*Parameters:
	channelname[]: channel name of a Teamspeak channel
	talkpower: new needed talkpower

Usage:
	Sets the talkpower needed in this channel.
	
Return:
	none
*/

native TSC_SetChannelSubChannel(channelname[], parentchannelname[]);
/*Parameters:
	channelname[]: channel name of a Teamspeak channel
	parentchannelname[]: parent channel name of the Teamspeak channel

Usage:
	Puts the channel specified with channelname in the channel specified by parentchannelname as sub-channel.
	
Return:
	none
*/

native TSC_MoveChannelBelowChannel(channelname[], parentchannelname[]);
/*Parameters:
	channelname[]: channel name of a Teamspeak channel
	parentchannelname[]: parent channel name of the Teamspeak channel

Usage:
	Moves a channel specified with channelname under a channel specified with parentchannelname.
	
Return:
	none
*/


	

native TSC_KickClient(uid[], kicktype, reason[]);
/*Parameters:
	uid[]: unique ID of a Teamspeak user
	kicktype: type of kick
	reason[]: reason message

Usage:
	Kicks a client from a channel if kicktype = 'KICK_TYPE_CHANNEL' or
	from the server if kicktype = 'KICK_TYPE_SERVER'.
	
Return:
	none
*/

native TSC_BanClient(uid[], seconds, reason[]);
/*Parameters:
	uid[]: unique ID of a Teamspeak user
	seconds: amount of seconds the ban will last
	reason[]: reason message

Usage:
	Bans a client from the server.
	
Return:
	none
*/

native TSC_MoveClient(uid[], channelname[]);
/*Parameters:
	uid[]: unique ID of a Teamspeak user
	channelname[]: channel name of a Teamspeak channel

Usage:
	Moves a client to the specified channel.
	
Return:
	none
*/


native TSC_SetClientChannelGroup(uid[], groupid, channelname[]);
/*Parameters:
	uid[]: unique ID of a Teamspeak user
	groupid: group ID of a Teamspeak channel group
	channelname[]: channel name of a Teamspeak channel

Usage:
	Sets a clients channel group for the desired channel.
	
Return:
	none
*/

native TSC_AddClientToServerGroup(uid[], groupid);
/*Parameters:
	uid[]: unique ID of a Teamspeak user
	groupid: group ID of a Teamspeak server group

Usage:
	Adds a client to a server group.
	
Return:
	none
*/

native TSC_RemoveClientFromServerGroup(uid[], groupid);
/*Parameters:
	uid[]: unique ID of a Teamspeak user
	groupid: group ID of a Teamspeak server group

Usage:
	Removes a client from a server group.
	
Return:
	none
*/

native TSC_ToggleClientTalkAbility(uid[], bool:toggle);
/*Parameters:
	uid[]: unique ID of a Teamspeak user
	toggle: talk ability status

Usage:
	Gives a client the ability to talk if his talk power is too low
	and removes it.
	
Return:
	none
*/


native TSC_PokeClient(uid[], msg[]);
/*Parameters:
	uid[]: unique ID of a Teamspeak user
	msg[]: message the user will recieve

Usage:
	Pokes a client.
	
Return:
	none
*/




native TSC_SendClientMessage(uid[], msg[]);
/*Parameters:
	uid[]: unique ID of a Teamspeak user
	msg[]: message the user will recieve

Usage:
	Sends a message to a specified client.
	
Return:
	none
*/

native TSC_SendChannelMessage(channelname[], msg[]);
/*Parameters:
	channelname[]: name of the target channel
	msg[]: message the channel will recieve

Usage:
	Sends a message to a specified channel.
	ATTENTION: messages to whole channels can only be sent to the channel
	where the serverquery client is currently in! See 'TSC_OnClientChannelText'
	for more informations.
	
Return:
	none
*/

native TSC_SendServerMessage(msg[]);
/*Parameters:
	msg[]: message the current virtual server will recieve

Usage:
	Sends a message to the current virtual server.
	
Return:
	none
*/



forward TSC_OnClientConnect(nickname[], uid[]);
/*Parameters:
	nickname[]: nickname of the Teamspeak user
	uid[]: unique ID of the Teamspeak user

Event:
	Called when client joins the server.
*/

forward TSC_OnClientDisconnect(nickname[], uid[]);
/*Parameters:
	nickname[]: nickname of the Teamspeak user
	uid[]: unique ID of the Teamspeak user

Event:
	Called when client leaves the server.
*/


forward TSC_OnClientChannelMove(nickname[], uid[], channelname[]);
/*Parameters:
	nickname[]: nickname of the Teamspeak user
	uid[]: unique ID of the Teamspeak user
	channelname[]: name of the channel to which the user is moving

Event:
	Called when client moves/switches to a channel
*/

forward TSC_OnClientChannelText(nickname[], uid[], channelname[], msg[]);
/*Parameters:
	nickname[]: nickname of the Teamspeak user
	uid[]: unique ID of the Teamspeak user
	channelname[]: channel name
	msg[]: message

Event:
	Called when client sends a message in a channel.
	!WARNING! Only works for ONE channel! (the channel where the serverquery is, by default the
	default channel). You can change the channel by moving the serverquery user to the desired channel
	with 'TSC_MoveClient("serveradmin", "Target Channel")'
*/

forward TSC_OnClientServerText(nickname[], uid[], msg[]);
/*Parameters:
	nickname[]: nickname of the Teamspeak user
	uid[]: unique ID of the Teamspeak user
	msg[]: message

Event:
	Called when client sends a message in the server.
*/
