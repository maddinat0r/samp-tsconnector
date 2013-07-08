#pragma once

#include "main.h"
#include "natives.h"
#include "thread.h"

#include "CTeamspeak.h"
#include "CCallback.h"


list<AMX*> AmxList;
void **ppPluginData;
extern void *pAMXFunctions;
logprintf_t logprintf;

PLUGIN_EXPORT void PLUGIN_CALL ProcessTick() {
	while(!CCallback::IsQueueEmpty()) {
		CCallback *Callback = CCallback::GetNextCallback();

		int CB_IDX;
		for (list<AMX*>::iterator amx = AmxList.begin(), end = AmxList.end(); amx != end; ++amx) {
			if (amx_FindPublic( (*amx), Callback->Name.c_str(), &CB_IDX) == AMX_ERR_NONE) {
				cell AmxAddress = -1;
				while(!Callback->Params.empty()) {
					cell tmpAddress;
					amx_PushString( (*amx), &tmpAddress, NULL, Callback->Params.top().c_str(), 0, 0);
					Callback->Params.pop();
					if(AmxAddress == -1)
						AmxAddress = tmpAddress;
				}

				amx_Exec( (*amx), NULL, CB_IDX);
				if(AmxAddress >= NULL)
					amx_Release( (*amx), AmxAddress);
			}
		}

		delete Callback;
	}
}

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() {
	return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES | SUPPORTS_PROCESS_TICK; 
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData) {
	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
	logprintf = (logprintf_t)ppData[PLUGIN_DATA_LOGPRINTF];
	
#ifdef _WIN32
	WSAData wsa_unused;
	if(WSAStartup(MAKEWORD(2,0), &wsa_unused) != 0)
		return 0;
#endif


#ifdef WIN32
	DWORD ThreadID = 0;
	HANDLE ThreadHandle = CreateThread(NULL, 0, &SocketThread, NULL, 0, &ThreadID);
	CloseHandle(ThreadHandle);
#else
	pthread_t threadHandle;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&threadHandle, &attr, &SocketThread, NULL);
#endif


	logprintf("TSConnector v0.4 loaded.");
	return 1;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload() {
#ifdef _WIN32
	WSACleanup();	
#endif
	ThreadAlive = false;
	logprintf("TSConnector unloaded.");
}


#if defined __cplusplus
extern "C"
#endif
const AMX_NATIVE_INFO NativesList[] = {
	{"TSC_Connect",						native_TSC_Connect},
	{"TSC_Disconnect",					native_TSC_Disconnect},
	{"TSC_Login",						native_TSC_Login},


	{"TSC_CreateChannel",				native_TSC_CreateChannel},
	{"TSC_DeleteChannel",				native_TSC_DeleteChannel},

	{"TSC_SetChannelName",				native_TSC_SetChannelName},
	{"TSC_SetChannelDescription",		native_TSC_SetChannelDescription},
	{"TSC_SetChannelType",				native_TSC_SetChannelType},
	{"TSC_SetChannelPassword",			native_TSC_SetChannelPassword},
	{"TSC_SetChannelTalkPower",			native_TSC_SetChannelTalkPower},
	{"TSC_SetChannelUserLimit",			native_TSC_SetChannelUserLimit},
	{"TSC_SetChannelSubChannel",		native_TSC_SetChannelSubChannel},
	{"TSC_MoveChannelBelowChannel",		native_TSC_MoveChannelBelowChannel},

	
	{"TSC_KickClient",					native_TSC_KickClient},
	{"TSC_BanClient",					native_TSC_BanClient},
	{"TSC_MoveClient",					native_TSC_MoveClient},
	
	{"TSC_SetClientChannelGroup",		native_TSC_SetClientChannelGroup},
	{"TSC_AddClientToServerGroup",		native_TSC_AddClientToServerGroup},
	{"TSC_RemoveClientFromServerGroup",	native_TSC_RemoveClientFromServerGroup},
	{"TSC_ToggleClientTalkAbility",		native_TSC_ToggleClientTalkAbility},

	{"TSC_PokeClient",					native_TSC_PokeClient},


	{NULL, NULL}
};


PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx) {
	AmxList.push_back(amx);
	return amx_Register(amx, NativesList, -1);
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx) {
	for(list<AMX*>::iterator i = AmxList.begin(), max = AmxList.end(); i != max; ++i) {
		if( (*i) == amx) {
			AmxList.erase(i);
			break;
		}
	}
	return AMX_ERR_NONE;
}


int AMX_SetString(AMX* amx, cell param, const char *str) {
	cell *Dest;
	amx_GetAddr(amx, param, &Dest);
	amx_SetString(Dest, str, 0, 0, strlen(str) + 1);
	return 1;
}
