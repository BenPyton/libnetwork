//#include "Server.h"
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

namespace net
{
	template<typename T>
	bool Server::pollMsg(T& _data, Socket*& _client)
	{
		bool success = false;
		Message msg;
		/*m_mutRecvFromQueue.lock();
		if (!m_recvFrom.empty())
		{
			msg = m_recvFrom.front();
			m_recvFrom.pop();
			success = true;
			DebugLog("[Server pollMsg] Msg recieved from #%p\n", msg.client);
		}
		m_mutRecvFromQueue.unlock();*/
		success = GetMsgFromReceivingQueue(msg);
		if (success)
		{
			DebugLog("[Server pollMsg] Msg received from #%p\n", msg.client);
			Serializer bs(0, Serializer::Mode::Read);
			bs.resize(msg.msg.size());
			memcpy(bs.data(), msg.msg.c_str(), msg.msg.size());
			bs.serialize(_data);
			_client = msg.client;
		}

		return success;
	}

	template<typename T>
	void Server::waitMsg(T& _data, Socket* _client)
	{
		bool received = false;
		Message msg;
		DebugLog("[Server waitMsg] Wait for client msg...\n");
		while (!received)
		{
			/*m_mutRecvFromQueue.lock();
			if (!m_recvFrom.empty() && (nullptr == _client || m_recvFrom.front().client == _client))
			{
				DebugLog("[Server waitMsg] Msg recieved!\n");
				msg = m_recvFrom.front();
				m_recvFrom.pop();
				recieved = true;
			}
			m_mutRecvFromQueue.unlock();*/
			received = GetMsgFromReceivingQueue(msg, _client);
			if (!received) // block
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
			else
			{
				DebugLog("[Server waitMsg] Msg received from #%p\n", msg.client);
				Serializer bs(0, Serializer::Mode::Read);
				bs.resize(msg.msg.size());
				memcpy(bs.data(), msg.msg.c_str(), msg.msg.size());
				bs.serialize(_data);
			}
		}
	}

	template<typename T>
	void Server::send(Socket* _client, T& _data)
	{
		Serializer bs(0, Serializer::Mode::Write);
		bs.serialize(_data);
/*
		m_mutToSendQueue.lock();
		m_toSend.push({ _client, string(bs.data(), bs.size()) });
		m_mutToSendQueue.unlock();*/

		//AddMsgToSendingQueue({ _client, string(bs.data(), bs.size()) });
		AddMsgToSendingQueue({ _client, bs});
	}

	template<typename T>
	inline void Server::broadcast(T & _data)
	{
		for (std::vector<Socket*>::iterator it = m_clients.begin(); it != m_clients.end(); ++it)
		{
			send(*it, _data);
		}
	}
}