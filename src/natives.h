#pragma once

#ifndef INC_NATIVES_H
#define INC_NATIVES_H

cell AMX_NATIVE_CALL native_TSC_Connect(AMX* amx, cell* params);
cell AMX_NATIVE_CALL native_TSC_Disconnect(AMX* amx, cell* params);
cell AMX_NATIVE_CALL native_TSC_Login(AMX* amx, cell* params);

cell AMX_NATIVE_CALL native_TSC_SetActiveVServer(AMX* amx, cell* params);

cell AMX_NATIVE_CALL native_TSC_CreateChannel(AMX* amx, cell* params);
cell AMX_NATIVE_CALL native_TSC_DeleteChannel(AMX* amx, cell* params); //test
cell AMX_NATIVE_CALL native_TSC_GetChannelIDByName(AMX* amx, cell* params);
cell AMX_NATIVE_CALL native_TSC_SetChannelName(AMX* amx, cell* params);
cell AMX_NATIVE_CALL native_TSC_SetChannelDescription(AMX* amx, cell* params);
cell AMX_NATIVE_CALL native_TSC_SetChannelType(AMX* amx, cell* params);
cell AMX_NATIVE_CALL native_TSC_SetChannelPassword(AMX* amx, cell* params);
cell AMX_NATIVE_CALL native_TSC_SetChannelTalkPower(AMX* amx, cell* params);
cell AMX_NATIVE_CALL native_TSC_SetChannelUserLimit(AMX* amx, cell* params);

cell AMX_NATIVE_CALL native_TSC_GetClientUIDByName(AMX* amx, cell* params);
cell AMX_NATIVE_CALL native_TSC_KickClient(AMX* amx, cell* params);
cell AMX_NATIVE_CALL native_TSC_BanClient(AMX* amx, cell* params);

cell AMX_NATIVE_CALL native_TSC_SetTimeoutTime(AMX* amx, cell* params);
#endif

/*

native TSC_Connect(ip[]);
native TSC_Disconnect();
native TSC_Login(user[], pass[]);

native TSC_SetActiveVServer(port[]);

native TSC_CreateChannel(channelname[]);
native TSC_DeleteChannel(channelid);
native TSC_GetChannelIDByName(channelname[]);
native TSC_SetChannelName(channelid, channelname[]);
native TSC_SetChannelDescription(channelid, channeldesc[]);
native TSC_SetChannelType(channelid, channeltype);
native TSC_SetChannelPassword(channelid, channelpasswd[]);
native TSC_SetChannelTalkPower(channelid, talkpower);
native TSC_SetChannelUserLimit(channelid, maxuser);

native TSC_GetClientUIDByName(name[]);
native TSC_KickClient(uid, kicktype, reason[]);
native TSC_BanClient(uid, seconds, reason[]);

*/