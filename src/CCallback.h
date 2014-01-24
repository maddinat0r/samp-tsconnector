#pragma once
#ifndef INC_CCALLBACK_H
#define INC_CCALLBACK_H


#include "main.h"

#include <string>
#include <stack>
#include <boost/unordered_set.hpp>
#include <boost/lockfree/spsc_queue.hpp>

using std::string;
using std::stack;
using boost::unordered_set;


class CCallback 
{
private:
	static boost::lockfree::spsc_queue<
		CCallback *,
		boost::lockfree::fixed_sized<true>,
		boost::lockfree::capacity<32678>
	> m_CallbackQueue;

	static unordered_set<AMX *> m_AmxList;
	
public:
	static inline void Queue(CCallback *cb)
	{
		m_CallbackQueue.push(cb);
	}
	static inline void AddAmxInstance(AMX *amx)
	{
		m_AmxList.insert(amx);
	}
	static inline void RemoveAmxInstance(AMX *amx)
	{
		m_AmxList.erase(amx);
	}

	static void Process();

	string Name;
	stack<string> Params;
};


#endif // INC_CCALLBACK_H
