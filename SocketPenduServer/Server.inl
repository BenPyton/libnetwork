#include "Server.h"
/*
* @author PELLETIER Benoit
*
* @file Server.inl
*
* @date 18/11/2018
*
* @brief Define inline functions of the class "Server"
*
*/

template<typename T>
bool net::Server::pollMsg(T& _data, Socket*& _client)
{
	bool success = false;
	Message msg;
	m_mutRecvFromQueue.lock();
	if (!m_recvFrom.empty())
	{
		msg = m_recvFrom.front();
		m_recvFrom.pop();
		success = true;
		DebugLog("[Server pollMsg] Msg recieved from #%p\n", msg.client);
	}
	m_mutRecvFromQueue.unlock();

	if (success)
	{
		BinSerializer bs(0, BinSerializer::Read);
		bs.resize(msg.msg.size());
		memcpy(bs.data(), msg.msg.c_str(), msg.msg.size());
		bs.serialize(_data);
		_client = msg.client;
	}

	return success;
}

template<typename T>
void net::Server::waitMsg(T& _data, Socket* _client)
{
	bool recieved = false;
	Message recvMsg;
	DebugLog("[Server waitMsg] Wait for client msg\n");
	while (!recieved)
	{
		m_mutRecvFromQueue.lock();
		if (!m_recvFrom.empty() && (nullptr == _client || m_recvFrom.front().client == _client))
		{
			DebugLog("[Server waitMsg] Msg recieved!\n");
			recvMsg = m_recvFrom.front();
			m_recvFrom.pop();
			recieved = true;
		}
		m_mutRecvFromQueue.unlock();
		if (!recieved) // block
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		else
		{
			BinSerializer bs(0, BinSerializer::Read);
			bs.resize(recvMsg.msg.size());
			memcpy(bs.data(), recvMsg.msg.c_str(), recvMsg.msg.size());
			bs.serialize(_data);
		}
	}
}

template<typename T>
void net::Server::send(Socket* _client, T& _data)
{
	BinSerializer bs(0, BinSerializer::Write);
	bs.serialize(_data);

	m_mutToSendQueue.lock();
	m_toSend.push({ _client, string(bs.data(), bs.size()) });
	m_mutToSendQueue.unlock();
}

template<typename T>
inline void net::Server::broadcast(T & _data)
{
	for (std::vector<Socket*>::iterator it = m_clients.begin(); it != m_clients.end(); ++it)
	{
		send(*it, _data);
	}
}
