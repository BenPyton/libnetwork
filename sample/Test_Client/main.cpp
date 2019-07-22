// SocketsPendu.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <vector>
#include "Types.h"
#include <Network.h>
#include <regex>


using namespace std;

net::ClientV2 client;
string username;

void OnMessageReceived(net::Socket* _sock, string _msg)
{
	//wcout << "[" << _sock->GetIP() << "] Message received: " << _msg.c_str() << endl;
	//cout << _msg << endl;
	printf("%s\n", _msg.c_str());
}

void OnConnection(net::Socket* _server)
{
	//client.send(username + " has joined !");
	printf("Connected to the server!\n\n\n");
}

// Ask the user to enter an int
template<typename T> T Ask(string question)
{
	T value = 0;
	bool good = false;

	// We loop until the user enter a number
	do
	{
		// We ask the question and get the user input
		cout << question;
		cin >> value;

		// we clear cin when a number is not entered
		good = !cin.fail();
		cin.clear();
		cin.ignore(INT_MAX, '\n');
	} while (!good);

	return value;
}
string Ask(string question)
{
	string value;
	bool good = false;

	// We loop until the user enter a number
	do
	{
		// We ask the question and get the user input
		cout << question;
		getline(cin, value);

		// we clear cin when a number is not entered
		good = !cin.fail();
	} while (!good);

	return value;
}
string AskIP(string question)
{
	string value;
	bool good = false;

	regex r("[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}");

	// We loop until the user enter a number
	do
	{
		// We ask the question and get the user input
		cout << question;
		getline(cin, value);

		good = regex_match(value, r);
	} while (!good);

	return value;// wstring(value.begin(), value.end());
}

int main()
{
	bool connected = false;

	net::Socket::Init();

	client.onConnection().subscribe(OnConnection);
	client.onMessageReceived().subscribe(OnMessageReceived);
	username = Ask("username: ");


	do 
	{
		string addr = AskIP("Enter server address: ");
		UINT16 port = Ask<UINT16>("Enter server port: ");

		try
		{
			client.connect(addr, port);
			connected = true;
		}
		catch (net::SocketException _e)
		{
			cout << "Can't connect to " << addr << ":" << port << endl;
			connected = false;
		}
	} while (!connected); 


	string data;
	while (client.isRunning())
	{
		getline(cin, data);

		if (data.empty())
		{
			//client.send(username + " has quit.");
			client.disconnect();
		}
		else
		{
			client.send("[" + username + "] " + data);
		}
	}

	net::Socket::Quit();

	cout << "Program Terminated." << endl;

	system("pause");
    return 0;
}

