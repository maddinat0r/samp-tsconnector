#pragma once

#ifndef INC_MAIN_H
#define INC_MAIN_H

#include <malloc.h>

//SAMP
#include "SDK/amx/amx.h"
#include "SDK/plugincommon.h"

typedef void (*logprintf_t)(char* format, ...);


#ifdef _WIN32 //Window$
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <Windows.h>
#include <Ws2tcpip.h>

#else //Linux
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#define SOCKET_ERROR (-1)
#endif



//STD
#include <string>
#include <vector>

using std::string;
using std::vector;


extern logprintf_t logprintf;

string AMX_GetString(AMX* amx, cell param);
int AMX_SetString(AMX* amx, cell param, string str);


#endif