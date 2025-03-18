// Copyright (c) 2019 Benoit Pelletier
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#include <Network.h>

net::Server server;

void OnClientConnection(net::Socket* _client)
{
	std::string ip = _client->GetIP();

	std::cout << "[" << ip << "] Connected." << std::endl;
	server.broadcast("[" + ip + "] join the chat !");
}

void OnClientDisconnection(net::Socket* _client)
{
	std::string ip = _client->GetIP();

	std::cout << "[" << ip << "] Disconnected." << std::endl;
	server.broadcast("[" + ip + "] quit the chat.");
}

void OnMessageReceived(net::Socket* _client, std::string _msg)
{
	std::cout << "[" << _client->GetIP() << "] Message received: " << _msg.c_str() << std::endl;
	server.broadcast(_msg.c_str());
}

int main()
{
	unsigned short port = 50000;
	int returnCode = 0;

	net::Socket::Init();

	std::string host = net::Socket::GetHostName();
	std::vector<std::string> localIPs = net::Socket::GetLocalIP(host, "");

	std::cout << "Host Name: " << host << std::endl;

	if (localIPs.size() <= 0)
	{
		std::cout << "[ERROR] No IP found." << std::endl;
		returnCode = -1;
		goto end;
	}

	for (std::string localIP : localIPs)
	{
		std::cout << "Local IP: " << localIP << std::endl;
	}
	std::cout << "Port: " << port << std::endl;

	// Bind callbacks to the server events (e.g. when a client connects to the server)
	server.onConnection().subscribe(OnClientConnection);
	server.onMessageReceived().subscribe(OnMessageReceived);
	server.onDisconnection().subscribe(OnClientDisconnection);

	// Run all the necessary threads of the server (accept, sendto and recvfrom)
	server.launch(localIPs[0], port);

	while (server.isRunning())
	{
		// Main loop (here we have nothing to do)
		// We just slow down the thread pace to avoid comsuming too much cpu for an empty loop.
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}

end:
	net::Socket::Quit();

	std::cout << "Program Terminated." << std::endl;

	system("pause");
	return returnCode;
}
