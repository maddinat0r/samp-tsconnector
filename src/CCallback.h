#pragma once
#ifndef INC_CCALLBACK_H
#define INC_CCALLBACK_H


#include "main.h"

#include <string>
#include <stack>
#include <deque>
#include <boost/variant.hpp>
#include <boost/unordered_set.hpp>
#include <boost/lockfree/spsc_queue.hpp>

using std::string;
using std::stack;
using std::deque;
using boost::variant;
using boost::unordered_set;

#include "CSingleton.h"




class CCallbackHandler : public CSingleton<CCallbackHandler>
{
	friend class CSingleton<CCallbackHandler>;
private: //callback data
	struct Callback
	{
		template <typename... Args>
		Callback(const char *name, Args&&... args) :
			Name(name),
			Params(deque<variant<cell, string>>{ args... })
		{ }

		string Name;
		stack<variant<cell, string>> Params;
	};


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
	template <typename... Args>
	void Call(const char *name, Args&&... args)
	{
		m_Queue.push(new Callback(name, std::forward<Args>(args)...));
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
