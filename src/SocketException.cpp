/*
* @author PELLETIER Benoit
*
* @file SocketException.cpp
*
* @date 18/07/2019
*
* @brief Define an exception for socket class
*
*/

#include "Network\SocketException.h"

namespace net{
	SocketException::SocketException() noexcept
		: std::exception()
	{
	}

	SocketException::SocketException(int _code) noexcept
		: std::exception(), m_code(_code)
	{
	}

	SocketException::SocketException(char const * const _msg, int _code) noexcept
		: std::exception(_msg), m_code(_code)
	{
	}
}
