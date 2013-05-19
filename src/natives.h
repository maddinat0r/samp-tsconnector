#pragma once

#ifndef INC_NATIVES_H
#define INC_NATIVES_H

//system natives
cell AMX_NATIVE_CALL native_TSC_Connect(AMX* amx, cell* params);
cell AMX_NATIVE_CALL native_TSC_Disconnect(AMX* amx, cell* params);
cell AMX_NATIVE_CALL native_TSC_Login(AMX* amx, cell* params);

//server natives
cell AMX_NATIVE_CALL native_TSC_SetActiveVServer(AMX* amx, cell* params);
cell AMX_NATIVE_CALL native_TSC_SetTimeoutTime(AMX* amx, cell* params);


//channel natives
cell AMX_NATIVE_CALL native_TSC_CreateChannel(AMX* amx, cell* params);
cell AMX_NATIVE_CALL native_TSC_DeleteChannel(AMX* amx, cell* params);

cell AMX_NATIVE_CALL native_TSC_GetChannelIDByName(AMX* amx, cell* params);
cell AMX_NATIVE_CALL native_TSC_SetChannelName(AMX* amx, cell* params);
cell AMX_NATIVE_CALL native_TSC_SetChannelDescription(AMX* amx, cell* params);
cell AMX_NATIVE_CALL native_TSC_SetChannelType(AMX* amx, cell* params);
cell AMX_NATIVE_CALL native_TSC_SetChannelPassword(AMX* amx, cell* params);
cell AMX_NATIVE_CALL native_TSC_SetChannelTalkPower(AMX* amx, cell* params);
cell AMX_NATIVE_CALL native_TSC_SetChannelUserLimit(AMX* amx, cell* params);

cell AMX_NATIVE_CALL native_TSC_GetChannelName(AMX* amx, cell* params);
cell AMX_NATIVE_CALL native_TSC_GetChannelClientList(AMX* amx, cell* params);


//client natives
cell AMX_NATIVE_CALL native_TSC_KickClient(AMX* amx, cell* params);
cell AMX_NATIVE_CALL native_TSC_BanClient(AMX* amx, cell* params);
cell AMX_NATIVE_CALL native_TSC_MoveClient(AMX* amx, cell* params);

cell AMX_NATIVE_CALL native_TSC_SetClientChannelGroup(AMX* amx, cell* params);
cell AMX_NATIVE_CALL native_TSC_AddClientToServerGroup(AMX* amx, cell* params);
cell AMX_NATIVE_CALL native_TSC_RemoveClientFromServerGroup(AMX* amx, cell* params);

cell AMX_NATIVE_CALL native_TSC_GetClientName(AMX* amx, cell* params);
cell AMX_NATIVE_CALL native_TSC_GetClientIDByName(AMX* amx, cell* params);
cell AMX_NATIVE_CALL native_TSC_GetClientCurrentChannelID(AMX* amx, cell* params);




#endif