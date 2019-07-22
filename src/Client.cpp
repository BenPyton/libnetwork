/*
* @author PELLETIER Benoit
*
* @file Client.cpp
*
* @date 18/11/2018
*
* @brief Define a client that can be connected to a server
*
*/

#include "Network/Socket.h"
#include "Network/Client.h"

namespace net{
	Client::Client()
	{
		m_running = false;
	}

	Client::~Client()
	{
	}

	void Client::connect(string _addr, unsigned short _port, Protocol _protocol)
	{
		m_pSock = new Socket(_addr, _port, _protocol); 
		DebugLog("[Client connect] Connecting to server...\n");
		m_pSock->Connect();
		printf("[Client connect] Connected to server!\n");
		m_running = true;

		m_threadToSend = thread(&Client::RunSendToServer, this);
		m_threadRecvFrom = thread(&Client::RunRecvFromServer, this);
	}

	void Client::shutdown()
	{
		m_running = false;
		m_threadToSend.join();
		m_threadRecvFrom.join();

		m_pSock->Close();
		delete m_pSock;
	}

	void Client::RunSendToServer()
	{
		DebugLog("[Thread SendToServer] Thread started !\n");
		bool sending = false;
		string msg;
		Serializer serializer(sizeof(size_t), Serializer::Mode::Write);
		serializer.resize(sizeof(size_t));
		while (m_running)
		{
			sending = false;
			try
			{
				sending = GetMsgFromSendingQueue(msg);

				if (sending)
				{
					DebugLog("[Thread SendToServer] Sending data to server...\n");
					// Send data size
					size_t size = msg.size();
					serializer.rewind();
					serializer.serialize(size);
					m_pSock->Send(serializer.data(), serializer.size());

					DebugLog("[Thread SendToServer] Sended data length: %d\n", size);

					// Send data
					m_pSock->Send(msg.c_str(), size);

					DebugLog("[Thread SendToServer] Successful!\n");
				}
			}
			catch (exception e)
			{
				m_running = false;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		DebugLog("[Thread SendToServer] Thread terminated !\n");
	}

	void Client::RunRecvFromServer()
	{
		DebugLog("[Thread RecvFromServer] Thread started !\n");
		Serializer serializer(sizeof(size_t), Serializer::Mode::Read);
		serializer.resize(sizeof(size_t));
		while (m_running)
		{
			try
			{
				// Recv data size for next recv
				size_t size = 0;
				m_pSock->Recv(serializer.data(), serializer.size());
				serializer.rewind();
				serializer.serialize(size);

				DebugLog("[Thread RecvFromServer] Receiving data from server...\n");
				// Recv data
				string str(size, 0);
				char* buffer = (char*)malloc(sizeof(char) * size);
				m_pSock->Recv(buffer, size);
				str.assign(buffer, size);
				free(buffer);

				AddMsgToReceivingQueue(str);
				DebugLog("[Thread RecvFromServer] Successful!\n");
			}
			catch (exception e)
			{
				DebugLog("[Thread RecvFromServer] Server connection lost...\n");
				m_running = false;
			}
		}
		DebugLog("[Thread RecvFromServer] Thread terminated !\n");
	}

	void Client::AddMsgToSendingQueue(const string & _msg)
	{
		m_mutToSendQueue.lock();
		m_toSend.push(_msg);
		m_mutToSendQueue.unlock();
	}

	bool Client::GetMsgFromSendingQueue(string & _msg)
	{
		bool sending = false;
		m_mutToSendQueue.lock();
		if (!m_toSend.empty())
		{
			_msg = std::move(m_toSend.front());
			m_toSend.pop();
			sending = true;
		}
		m_mutToSendQueue.unlock();
		return sending;
	}

	void Client::AddMsgToReceivingQueue(const string & _msg)
	{
		m_mutRecvFromQueue.lock();
		m_recvFrom.push(_msg);
		m_mutRecvFromQueue.unlock();
	}

	bool Client::GetMsgFromReceivingQueue(string & _msg)
	{
		bool received = false;
		m_mutRecvFromQueue.lock();
		if (!m_recvFrom.empty())
		{
			_msg = std::move(m_recvFrom.front());
			m_recvFrom.pop();
			received = true;
		}
		m_mutRecvFromQueue.unlock();
		return received;
	}
}
