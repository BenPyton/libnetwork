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

namespace net
{
	template<typename T>
	void Client::send(T& _msg)
	{
		Serializer bs(0, Serializer::Mode::Write);
		bs.serialize(_msg);
		string str(bs.data(), bs.size());

		AddMsgToSendingQueue(str);
	}

	template<typename T>
	bool Client::pollMsg(T& _data)
	{
		bool received = false;
		string msg;

		received = GetMsgFromReceivingQueue(msg);
		if (received)
		{
			DebugLog("[Client pollMsg] Msg received from server !\n");
			Serializer bs(0, Serializer::Mode::Read);
			bs.resize(msg.size());
			memcpy(bs.data(), msg.c_str(), msg.size());
			bs.serialize(_data);
		}

		return received;
	}

	template<typename T>
	void Client::waitMsg(T& _msg)
	{
		bool received = false;
		string msg;
		DebugLog("[Client waitMsg] Wait for server msg...\n");
		while (!received) {
			received = GetMsgFromReceivingQueue(msg);
			if (!received) // block
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
			else
			{
				DebugLog("[Client waitMsg] Msg received from server !\n");
				Serializer bs(0, Serializer::Mode::Read);
				bs.resize(msg.size());
				memcpy(bs.data(), msg.c_str(), msg.size());
				bs.serialize(_msg);
			}
		}
	}
}