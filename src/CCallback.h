#pragma once
#ifndef INC_CCALLBACK_H
#define INC_CCALLBACK_H


#include "main.h"

#include <string>
#include <stack>
#include <boost/variant.hpp>
#include <boost/unordered_set.hpp>
#include <boost/lockfree/spsc_queue.hpp>

using std::string;
using std::stack;
using boost::variant;
using boost::unordered_set;

#include "CSingleton.h"


struct Callback
{
	typedef variant<cell, string> Param_t;
	
	Callback(const char *name) :
		Name(name)
	{}

	string Name;
	stack<Param_t> Params;
};


class CCallbackHandler : public CSingleton<CCallbackHandler>
{
	friend class CSingleton<CCallbackHandler>;
private: //constructor / deconstructor
	CCallbackHandler() {}
	~CCallbackHandler() {}


private: //variables
	boost::lockfree::spsc_queue<
			Callback *,
			boost::lockfree::fixed_sized<true>,
			boost::lockfree::capacity<32678>
		> m_Queue;

	unordered_set<AMX *> m_AmxList;


public: //functions
	inline void Push(Callback *cb)
	{
		m_Queue.push(cb);
	}

	inline void AddAmx(AMX *amx)
	{
		m_AmxList.insert(amx);
	}
	inline void EraseAmx(AMX *amx)
	{
		m_AmxList.erase(amx);
	}


	void Process();
};


#endif // INC_CCALLBACK_H
