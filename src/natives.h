#pragma once
#ifndef INC_NATIVES_H
#define INC_NATIVES_H


#define AMX_DECLARE_NATIVE(native) \
	cell AMX_NATIVE_CALL native(AMX *amx, cell *params)

#define AMX_DEFINE_NATIVE(native) \
	{#native, Native::native},


namespace Native
{
	//server functions
	AMX_DECLARE_NATIVE(TSC_Connect);
	AMX_DECLARE_NATIVE(TSC_Disconnect);
	AMX_DECLARE_NATIVE(TSC_ChangeNickname);
	AMX_DECLARE_NATIVE(TSC_SendServerMessage);


	//channel functions
	AMX_DECLARE_NATIVE(TSC_CreateChannel);
	AMX_DECLARE_NATIVE(TSC_DeleteChannel);
	AMX_DECLARE_NATIVE(TSC_GetChannelIdByName);
	AMX_DECLARE_NATIVE(TSC_IsValidChannel);
	AMX_DECLARE_NATIVE(TSC_SetChannelName);
	AMX_DECLARE_NATIVE(TSC_GetChannelName);
	AMX_DECLARE_NATIVE(TSC_SetChannelDescription);
	AMX_DECLARE_NATIVE(TSC_SetChannelType);
	AMX_DECLARE_NATIVE(TSC_GetChannelType);
	AMX_DECLARE_NATIVE(TSC_SetChannelPassword);
	AMX_DECLARE_NATIVE(TSC_HasChannelPassword);
	AMX_DECLARE_NATIVE(TSC_SetChannelRequiredTP);
	AMX_DECLARE_NATIVE(TSC_GetChannelRequiredTP);
	AMX_DECLARE_NATIVE(TSC_SetChannelUserLimit);
	AMX_DECLARE_NATIVE(TSC_GetChannelUserLimit);
	AMX_DECLARE_NATIVE(TSC_SetChannelParentId);
	AMX_DECLARE_NATIVE(TSC_GetChannelParentId);
	AMX_DECLARE_NATIVE(TSC_SetChannelOrderId);
	AMX_DECLARE_NATIVE(TSC_GetChannelOrderId);
	AMX_DECLARE_NATIVE(TSC_GetDefaultChannelId);

	
	//client functions
	AMX_DECLARE_NATIVE(TSC_FindClient);

	AMX_DECLARE_NATIVE(TSC_GetClientUid);
	AMX_DECLARE_NATIVE(TSC_GetClientDatabaseId);
	AMX_DECLARE_NATIVE(TSC_GetClientChannelId);

	AMX_DECLARE_NATIVE(TSC_KickClient);
	AMX_DECLARE_NATIVE(TSC_BanClient);
	AMX_DECLARE_NATIVE(TSC_MoveClient);

	AMX_DECLARE_NATIVE(TSC_SetClientChannelGroup);
	AMX_DECLARE_NATIVE(TSC_AddClientToServerGroup);
	AMX_DECLARE_NATIVE(TSC_RemoveClientFromServerGroup);

	AMX_DECLARE_NATIVE(TSC_PokeClient);
	AMX_DECLARE_NATIVE(TSC_SendClientMessage);
}


#endif // INC_NATIVES_H
