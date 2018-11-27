/*
* @author PELLETIER Benoit
*
* @file Event.h
*
* @date 17/11/2018
*
* @brief Event class that call subscribers when emitted
*
*/

#ifndef _EVENT_H
#define _EVENT_H

#include <iostream>
#include <vector>

using namespace std;

template<typename T>
class Event
{
	typedef void(*callback)(T);
	typedef typename vector<callback>::const_iterator subscriber;

private:
	vector<callback> m_callbacks;

public:
	Event() { }

	subscriber subscribe(callback _cb)
	{
		m_callbacks.push_back(_cb);
		return m_callbacks.crbegin().base();
	}

	void unsubscribe(subscriber _s)
	{
		m_callbacks.erase(_s);
	}

	void emit(T _v)
	{
		for (subscriber it = m_callbacks.begin(); it != m_callbacks.end(); ++it)
		{
			(*it)(_v);
		}
	}
};

#endif // _EVENT_H