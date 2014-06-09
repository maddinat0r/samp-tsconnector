#include "main.h"
#include "natives.h"
#include "CTeamspeak.h"
#include "CCallback.h"


extern void *pAMXFunctions;
logprintf_t logprintf;


PLUGIN_EXPORT void PLUGIN_CALL ProcessTick() 
{
	CCallback::Process();
}

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() 
{
	return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES | SUPPORTS_PROCESS_TICK; 
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData) 
{
	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
	logprintf = reinterpret_cast<logprintf_t>(ppData[PLUGIN_DATA_LOGPRINTF]);
	

	logprintf(" >> plugin.TSConnector: v0.6.2 loaded.");
	return 1;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload() 
{
	CTeamspeak::Destroy();

	logprintf("plugin.TSConnector: Plugin unloaded.");
}


extern "C" const AMX_NATIVE_INFO native_list[] = 
{
	AMX_DEFINE_NATIVE(TSC_Connect)
	AMX_DEFINE_NATIVE(TSC_Disconnect)
	AMX_DEFINE_NATIVE(TSC_Login)
	AMX_DEFINE_NATIVE(TSC_ChangeNickname)


	AMX_DEFINE_NATIVE(TSC_CreateChannel)
	AMX_DEFINE_NATIVE(TSC_DeleteChannel)
	AMX_DEFINE_NATIVE(TSC_SetChannelName)
	AMX_DEFINE_NATIVE(TSC_SetChannelDescription)
	AMX_DEFINE_NATIVE(TSC_SetChannelType)
	AMX_DEFINE_NATIVE(TSC_SetChannelPassword)
	AMX_DEFINE_NATIVE(TSC_SetChannelTalkPower)
	AMX_DEFINE_NATIVE(TSC_SetChannelUserLimit)
	AMX_DEFINE_NATIVE(TSC_SetChannelSubChannel)
	AMX_DEFINE_NATIVE(TSC_MoveChannelBelowChannel)


	AMX_DEFINE_NATIVE(TSC_KickClient)
	AMX_DEFINE_NATIVE(TSC_BanClient)
	AMX_DEFINE_NATIVE(TSC_MoveClient)


	AMX_DEFINE_NATIVE(TSC_SetClientChannelGroup)
	AMX_DEFINE_NATIVE(TSC_AddClientToServerGroup)
	AMX_DEFINE_NATIVE(TSC_RemoveClientFromServerGroup)
	AMX_DEFINE_NATIVE(TSC_ToggleClientTalkAbility)

	AMX_DEFINE_NATIVE(TSC_PokeClient)


	AMX_DEFINE_NATIVE(TSC_SendClientMessage)
	AMX_DEFINE_NATIVE(TSC_SendChannelMessage)
	AMX_DEFINE_NATIVE(TSC_SendServerMessage)

	{NULL, NULL}
};


PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx) 
{
	CCallback::AddAmxInstance(amx);
	return amx_Register(amx, native_list, -1);
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx) 
{
	CCallback::RemoveAmxInstance(amx);
	return AMX_ERR_NONE;
}
