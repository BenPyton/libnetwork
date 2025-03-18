// Copyright (c) 2018 Benoit Pelletier
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef _EVENT_H
#define _EVENT_H

#include <vector>

template<typename... Args>
class Event
{
	typedef void (*callback)(Args...);
	typedef typename std::vector<callback>::const_iterator subscriber;

private:
	std::vector<callback> m_callbacks;

public:
	Event() {}

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
