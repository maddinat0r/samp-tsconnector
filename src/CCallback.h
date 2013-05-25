#pragma once

#ifndef INC_CCALLBACK_H
#define INC_CCALLBACK_H

#include "thread.h"

#include <string>
#include <stack>
#include <queue>

using std::string;
using std::stack;
using std::queue;

class CCallback {
private:
	static CMutex CallbackMutex;
	static queue<CCallback*> CallbackQueue;
	
public:
	static void AddCallbackToQueue(CCallback *cb);
	static CCallback *GetNextCallback();
	static bool IsQueueEmpty();

	string Name;
	stack<string> Params;
};


#endif
