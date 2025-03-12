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
#include "Event.h"

using namespace std;

namespace net
{

	class Socket;

	class MsgSystem
	{
	protected:
		typedef list<Socket*>::iterator SocketIt;

		struct PairClientMessage
		{
			Socket* socket;
			string message;
		};

		list<Socket*> m_sockets;
		vector<Socket*> m_connectedSockets;
		vector<Socket*> m_disconnectedSockets;

		std::atomic<bool> m_running = false;

	private:
		thread m_threadSendMsg;
		thread m_threadRecvMsg;
		thread m_threadEvent;

		mutex m_mutSocketList;
		mutex m_mutMsgToSendQueue;
		mutex m_mutMsgRecvFromQueue;
		mutex m_mutConnectedSocket;
		mutex m_mutDisconnectedSocket;

		queue<PairClientMessage> m_msgToSend;
		queue<PairClientMessage> m_msgRecvFrom;

		int m_currentMsgId = 0;

		Event<Socket*, string> m_messageReceivedEvent;
		Event<Socket*> m_connectionEvent;
		Event<Socket*> m_disconnectionEvent;

	public:
		MsgSystem();
		~MsgSystem();

		// Event triggered when a message is received from any connected socket
		Event<Socket*, string>& onMessageReceived() { return m_messageReceivedEvent; }

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
		void AddMsgToSendQueue(Socket* _sock, string _msg);

	private:
		void _RunSendMessages();
		void _RunRecvMessages();
		void _RunEvent();
	};
} //namespace net

#endif // _MSGSYSTEM_H
