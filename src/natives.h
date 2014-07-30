#pragma once
#ifndef INC_NATIVES_H
#define INC_NATIVES_H


#define AMX_DECLARE_NATIVE(native) \
	cell AMX_NATIVE_CALL native(AMX *amx, cell *params)

#define AMX_DEFINE_NATIVE(native) \
	{#native, Native::native},


namespace Native
{
	//system natives
	AMX_DECLARE_NATIVE(TSC_Connect);
	AMX_DECLARE_NATIVE(TSC_Disconnect);
	AMX_DECLARE_NATIVE(TSC_ChangeNickname);


	//channel natives
	AMX_DECLARE_NATIVE(TSC_CreateChannel);
	AMX_DECLARE_NATIVE(TSC_DeleteChannel);
	AMX_DECLARE_NATIVE(TSC_SetChannelName);
	AMX_DECLARE_NATIVE(TSC_GetChannelName);
	AMX_DECLARE_NATIVE(TSC_SetChannelDescription);
	AMX_DECLARE_NATIVE(TSC_SetChannelType);
	AMX_DECLARE_NATIVE(TSC_GetChannelType);
	AMX_DECLARE_NATIVE(TSC_SetChannelPassword);
	AMX_DECLARE_NATIVE(TSC_HasChannelPassword);
	//AMX_DECLARE_NATIVE(TSC_SetChannelTalkPower);
	AMX_DECLARE_NATIVE(TSC_SetChannelUserLimit);
	AMX_DECLARE_NATIVE(TSC_GetChannelUserLimit);
	AMX_DECLARE_NATIVE(TSC_SetChannelParentId);
	AMX_DECLARE_NATIVE(TSC_GetChannelParentId);
	AMX_DECLARE_NATIVE(TSC_SetChannelOrderId);
	AMX_DECLARE_NATIVE(TSC_GetChannelOrderId);
	AMX_DECLARE_NATIVE(TSC_GetDefaultChannelId);
	AMX_DECLARE_NATIVE(TSC_FindChannel);

	/*
	//client natives
	AMX_DECLARE_NATIVE(TSC_KickClient);
	AMX_DECLARE_NATIVE(TSC_BanClient);
	AMX_DECLARE_NATIVE(TSC_MoveClient);


	AMX_DECLARE_NATIVE(TSC_SetClientChannelGroup);
	AMX_DECLARE_NATIVE(TSC_AddClientToServerGroup);
	AMX_DECLARE_NATIVE(TSC_RemoveClientFromServerGroup);
	AMX_DECLARE_NATIVE(TSC_ToggleClientTalkAbility);

	AMX_DECLARE_NATIVE(TSC_PokeClient);


	//messaging
	AMX_DECLARE_NATIVE(TSC_SendClientMessage);*/
	AMX_DECLARE_NATIVE(TSC_SendServerMessage);
}


#endif // INC_NATIVES_H
