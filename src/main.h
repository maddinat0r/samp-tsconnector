#pragma once
#ifndef INC_MAIN_H
#define INC_MAIN_H


#include <malloc.h>
#include "SDK/amx/amx.h"
#include "SDK/plugincommon.h"

typedef void (*logprintf_t)(char* format, ...);
extern logprintf_t logprintf;


#endif // INC_MAIN_H
