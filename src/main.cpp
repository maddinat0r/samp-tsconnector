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
				queue<cell> AmxAddresses;
				while(!Callback->Params.empty()) {
					cell tmpAddress;
					amx_PushString( (*amx), &tmpAddress, NULL, Callback->Params.top().c_str(), 0, 0);
					Callback->Params.pop();
					AmxAddresses.push(tmpAddress);
				}
				amx_Exec( (*amx), NULL, CB_IDX);
				while(!AmxAddresses.empty()) {
					amx_Release( (*amx), AmxAddresses.front());
					AmxAddresses.pop();
				}
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

	logprintf("TSConnector v0.3.1 loaded.");

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

	{"TSC_SetTimeoutTime",				native_TSC_SetTimeoutTime},


	{"TSC_CreateChannel",				native_TSC_CreateChannel},
	{"TSC_DeleteChannel",				native_TSC_DeleteChannel},

	{"TSC_SetChannelName",				native_TSC_SetChannelName},
	{"TSC_SetChannelDescription",		native_TSC_SetChannelDescription},
	{"TSC_SetChannelPassword",			native_TSC_SetChannelPassword},
	{"TSC_SetChannelTalkPower",			native_TSC_SetChannelTalkPower},
	{"TSC_SetChannelSubChannel",		native_TSC_SetChannelSubChannel},
	{"TSC_MoveChannelBelowChannel",		native_TSC_MoveChannelBelowChannel},

	
	{"TSC_KickClient",					native_TSC_KickClient},
	{"TSC_BanClient",					native_TSC_BanClient},
	{"TSC_MoveClient",					native_TSC_MoveClient},
	
	{"TSC_SetClientChannelGroup",		native_TSC_SetClientChannelGroup},
	{"TSC_AddClientToServerGroup",		native_TSC_AddClientToServerGroup},
	{"TSC_RemoveClientFromServerGroup",	native_TSC_RemoveClientFromServerGroup},

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
	delete[] Dest;
	return Return;
}


int AMX_SetString(AMX* amx, cell param, string str) {
	cell *Dest;
	amx_GetAddr(amx, param, &Dest);
	amx_SetString(Dest, str.c_str(), 0, 0, str.length() + 1);
	return 1;
}
