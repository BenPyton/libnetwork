/*
* @author PELLETIER Benoit
*
* @file Client.inl
*
* @date 18/11/2018
*
* @brief Define inline functions of the class "Client"
*
*/


template<typename T>
void net::Client::send(T& _msg)
{
	BinSerializer bs(0, BinSerializer::Write);
	bs.serialize(_msg);
	string str(bs.data(), bs.size());

	m_mutToSendQueue.lock();
	m_toSend.push(str);
	m_mutToSendQueue.unlock();
}

template<typename T>
bool net::Client::pollMsg(T& _data)
{
	bool success = false;
	string msg;
	m_mutRecvFromQueue.lock();
	if (!m_recvFrom.empty())
	{
		msg = m_recvFrom.front();
		m_recvFrom.pop();
		success = true;
		DebugLog("[Client pollMsg] Msg recieved from server !\n");
	}
	m_mutRecvFromQueue.unlock();

	if (success)
	{
		BinSerializer bs(0, BinSerializer::Read);
		bs.resize(msg.size());
		memcpy(bs.data(), msg.c_str(), msg.size());
		bs.serialize(_data);
	}

	return success;
}

template<typename T>
void net::Client::waitMsg(T& _msg)
{
	bool recieved = false;
	string recvMsg;
	DebugLog("[Client waitMsg] Wait for server msg\n");
	while (!recieved) {
		m_mutRecvFromQueue.lock();
		if (!m_recvFrom.empty())
		{
			DebugLog("[Client waitMsg] Msg recieved!\n");
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
			bs.resize(recvMsg.size());
			memcpy(bs.data(), recvMsg.c_str(), recvMsg.size());
			bs.serialize(_msg);
		}
	}
}