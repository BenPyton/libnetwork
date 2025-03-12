/*
 * @author PELLETIER Benoit
 *
 * @file Client.h
 *
 * @date 18/07/2019
 *
 * @brief Simple client class
 *
 */

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
	};
} //namespace net

#endif // _CLIENT_H
