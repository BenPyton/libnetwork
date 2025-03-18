// Copyright (c) 2019 Benoit Pelletier
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef _CLIENT_H
#define _CLIENT_H

#include "MsgSystem.h"
#include "Exports.h"

namespace net
{
	class LIBNETWORK_API Client : public MsgSystem
	{
	public:
		Client();
		~Client();

		void connect(std::string _addr, unsigned short _port);
		void disconnect();

		void send(std::string _msg);

	private:
		class Socket* m_pClient = nullptr;
	};
} //namespace net

#endif // _CLIENT_H
