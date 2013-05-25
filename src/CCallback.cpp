#include "CCallback.h"

CMutex CCallback::CallbackMutex;
queue<CCallback*> CCallback::CallbackQueue;

void CCallback::AddCallbackToQueue(CCallback *cb) {
	CallbackMutex.Lock();
	CallbackQueue.push(cb);
	CallbackMutex.Unlock();
}

CCallback *CCallback::GetNextCallback() {
	CallbackMutex.Lock();
	CCallback *nextcb = CallbackQueue.front();
	CallbackQueue.pop();
	CallbackMutex.Unlock();
	return nextcb;
}

bool CCallback::IsQueueEmpty() {
	CallbackMutex.Lock();
	bool empty = CallbackQueue.empty();
	CallbackMutex.Unlock();
	return empty;
}
