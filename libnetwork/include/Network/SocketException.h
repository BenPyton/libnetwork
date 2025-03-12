/*
 * @author PELLETIER Benoit
 *
 * @file SocketException.h
 *
 * @date 18/07/2019
 *
 * @brief Define an exception for socket class
 *
 */

#ifndef _SOCKETEXCEPTION_H
#define _SOCKETEXCEPTION_H

#include <iostream>
#include <stdexcept>
#include "Exports.h"

namespace net
{
	class LIBNETWORK_API SocketException : public std::runtime_error
	{
	private:
		int m_code = -1;

	public:
		SocketException() noexcept;
		SocketException(int _code) noexcept;
		SocketException(char const* const _msg, int _code) noexcept;

		inline int code() { return m_code; }
	};
} //namespace net

#endif // _SOCKETEXCEPTION_H
