/*
 * @author PELLETIER Benoit
 *
 * @file MsgSystem.h
 *
 * @date 18/07/2019
 *
 * @brief Define a base class that manage messages
 *
 */

#ifndef _MSGSYSTEM_H
#define _MSGSYSTEM_H

#include <thread>
#include <mutex>
#include <atomic>
#include <queue>
#include <list>
#include <string>
#include "Event.h"
#include "Exports.h"

namespace net
{
	class Socket;

	class LIBNETWORK_API MsgSystem
	{
	private:
		typedef std::list<Socket*>::iterator SocketIt;

		struct PairClientMessage
		{
			Socket* socket = nullptr;
			std::string message;
		};

		std::list<Socket*> m_sockets;
		std::vector<Socket*> m_connectedSockets;
		std::vector<Socket*> m_disconnectedSockets;

		std::atomic<bool> m_running {false};

		std::thread m_threadSendMsg;
		std::thread m_threadRecvMsg;
		std::thread m_threadEvent;

		std::mutex m_mutSocketList;
		std::mutex m_mutMsgToSendQueue;
		std::mutex m_mutMsgRecvFromQueue;
		std::mutex m_mutConnectedSocket;
		std::mutex m_mutDisconnectedSocket;

		std::queue<PairClientMessage> m_msgToSend;
		std::queue<PairClientMessage> m_msgRecvFrom;

		int m_currentMsgId = 0;

		Event<Socket*, std::string> m_messageReceivedEvent;
		Event<Socket*> m_connectionEvent;
		Event<Socket*> m_disconnectionEvent;

	public:
		// Event triggered when a message is received from any connected socket
		Event<Socket*, std::string>& onMessageReceived() { return m_messageReceivedEvent; }

		// Event triggered when a remote socket is connected
		Event<Socket*>& onConnection() { return m_connectionEvent; }

		// Event triggered when a remote socket is disconnected
		Event<Socket*>& onDisconnection() { return m_disconnectionEvent; }

		inline bool isRunning() { return m_running; }

	protected:
		void RunThreads();
		void WaitEndOfThreads();
		void CloseAllSockets();

		void AddSocket(Socket* _sock);
		void RemoveSocket(SocketIt _it);
		void AddMsgToSendQueue(Socket* _sock, std::string _msg);

		inline void setRunning(bool _running) { m_running = _running; }

	private:
		void _RunSendMessages();
		void _RunRecvMessages();
		void _RunEvent();
	};
} //namespace net

#endif // _MSGSYSTEM_H
