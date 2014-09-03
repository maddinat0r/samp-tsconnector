#pragma once
#ifndef INC_CCALLBACK_H
#define INC_CCALLBACK_H


#include "main.h"

#include <string>
#include <stack>
#include <deque>
#include <functional>
#include <boost/variant.hpp>
#include <boost/unordered_set.hpp>
#include <boost/lockfree/spsc_queue.hpp>

using std::string;
using std::stack;
using std::deque;
using std::function;
using boost::variant;
using boost::unordered_set;

#include "CSingleton.h"


class CCallback
{
	friend class CCallbackHandler;
private: //variables
	string m_Name;
	stack<variant<cell, string>> m_Params;
	function<void()>
		m_PreExecute,
		m_PostExecute;


public: //constructor / destructor
	template <typename... Args>
	CCallback(string name, Args&&... args) :
		m_Name(name),
		m_Params(deque<variant<cell, string>>{ args... })
	{ }
	CCallback(string name, decltype(m_Params) && arg_list) :
		m_Name(name),
		m_Params(arg_list)
	{ }
	~CCallback() = default;


public: //funtions
	inline void OnPreExecute(decltype(m_PreExecute) &&func)
	{
		m_PreExecute = func;
	}
	inline void OnPostExecute(decltype(m_PostExecute) &&func)
	{
		m_PostExecute = func;
	}


private: //functions
	inline void CallPreExecute()
	{
		if (m_PreExecute)
			m_PreExecute();
	}
	inline void CallPostExecute()
	{
		if (m_PostExecute)
			m_PostExecute();
	}
};

class CCallbackHandler : public CSingleton<CCallbackHandler>
{
	friend class CSingleton<CCallbackHandler>;
private: //constructor / deconstructor
	CCallbackHandler() {}
	~CCallbackHandler() {}


private: //variables
	boost::lockfree::spsc_queue<
			CCallback *,
			boost::lockfree::fixed_sized<true>,
			boost::lockfree::capacity<32678>
		> m_Queue;

	unordered_set<AMX *> m_AmxList;


public: //functions
	CCallback *Create(string name, string format, 
		AMX* amx, cell* params, const cell param_offset);

	inline void Call(CCallback *callback)
	{
		m_Queue.push(callback);
	}
	template <typename... Args>
	inline void Call(const char *name, Args&&... args)
	{
		Call(new CCallback(name, std::forward<Args>(args)...));
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
