#pragma once

#include "main.h"
#include "natives.h"

#include "CTeamspeak.h"

void **ppPluginData;
extern void *pAMXFunctions;
logprintf_t logprintf;



PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() {
	return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES; 
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData) {
	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
	logprintf = (logprintf_t)ppData[PLUGIN_DATA_LOGPRINTF];
	
#ifdef _WIN32
	WSAData wsa_unused;
	WSAStartup(MAKEWORD(2,0), &wsa_unused);
#endif

	logprintf("TSConnector v0.1 loaded.");
	return 1;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload() {
#ifdef _WIN32
	WSACleanup();	
#endif
	logprintf("TSConnector unloaded.");
}


#if defined __cplusplus
extern "C"
#endif
const AMX_NATIVE_INFO NativesList[] = {
	{"TSC_Connect",					native_TSC_Connect},
	{"TSC_Disconnect",				native_TSC_Disconnect},
	{"TSC_Login",					native_TSC_Login},

	{"TSC_SetActiveVServer",		native_TSC_SetActiveVServer},

	{"TSC_CreateChannel",			native_TSC_CreateChannel},
	{"TSC_DeleteChannel",			native_TSC_DeleteChannel},
	{"TSC_GetChannelIDByName",		native_TSC_GetChannelIDByName},
	{"TSC_SetChannelName",			native_TSC_SetChannelName},
	{"TSC_SetChannelDescription",	native_TSC_SetChannelDescription},
	{"TSC_SetChannelType",			native_TSC_SetChannelType},
	{"TSC_SetChannelPassword",		native_TSC_SetChannelPassword},
	{"TSC_SetChannelTalkPower",		native_TSC_SetChannelTalkPower},
	{"TSC_SetChannelUserLimit",		native_TSC_SetChannelUserLimit},

	{"TSC_KickClient",				native_TSC_KickClient},
	{"TSC_BanClient",				native_TSC_BanClient},

	{"TSC_SetTimeoutTime",			native_TSC_SetTimeoutTime},
	{"TSC_GetChannelName",				native_TSC_GetChannelName},
	{"TSC_GetChannelClientList",		native_TSC_GetChannelClientList},

	{"TSC_GetClientIDByName",			native_TSC_GetClientIDByName},
	{NULL, NULL}
};

/*
PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx) {
	p_Amx.push_back(amx);
	return amx_Register(amx, NativesList, -1);
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx) { 
	for (list<AMX *>::iterator i = p_Amx.begin(); i != p_Amx.end(); i++) {
		if (* i == amx) {
			p_Amx.erase(i);
			break;
		}
	}
	return AMX_ERR_NONE;
}
*/

string AMX_GetString(AMX* amx, cell param) {
	cell *String;
	char *Dest;
	int Len;
	amx_GetAddr(amx, param, &String);
	amx_StrLen(String, &Len);
	Dest = new char[Len + 1];
	amx_GetString(Dest, String, 0, UNLIMITED);
	Dest[Len] = '\0';
	string Return(Dest);
	delete Dest;
	return Return;
}


int AMX_SetString(AMX* amx, cell param, string str) {
	cell *Dest;
	amx_GetAddr(amx, param, &Dest);
	amx_SetString(Dest, str.c_str(), 0, 0, str.length() + 1);
	return 1;
}