/*
* @author PELLETIER Benoit
*
* @file Event.h
*
* @date 18/07/2018
*
* @brief Event class that call subscribers when emitted
*
*/

#ifndef _EVENT_H
#define _EVENT_H

#include <vector>

using namespace std;

template<typename... Args>
class Event
{
	typedef void(*callback)(Args...);
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

	void emit(Args... _args)
	{
		for (subscriber it = m_callbacks.begin(); it != m_callbacks.end(); ++it)
		{
			(*it)(_args...);
		}
	}
};

#endif // _EVENT_H