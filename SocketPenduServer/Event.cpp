/*
* @author PELLETIER Benoit
*
* @file Event.cpp
*
* @date 17/11/2018
*
* @brief Event class that call subscribers when emitted
*
*/

#include "stdafx.h"
#include "Event.h"

//Event::Event()
//{
//}
//
//subscriber Event::subscribe(callback _cb)
//{
//	m_callbacks.push_back(_cb);
//	return m_callbacks.crbegin().base();
//}
//
//void Event::unsubscribe(subscriber _s)
//{
//	m_callbacks.erase(_s);
//}
//
//void Event::emit()
//{
//	for (vector<callback>::iterator it = m_callbacks.begin(); it != m_callbacks.end(); ++it)
//	{
//		(*it)();
//	}
//}
