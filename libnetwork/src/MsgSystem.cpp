/*
 * @author PELLETIER Benoit
 *
 * @file MsgSystem.cpp
 *
 * @date 18/07/2019
 *
 * @brief Define a base class that manage messages
 *
 */

#include "Network/MsgSystem.h"
#include "Network/Socket.h"
#include "Network/Message.h"
#include "Network/SocketException.h"
#include "Utils.h"
#include <algorithm>

namespace net
{
	void MsgSystem::RunThreads()
	{
		m_threadSendMsg = std::thread(&MsgSystem::_RunSendMessages, this);
		m_threadRecvMsg = std::thread(&MsgSystem::_RunRecvMessages, this);
		m_threadEvent = std::thread(&MsgSystem::_RunEvent, this);
	}

	void MsgSystem::WaitEndOfThreads()
	{
		m_threadSendMsg.join();
		m_threadRecvMsg.join();
		m_threadEvent.join();
	}

	void MsgSystem::CloseAllSockets()
	{
		m_mutSocketList.lock();
		for (SocketIt it = m_sockets.begin(); it != m_sockets.end(); ++it)
		{
			(*it)->Close();
			delete *it;
		}
		m_mutSocketList.unlock();
	}

	void MsgSystem::AddSocket(Socket* _sock)
	{
		m_mutSocketList.lock();
		m_sockets.push_back(_sock);
		m_mutSocketList.unlock();

		m_mutConnectedSocket.lock();
		m_connectedSockets.push_back(_sock);
		m_mutConnectedSocket.unlock();
	}

	void MsgSystem::RemoveSocket(SocketIt _it)
	{
		m_mutSocketList.lock();
		m_sockets.erase(_it);
		m_mutSocketList.unlock();
	}

	void MsgSystem::AddMsgToSendQueue(Socket* _sock, std::string _msg)
	{
		m_mutMsgToSendQueue.lock();
		m_msgToSend.push({_sock, _msg});
		m_mutMsgToSendQueue.unlock();
	}

