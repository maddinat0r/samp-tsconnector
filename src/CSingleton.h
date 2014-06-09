#pragma once
#ifndef INC_CSINGLETON_H
#define INC_CSINGLETON_H


template<class T>
class CSingleton
{
protected:
	static T *m_Instance;

public:
	CSingleton() { }
	virtual ~CSingleton() { }

	inline static T *Get()
	{
		if (m_Instance == NULL)
			m_Instance = new T;
		return m_Instance;
	}

	inline static void Destroy()
	{
		if (m_Instance != NULL)
		{
			delete m_Instance;
			m_Instance = NULL;
		}
	}
};

template <class T>
T* CSingleton<T>::m_Instance = NULL;


#endif // INC_CSINGLETON_H
