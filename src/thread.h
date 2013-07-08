#pragma once

#ifndef INC_THREAD_H
#define INC_THREAD_H



#ifdef _WIN32 //Window$
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#define SLEEP(x) { Sleep(x); }
#else //Linux
#include <pthread.h>
#include <unistd.h>

#define SLEEP(x) { usleep(x * 1000); }
#endif


#ifdef _WIN32
DWORD __stdcall SocketThread(LPVOID lpParam);
#else
void * SocketThread(void *lpParam);
#endif



class CMutex {
private:
#ifdef _WIN32
	CRITICAL_SECTION m_Mutex;
#else
	pthread_mutex_t m_Mutex;
#endif

public:
	CMutex() {
	#ifdef _WIN32
		InitializeCriticalSection(&m_Mutex);
	#else
		pthread_mutexattr_t MutexAttr;
		pthread_mutexattr_init(&MutexAttr);
		pthread_mutexattr_settype(&MutexAttr, PTHREAD_MUTEX_RECURSIVE);
		pthread_mutex_init(&m_Mutex, &MutexAttr);
	#endif
	}

	~CMutex() {
	#ifdef _WIN32
		DeleteCriticalSection(&m_Mutex);
	#else
		pthread_mutex_destroy(&m_Mutex);
	#endif
	}

	inline void Lock() {
		
	#ifdef _WIN32
		EnterCriticalSection(&m_Mutex);
	#else
		pthread_mutex_lock(&m_Mutex); 
	#endif
	}

	inline void Unlock() {
	#ifdef _WIN32
		LeaveCriticalSection(&m_Mutex);
	#else
		pthread_mutex_unlock(&m_Mutex); 
	#endif
	}

};

extern bool ThreadAlive;

#endif