	void MsgSystem::_RunSendMessages()
	{
		PairClientMessage msgToSend;
		bool sendMsg;

		DebugLog("[Thread SendMsg] Thread started !\n");
		while (m_running)
		{
			sendMsg = false;

			m_mutMsgToSendQueue.lock();
			if (!m_msgToSend.empty())
			{
				msgToSend = m_msgToSend.front();
				m_msgToSend.pop();
				sendMsg = true;
			}
			m_mutMsgToSendQueue.unlock();

			if (sendMsg)
			{
				DebugLog("[Thread SendMsg] Sending message #%d with data: %s\n", m_currentMsgId, msgToSend.message.c_str());
				std::vector<Fragment> fragments = Message::FragmentString(m_currentMsgId, msgToSend.message);
				int result = 0;
				for (int i = 0; i < fragments.size(); i++)
				{
					if (msgToSend.socket == NULL) // broadcast
					{
						for (SocketIt it = m_sockets.begin(); it != m_sockets.end(); ++it)
						{
							if ((result = (*it)->Send(reinterpret_cast<const char*>(&fragments[i]), fragments[i].size())) < 0)
							{
								DebugLog("[Thread SendMsg] Error when sending message.\n");
							}
						}
					}
					else
					{
						//result = m_pServer->SendTo(reinterpret_cast<const char*>(&fragments[i]), fragments[i].size(), dstAddr, dstPort);
						if ((result = msgToSend.socket->Send(reinterpret_cast<const char*>(&fragments[i]), fragments[i].size())) < 0)
						{
							DebugLog("[Thread SendMsg] Error when sending message.\n");
						}
					}

					DebugLog("[Thread SendMsg] Fragment #%d sent with result: %d\n", i, result);
				}

				m_currentMsgId++;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		DebugLog("[Thread SendMsg] Thread terminated !\n");
	}

	void MsgSystem::_RunRecvMessages()
	{
		fd_set readfds;
		int activity = 0;
		timeval timeout;

		// list of uncompleted messages, waiting here to be completed
		std::vector<Message> pendingMsg;
		std::vector<SocketIt> pendingClose;

		DebugLog("[Thread RecvMsg] Thread started !\n");
		while (m_running)
		{
			// clear the socket set
			FD_ZERO(&readfds);
			int max_sd = -1;

			// add client sockets to set
			m_mutSocketList.lock();
			for (SocketIt it = m_sockets.begin(); it != m_sockets.end(); ++it)
			{
				SOCKET sock = (*it)->GetSock();
				if (sock > 0) // if valid socket descriptor then add to read list
					FD_SET(sock, &readfds);

				// highest file descriptor number, need it for the select function
				if (sock > max_sd)
					max_sd = sock;
			}
			m_mutSocketList.unlock();

			//wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
			timeout.tv_sec = 0;
			timeout.tv_usec = 100000;									   // 100 ms of timeout
			activity = select(max_sd + 1, &readfds, NULL, NULL, &timeout); // update sockets in file descriptor

			int result = 0;
			for (SocketIt it = m_sockets.begin(); it != m_sockets.end(); ++it)
			{
				Socket* client = *it;
				if (FD_ISSET(client->GetSock(), &readfds))
				{
					// get a fragment
					DebugLog("\n[Thread RecvMsg] Receiving message...\n");
					char buffer[Fragment::max_size] = {0};
					try
					{
						//result = socket->RecvFrom(buffer, net::Fragment::max_size, srcAddr, srcPort);
						result = client->Recv(buffer, Fragment::max_size);
					}
					catch (SocketException& e)
					{
						if (e.code() == 10054) // remote force close connection (eg. close the program window)
						{
							DebugLog("[Thread RecvMsg] Connection closed by remote.\n");
						}
						else
						{
							DebugLog("[Thread RecvMsg] Unknown connection error.\n");
						}
					}

					if (result > 0)
					{
						Fragment fragment;
						memcpy(reinterpret_cast<char*>(&fragment), buffer, Fragment::max_size);

						std::vector<Message>::iterator msg_it = find_if(pendingMsg.begin(), pendingMsg.end(), [&fragment](Message& _msg) -> bool { return _msg.id() == fragment.header.msg_id; });

						if (msg_it == pendingMsg.end()) // not found then create a new msg
						{
							pendingMsg.push_back(std::move(Message(fragment.header.msg_id, fragment.header.nb_frag)));
							msg_it = pendingMsg.end() - 1;
						}

						Message* msg = &*msg_it;

						// add the new fragment to the msg
						msg->addFragment(fragment);

						DebugLog("[Thread RecvMsg] Message #%llu nb frag: %zu/%hu\n", msg->id(), msg->size(), msg->totalFrag());
						//DebugLog("Message Completed: %s\n", msg->isComplete() ? "Yes" : "No");
						if (msg->isComplete())
						{
							std::string recvdMsg = msg->getString();

							m_mutMsgRecvFromQueue.lock();
							m_msgRecvFrom.push({client, recvdMsg});
							m_mutMsgRecvFromQueue.unlock();

							pendingMsg.erase(msg_it);
						}
					}
					else
					{
						pendingClose.push_back(it);

						if (result == 0)
							DebugLog("[Thread RecvMsg] Connection properly closed.\n");
						else
							DebugLog("[Thread RecvMsg] Error while receiving message from [%s].\n", client->GetIP().c_str());
					}
				}
			}

			// remove all disconnected sockets from the socket list
			for (int i = 0; i < pendingClose.size(); i++)
			{
				m_mutDisconnectedSocket.lock();
				m_disconnectedSockets.push_back(*pendingClose[i]);
				m_mutDisconnectedSocket.unlock();
				RemoveSocket(pendingClose[i]);
			}
			pendingClose.clear();

			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		DebugLog("[Thread RecvMsg] Thread terminated !\n");
	}

	void MsgSystem::_RunEvent()
	{
		DebugLog("[Thread Event] Thread started !\n");
		bool recvdMsg = false;
		PairClientMessage pair;

		while (m_running)
		{
			// Trigger Connection Event for each disconnected socket, and delete them
			m_mutConnectedSocket.lock();
			for (int i = 0; i < m_connectedSockets.size(); i++)
			{
				DebugLog("[Thread Event] Emit connected socket.\n");
				m_connectionEvent.emit(m_connectedSockets[i]);
			}
			m_connectedSockets.clear();
			m_mutConnectedSocket.unlock();

			// Trigger Received Message Event
			recvdMsg = false;
			m_mutMsgRecvFromQueue.lock();
			if (!m_msgRecvFrom.empty())
			{
				pair = m_msgRecvFrom.front();
				m_msgRecvFrom.pop();
				recvdMsg = true;
			}
			m_mutMsgRecvFromQueue.unlock();

			if (recvdMsg)
			{
				DebugLog("[Thread Event] Emit receive message.\n");
				m_messageReceivedEvent.emit(pair.socket, pair.message);
			}

			// Trigger Disconnection Event for each disconnected socket, and delete them
			m_mutDisconnectedSocket.lock();
			for (int i = 0; i < m_disconnectedSockets.size(); i++)
			{
				Socket* sock = m_disconnectedSockets[i];
				sock->Close();
				DebugLog("[Thread Event] Emit disconnected socket.\n");
				m_disconnectionEvent.emit(sock);
				delete sock;
			}
			m_disconnectedSockets.clear();
			m_mutDisconnectedSocket.unlock();

			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		DebugLog("[Thread Event] Thread terminated !\n");
	}
} //namespace net
