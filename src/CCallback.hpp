#pragma once
#ifndef INC_CCALLBACK_H
#define INC_CCALLBACK_H


#include "main.hpp"

#include <string>
#include <stack>
#include <deque>
#include <functional>
#include <memory>
#include <boost/variant.hpp>
#include <boost/unordered_set.hpp>
#include <boost/lockfree/spsc_queue.hpp>

using std::string;
using std::stack;
using std::deque;
using std::function;
using std::shared_ptr;
using boost::variant;
using boost::unordered_set;

#include "CSingleton.hpp"


enum class EErrorType
{
	INVALID,
	CONNECTION_ERROR,
	TEAMSPEAK_ERROR,
	CALLBACK_ERROR
};

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
typedef shared_ptr<CCallback> Callback_t;

class CCallbackHandler : public CSingleton<CCallbackHandler>
{
	friend class CSingleton<CCallbackHandler>;
private: //constructor / deconstructor
	CCallbackHandler() {}
	~CCallbackHandler() {}


private: //variables
	boost::lockfree::spsc_queue<
			Callback_t,
			boost::lockfree::fixed_sized<true>,
			boost::lockfree::capacity<32678>
		> m_Queue;

	unordered_set<AMX *> m_AmxList;


public: //functions
	Callback_t Create(string name, string format,
		AMX* amx, cell* params, const cell param_offset);

	inline void Call(Callback_t callback)
	{
		m_Queue.push(callback);
	}
	template <typename... Args>
	inline void Call(const string &name, Args&&... args)
	{
		Call(std::make_shared<CCallback>(name, std::forward<Args>(args)...));
	}

	inline void ForwardError(EErrorType error_type, 
		const unsigned int error_id, const string &error_msg)
	{
		Call("TSC_OnError", 
			static_cast<unsigned int>(error_type), error_id, error_msg);
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
