// SocketPenduServer.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <vector>
#include "Types.h"
#include <map>
#include <Network.h>
#include <Ws2tcpip.h>

using namespace std;

net::ServerV2 server;

void OnClientConnection(net::Socket* _client)
{
	string ip = _client->GetIP();

	printf("[%s] Connected.\n", ip.c_str());
	server.broadcast("[" + ip + "] join the chat !");
}

void OnDisconnection(net::Socket* _client)
{
	string ip = _client->GetIP();

	printf("[%s] Disconnected.\n", ip.c_str());
	server.broadcast("[" + ip + "] quit the chat.");
}

void OnMessageReceived(net::Socket* _client, string _msg)
{
	cout << "[" << _client->GetIP() << "] Message received: " << _msg.c_str() << endl;
	server.broadcast(_msg.c_str());
}

int main()
{
	//string addr = "127.0.0.1";
	unsigned short port = 50000;

	net::Socket::Init();

	string host = net::Socket::GetHostName();
	vector<string> localIPs = net::Socket::GetLocalIP(host, "");

	cout << "Host Name: " << host << endl;
	for (string localIP : localIPs)
	{
		cout << "Local IP: " << localIP << endl;
	}

	bool running = true;
	string dataToSend;

	// bind a function to the event when a client connect to the server
	server.onConnection().subscribe(OnClientConnection);
	server.onMessageReceived().subscribe(OnMessageReceived);
	server.onDisconnection().subscribe(OnDisconnection);

	// run all the necessary threads of the server (accept, sendto and recvfrom)
	server.launch(localIPs[0], port);

	while (server.isRunning());

	net::Socket::Quit();

	cout << "Program Terminated." << endl;

	system("pause");
    return 0;
}

