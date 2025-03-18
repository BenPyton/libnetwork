// Copyright (c) 2019 Benoit Pelletier
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "Network\SocketException.h"

namespace net
{
	SocketException::SocketException() noexcept
		: std::runtime_error("")
		, m_code(-1)
	{
	}

	SocketException::SocketException(int _code) noexcept
		: std::runtime_error("")
		, m_code(_code)
	{
	}

	SocketException::SocketException(char const* const _msg, int _code) noexcept
		: std::runtime_error(_msg)
		, m_code(_code)
	{
	}
} //namespace net
