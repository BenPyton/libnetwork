/*
* @author PELLETIER Benoit
*
* @file ClientV2.h
*
* @date 18/07/2019
*
* @brief Simple client class
*
*/

#ifndef _CLIENTV2_H
#define _CLIENTV2_H

#include "MsgSystem.h"
#include "Exports.h"

using namespace std;

namespace net{

	class LIBNETWORK_API ClientV2 : public MsgSystem
	{
	public:
		ClientV2();
		~ClientV2();

		void connect(string _addr, unsigned short _port);
		void disconnect();

		void send(string _msg);
	};
}

#endif // _CLIENTV2_H