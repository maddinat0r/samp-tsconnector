#pragma once
#ifndef INC_MAIN_H
#define INC_MAIN_H


#include <malloc.h>
#include <sdk/amx/amx.h>
#include <sdk/plugincommon.h>

typedef void (*logprintf_t)(char* format, ...);
extern logprintf_t logprintf;


#endif // INC_MAIN_H