//using namespace std;
//
//void RunThread(net::Socket* socket); 
//void RunSendThread(net::Socket* socket);
//
//atomic<bool> running = true;
//
//queue<string> toSendMsg;
//mutex mutToSendMsg;
//
//queue<string> RecvMsg;
//mutex mutRecvMsg;
//
//int main()
//{
//	wstring addr = L"127.0.0.1";
//	unsigned short port = 50000;
//
//
//	net::Socket::Init();
//
//	net::Socket socket(addr, port, net::Protocol::TCP);
//	//socket.Bind();
//
//	DebugLog("Try to connect to server...\n");
//	socket.Connect();
//	DebugLog("Successfully connected to the server!\n");
//
//	// Launch the socket read/write thread
//	thread t(&RunThread, &socket);
//	thread sendThread(&RunSendThread, &socket);
//
//	string data;
//	while (running)
//	{
//		cout << "Data to send: ";
//		getline(cin, data);
//
//		if (data.empty())
//		{
//			running = false;
//		}
//		else
//		{
//			mutToSendMsg.lock();
//			toSendMsg.push(data);
//			mutToSendMsg.unlock();
//		}
//
//		/*while (!RecvMsg.empty())
//		{
//			mutRecvMsg.lock();
//			string msg = RecvMsg.back();
//			RecvMsg.pop();
//			mutRecvMsg.unlock();
//
//			cout << "Received message: \"" << msg << "\"" << endl;
//		}*/
//	}
//
//	// wait end of thread to continue
//	t.join();
//	sendThread.join();
//
//	socket.Close();
//	net::Socket::Quit();
//
//	cout << "Program Terminated." << endl;
//
//	system("pause");
//    return 0;
//}
//
//void RunSendThread(net::Socket* socket)
//{
//	int currentMessageId = 0;
//	bool sendMsg = false; 
//	string msg;
//
//	DebugLog("[Send Thread] Thread start.\n");
//	while (running)
//	{
//		sendMsg = false;
//
//		mutToSendMsg.lock();
//		if (!toSendMsg.empty())
//		{
//			msg = toSendMsg.front();
//			toSendMsg.pop();
//			sendMsg = true;
//		}
//		mutToSendMsg.unlock();
//
//
//		if (sendMsg)
//		{
//			DebugLog("[Send Thread] Sending message #%d with data: %s\n", currentMessageId, msg.c_str());
//			vector<net::Fragment> fragments = net::Message::FragmentString(currentMessageId, msg);
//			int result = 0;
//			for (int i = 0; i < fragments.size(); i++)
//			{
//				result = socket->Send(reinterpret_cast<const char*>(&fragments[i]), fragments[i].size());
//				DebugLog("[Thread SendToClient] Fragment #%d send with result: %d\n", i, result);
//				if (result < 0)
//				{
//					DebugLog("[Thread SendToClient] Error When Sending Message.\n");
//				}
//			}
//
//			currentMessageId++;
//		}
//	}
//	DebugLog("[Send Thread] Thread terminated.\n");
//}
//
//void RunThread(net::Socket* socket)
//{
//	DebugLog("[Recv Thread] Thread start.\n");
//	wstring srcAddr;
//	unsigned short srcPort;
//
//	fd_set readfiledescset; // creer un set de file descripteur (utiliser pour la lecture)
//	fd_set writefiledescset; // create a file descriptor set to write
//	fd_set exceptfiledescset; // create a file descriptor set to write
//	timeval timeout;
//
//	vector<net::Message> msgs;
//	int result = 0;
//
//	string data;
//
//	int selected = 0;
//	while (running)
//	{
//		FD_ZERO(&readfiledescset); // vide le set de lecture
//		FD_SET(socket->GetSock(), &readfiledescset); // ajoute le socket d'ecoute dans le descripteur
//
//		FD_ZERO(&exceptfiledescset); // vide le set de lecture
//		FD_SET(socket->GetSock(), &exceptfiledescset); // ajoute le socket d'ecoute dans le descripteur
//
//		/*FD_ZERO(&writefiledescset);
//		if (!toSendMsg.empty())
//		{
//			FD_SET(socket->GetSock(), &writefiledescset);
//		}*/
//
//
//		timeout.tv_sec = 0;
//		timeout.tv_usec = 500 * 1000;
//		selected = select(socket->GetSock() + 1, &readfiledescset, NULL, &exceptfiledescset, &timeout);
//		if (selected < 0)
//		{
//			cout << "Error select file descriptor..." << endl;
//		}
//
//		// receive msg
//		if (FD_ISSET(socket->GetSock(), &readfiledescset))
//		{
//			if (FD_ISSET(socket->GetSock(), &exceptfiledescset))
//			{
//				running = false;
//				DebugLog("Connection error.\n");
//			}
//			else
//			{
//
//				// get a fragment
//				DebugLog("\nWaiting data from server...\n");
//				char buffer[net::Fragment::max_size] = { 0 };
//				//result = socket->RecvFrom(buffer, net::Fragment::max_size, srcAddr, srcPort);
//				result = socket->Recv(buffer, net::Fragment::max_size);
//				//DebugLog("Receive data from %s:%d\n", srcAddr.c_str(), srcPort);
//
//				if (result > 0)
//				{
//					net::Fragment fragment;
//					memcpy(reinterpret_cast<char*>(&fragment), buffer, net::Fragment::max_size);
//
//					string data;
//					data.assign(fragment.data.data(), fragment.header.data_size);
//					uint64_t id = fragment.header.frag_id;
//
//					DebugLog("Fragment #%d received with result: %d\n", id, result);
//					DebugLog("Data: \"%s\"\n", data.c_str());
//
//
//					vector<net::Message>::iterator it = find_if(msgs.begin(), msgs.end(),
//						[=](net::Message& _msg) -> bool { return _msg.id() == fragment.header.msg_id; });
//					//net::Message* msg = nullptr;
//
//					if (it == msgs.end()) // not found then create a new msg
//					{
//						msgs.push_back(std::move(net::Message(fragment.header.msg_id, fragment.header.nb_frag)));
//						it = msgs.end() - 1;
//
//						//msg = new net::Message(fragment.header.msg_id, fragment.header.nb_frag);
//					}
//					else // get found msg
//					{
//					}
//
//
//					net::Message* msg = &*it;
//
//					// add the new fragment to the msg
//					msg->addFragment(fragment);
//
//					DebugLog("Message #%d nb frag: %d/%d\n", msg->id(), msg->size(), msg->totalFrag());
//					//DebugLog("Message Completed: %s\n", msg->isComplete() ? "Yes" : "No");
//					if (msg->isComplete())
//					{
//						string recvdMsg = msg->getString();
//						//DebugLog("\n\nMessage received: %s\n", recvdMsg.c_str());
//						RecvMsg.push(recvdMsg);
//						msgs.erase(it);
//					}
//				}
//				else if (result == 0)
//				{
//					running = false;
//					DebugLog("Connection closed.\n");
//				}
//				else
//				{
//					DebugLog("Error while receiving data from server.\n");
//				}
//			}
//		}
//
//		//// send msg
//		//if (FD_ISSET(socket->GetSock(), &writefiledescset))
//		//{
//		//	while (!toSendMsg.empty())
//		//	{
//		//		mutToSendMsg.lock();
//		//		data = toSendMsg.back();
//		//		toSendMsg.pop();
//		//		mutToSendMsg.unlock();
//		//	}
//		//}
//
//		cout << ".";
//	}
//
//	DebugLog("[Recv Thread] Thread terminated.\n");
//}