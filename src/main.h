#pragma once

#ifndef INC_MAIN_H
#define INC_MAIN_H

#define _CRT_SECURE_NO_WARNINGS

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
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#define SOCKET_ERROR (-1)
#endif


//STD
#include <string>
#include <vector>
#include <list>

using std::string;
using std::vector;
using std::list;


extern logprintf_t logprintf;

int AMX_SetString(AMX* amx, cell param, const char *str);

#endif
