/*
* @author PELLETIER Benoit
*
* @file Server.cpp
*
* @date 18/11/2018
*
* @brief Define server that can be connected with multiple clients
*
*/

#include "Network/Socket.h"
#include "Network/Server.h"

namespace net{
	Server::Server()
	{
	}

	Server::~Server()
	{
		if (m_running)
		{
			shutdown();
		}
	}

	size_t Server::nbClient()
	{
		size_t size = 0;
		m_mutClientVector.lock();
		size = m_clients.size();
		m_mutClientVector.unlock();
		return size;
	}

	void Server::launch(string _addr, unsigned short _port, Protocol _protocol)
	{
		m_pServer = new Socket(_addr, _port, _protocol);
		m_pServer->Bind();
		m_pServer->Listen();

		m_running = true;

		m_threadServerAccept = thread(&Server::RunAcceptClient, this);
	}

	void Server::shutdown()
	{
		m_running = false;
		// wait for client accepting thread to terminate
		m_threadServerAccept.join();

		// close and delete all client sockets
		m_mutClientVector.lock();
		for (vector<Socket*>::iterator it = m_clients.begin(); it != m_clients.end(); ++it)
		{
			(*it)->Close();
			delete *it;
		}
		m_mutClientVector.unlock();

		// close and delete server socket
		m_pServer->Close();
		delete m_pServer;
	}

	void Server::RunSendToClient(Socket* client)
	{
		DebugLog("[Thread SendToClient] Thread started !\n");
		bool sending = false;
		Message msg;
		Serializer serializer(sizeof(size_t), Serializer::Mode::Write);
		serializer.resize(sizeof(size_t));
		while (m_running && _ClientIsRunning(client))
		{
			try {
				sending = false;
				// Pop msg to send from queue if any
				/*m_mutToSendQueue.lock();
				if (!m_toSend.empty() && m_toSend.front().client == client)
				{
					msg = m_toSend.front();
					m_toSend.pop();
					sending = true;
				}
				m_mutToSendQueue.unlock();*/
				sending = GetMsgFromSendingQueue(msg, client);

				if (sending)
				{
					DebugLog("[Thread SendToClient] Send to client #%p\n", client);
					// Send data size
					size_t size = msg.msg.size();
					DebugLog("[Thread SendToClient] Sending data length: %d\n", size);
					serializer.rewind();
					serializer.serialize(size);
					msg.client->Send(serializer.data(), serializer.size());
					// Send data
					msg.client->Send(msg.msg.c_str(), size);
					DebugLog("[Thread SendToClient] Successful !\n");
				}
			}
			catch (exception e)
			{
				// Here stop client threads
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		DebugLog("[Thread SendToClient] Thread terminated !\n");
	}

	void Server::RunRecvFromClient(Socket* client)
	{
		DebugLog("[Thread RecvFromClient] Thread started !\n");
		Serializer serializer(sizeof(size_t), Serializer::Mode::Read);
		serializer.resize(sizeof(size_t));
		while (m_running && _ClientIsRunning(client))
		{
			try {
				// Recv data size for next recv
				size_t size;
				client->Recv(serializer.data(), serializer.size());
				serializer.rewind();
				serializer.serialize(size);

				DebugLog("[Thread RecvFromClient] Receive from client #%p\n", client);

				// Recv data
				string str(size, 0);
				char* buffer = (char*)malloc(sizeof(char) * size);
				client->Recv(buffer, size);
				str.assign(buffer, size);
				free(buffer);

				DebugLog("[Thread RecvFromClient] Received data length from client: %d\n", str.size());
				DebugLog("[Thread RecvFromClient] Received data from client: %s\n", str.c_str());

				// Push recieved msg to queue
				/*Message msg = { client, str };
				m_mutRecvFromQueue.lock();
				m_recvFrom.push(msg);
				m_mutRecvFromQueue.unlock();*/
				AddMsgToReceivingQueue({ client, str });
			}
			catch (exception e)
			{
				printf("[Thread RecvFromClient] Client #%p disconnected !\n", client);
				m_clientDisconnectionEvent.emit(client);
				// Remove client from client vector
				m_mutClientVector.lock();
				for (vector<Socket*>::iterator it = m_clients.begin(); it != m_clients.end(); ++it)
				{
					if ((*it) == client)
					{
						m_clients.erase(it);
						break;
					}
				}
				m_mutClientVector.unlock();
				client->Close(); // close socket just in case
				delete client; // Free allocated memory
			}
		}
		DebugLog("[Thread RecvFromClient] Thread terminated !\n");
	}

	void Server::RunAcceptClient()
	{
		DebugLog("[Thread AcceptClient] Thread started !\n");
		while (m_running)
		{
			DebugLog("[Thread AcceptClient] Waiting client connection...\n");
			Socket* sock = new Socket(m_pServer->Accept());

			m_mutClientVector.lock();
			m_clients.push_back(sock);
			m_mutClientVector.unlock();

			thread sendThread(&Server::RunSendToClient, this, sock);
			thread recvThread(&Server::RunRecvFromClient, this, sock);

			// detach thread so they will run on their own
			sendThread.detach();
			recvThread.detach();

			printf("[Thread AcceptClient] Client #%p connected !\n", sock);
			m_clientConnectionEvent.emit(sock);
		}
		DebugLog("[Thread AcceptClient] Thread terminated !\n");
	}

	void Server::AddMsgToSendingQueue(const Message & _msg)
	{
		m_mutToSendQueue.lock();
		m_toSend.push(_msg);
		m_mutToSendQueue.unlock();
	}

	bool Server::GetMsgFromSendingQueue(Message & _msg, Socket* _client)
	{
		bool sending = false;
		m_mutToSendQueue.lock();
		if (!m_toSend.empty() && (nullptr == _client || m_toSend.front().client == _client))
		{
			_msg = m_toSend.front();
			m_toSend.pop();
			sending = true;
		}
		m_mutToSendQueue.unlock();
		return sending;
	}

	void Server::AddMsgToReceivingQueue(const Message & _msg)
	{
		m_mutRecvFromQueue.lock();
		m_recvFrom.push(_msg);
		m_mutRecvFromQueue.unlock();
	}

	bool Server::GetMsgFromReceivingQueue(Message & _msg, Socket* _client)
	{
		bool received = false;
		m_mutRecvFromQueue.lock();
		if (!m_recvFrom.empty() && (nullptr == _client || m_recvFrom.front().client == _client))
		{
			_msg = std::move(m_recvFrom.front());
			m_recvFrom.pop();
			received = true;
		}
		m_mutRecvFromQueue.unlock();
		return received;
	}

	bool Server::_ClientIsRunning(Socket * _client)
	{
		bool isClient = false;
		m_mutClientVector.lock();
		for (std::vector<Socket*>::iterator it = m_clients.begin(); it != m_clients.end(); ++it)
		{
			if (*it == _client)
			{
				isClient = true;
				break;
			}
		}
		m_mutClientVector.unlock();
		return isClient;
	}

}
