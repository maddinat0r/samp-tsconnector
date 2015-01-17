#pragma once
#ifndef INC_MAIN_H
#define INC_MAIN_H


#include <malloc.h>
#include <amx/amx2.h>
#include <plugincommon.h>

typedef void (*logprintf_t)(char* format, ...);
extern logprintf_t logprintf;


#endif // INC_MAIN_H
