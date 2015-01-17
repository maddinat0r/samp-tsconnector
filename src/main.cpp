#include "main.h"
#include "natives.h"
#include "CNetwork.h"
#include "CServer.h"
#include "CCallback.h"
#include "version.hpp"


extern void *pAMXFunctions;
logprintf_t logprintf;


PLUGIN_EXPORT void PLUGIN_CALL ProcessTick() 
{
	CCallbackHandler::Get()->Process();
}

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() 
{
	return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES | SUPPORTS_PROCESS_TICK; 
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData) 
{
	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
	logprintf = reinterpret_cast<logprintf_t>(ppData[PLUGIN_DATA_LOGPRINTF]);

	logprintf(" >> plugin.TSConnector: v" TSC_VERSION " loaded.");
	return 1;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload() 
{
	CNetwork::CSingleton::Destroy();
	CServer::CSingleton::Destroy();

	logprintf("plugin.TSConnector: Plugin unloaded.");
}


extern "C" const AMX_NATIVE_INFO native_list[] = 
{
	AMX_DEFINE_NATIVE(TSC_Connect)
	AMX_DEFINE_NATIVE(TSC_Disconnect)
	AMX_DEFINE_NATIVE(TSC_ChangeNickname)
	AMX_DEFINE_NATIVE(TSC_SendServerMessage)


	AMX_DEFINE_NATIVE(TSC_QueryChannelData)
	AMX_DEFINE_NATIVE(TSC_QueryClientData)
	AMX_DEFINE_NATIVE(TSC_GetQueriedData)
	AMX_DEFINE_NATIVE(TSC_GetQueriedDataAsInt)

	AMX_DEFINE_NATIVE(TSC_CreateChannel)
	AMX_DEFINE_NATIVE(TSC_DeleteChannel)
	AMX_DEFINE_NATIVE(TSC_GetChannelIdByName)
	AMX_DEFINE_NATIVE(TSC_IsValidChannel)
	AMX_DEFINE_NATIVE(TSC_SetChannelName)
	AMX_DEFINE_NATIVE(TSC_GetChannelName)
	AMX_DEFINE_NATIVE(TSC_SetChannelDescription)
	AMX_DEFINE_NATIVE(TSC_SetChannelType)
	AMX_DEFINE_NATIVE(TSC_GetChannelType)
	AMX_DEFINE_NATIVE(TSC_SetChannelPassword)
	AMX_DEFINE_NATIVE(TSC_HasChannelPassword)
	AMX_DEFINE_NATIVE(TSC_SetChannelRequiredTP)
	AMX_DEFINE_NATIVE(TSC_GetChannelRequiredTP)
	AMX_DEFINE_NATIVE(TSC_SetChannelUserLimit)
	AMX_DEFINE_NATIVE(TSC_GetChannelUserLimit)
	AMX_DEFINE_NATIVE(TSC_SetChannelParentId)
	AMX_DEFINE_NATIVE(TSC_GetChannelParentId)
	AMX_DEFINE_NATIVE(TSC_SetChannelOrderId)
	AMX_DEFINE_NATIVE(TSC_GetChannelOrderId)
	AMX_DEFINE_NATIVE(TSC_GetDefaultChannelId)


	AMX_DEFINE_NATIVE(TSC_GetClientIdByUid)
	AMX_DEFINE_NATIVE(TSC_GetClientIdByIpAddress)

	AMX_DEFINE_NATIVE(TSC_GetClientUid)
	AMX_DEFINE_NATIVE(TSC_GetClientDatabaseId)
	AMX_DEFINE_NATIVE(TSC_GetClientChannelId)
	AMX_DEFINE_NATIVE(TSC_GetClientIpAddress)

	AMX_DEFINE_NATIVE(TSC_KickClient)
	AMX_DEFINE_NATIVE(TSC_BanClient)
	AMX_DEFINE_NATIVE(TSC_MoveClient)

	AMX_DEFINE_NATIVE(TSC_SetClientChannelGroup)
	AMX_DEFINE_NATIVE(TSC_AddClientToServerGroup)
	AMX_DEFINE_NATIVE(TSC_RemoveClientFromServerGroup)

	AMX_DEFINE_NATIVE(TSC_PokeClient)
	AMX_DEFINE_NATIVE(TSC_SendClientMessage)
	
	{NULL, NULL}
};


PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx) 
{
	CCallbackHandler::Get()->AddAmx(amx);
	return amx_Register(amx, native_list, -1);
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx) 
{
	CCallbackHandler::Get()->EraseAmx(amx);
	return AMX_ERR_NONE;
}
