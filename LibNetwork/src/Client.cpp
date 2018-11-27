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

#include "Network/Client.h"
#include "Network/BinSerializer.h"

namespace net{
	Client::Client()
	{
		m_running = false;
	}

	Client::Client(const Client& _c)
	{
	}

	Client::~Client()
	{
	}

	void Client::connect(wstring _addr, unsigned short _port, Socket::Protocol _protocol)
	{
		m_sock = new Socket(_addr, _port, _protocol); 
		DebugLog("[Client connect] Connecting to server...\n");
		m_sock->Connect();
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

		m_sock->Close();
		delete m_sock;
	}

	void Client::RunSendToServer()
	{
		DebugLog("[Thread SendToServer] Thread started !\n");
		bool sending = false;
		string sendMsg;
		BinSerializer serializer(sizeof(size_t), BinSerializer::Write);
		serializer.resize(sizeof(size_t));
		while (m_running)
		{
			sending = false;
			try
			{
				// Pop msg to send from queue if any
				m_mutToSendQueue.lock();
				if (!m_toSend.empty())
				{
					sendMsg = m_toSend.front();
					m_toSend.pop();
					sending = true;
				}
				m_mutToSendQueue.unlock();

				if (sending)
				{
					DebugLog("[Thread SendToServer] Sending data to server...\n");
					// Send data size
					size_t size = sendMsg.size();
					serializer.rewind();
					serializer.serialize(size);
					m_sock->Send(serializer.data(), serializer.size());

					DebugLog("[Thread SendToServer] Sended data length: %d\n", size);

					// Send data
					m_sock->Send(sendMsg.c_str(), size);

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
		BinSerializer serializer(sizeof(size_t), BinSerializer::Read);
		serializer.resize(sizeof(size_t));
		while (m_running)
		{
			try
			{
				// Recv data size for next recv
				size_t size = 0;
				m_sock->Recv(serializer.data(), serializer.size());
				serializer.rewind();
				serializer.serialize(size);

				DebugLog("[Thread RecvFromServer] Recieving data from server...\n");
				// Recv data
				string str(size, 0);
				char* buffer = (char*)malloc(sizeof(char) * size);
				m_sock->Recv(buffer, size);
				str.assign(buffer, size);
				free(buffer);

				// Push recieved msg to queue
				m_mutRecvFromQueue.lock();
				m_recvFrom.push(str);
				m_mutRecvFromQueue.unlock();
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
}
