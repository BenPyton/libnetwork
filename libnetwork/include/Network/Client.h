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

using namespace std;

namespace net{

	class LIBNETWORK_API Client : public MsgSystem
	{
	public:
		Client();
		~Client();

		void connect(string _addr, unsigned short _port);
		void disconnect();

		void send(string _msg);
	};
}

#endif // _CLIENTV2_H