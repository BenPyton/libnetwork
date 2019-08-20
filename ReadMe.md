# Simple C++ Network API

### Description :
Simple client-server network API.<br>
Work only on Windows for now, but I will implement cross-platform socket in future.<br>

If you have issues with compilation, check the version of windows' SDK in project properties. <br>

There are two version of client and server classes : the old one (V1) with blocking sockets, and the new one (V2) with non-blocking sockets. <br>
The new one also work with an event system when receiving messages, unlike the old one that use a wait or poll message system.

The new version use a message class to cut strings in fragments that will be sent independently, then reconstruct in the other side.
That was done for the UDP protocol, but actually only TCP is used.

### Use of the new version :
Simple server side application:
```cpp
#include <Network.h>

using namespace net;

void OnMessageReceived(Socket* _client, std::string _msg)
{
	// Process _msg here 
	// ...
}

int main()
{
	Socket::Init();
	
	ServerV2 server;
	server.onMessageReceived().subscribe(OnMessageReceived);
	server.launch("127.0.0.1", 7777);
	
	while(server.isRunning()) // main loop
	{
		// do other stuff here
		
		// stopping server:
		// server.shutdown();

		// send message to a specific client:
		// server.send(clientSocket, msg);
		
		// send message to all clients:
		// server.broadcast(msg);
	}
	
	Socket::Quit();
	return 0;
}
```
<br>Simple client side application:<br>

```cpp 

#include <Network.h>

using namespace net;

void OnMessageReceived(Socket* _sock, std::string _msg)
{
	// _sock will always be the server socket 

	// Process _msg here 
	// ...
}

int main()
{
	Socket::Init();
	
	ClientV2 client;
	client.onMessageReceived().subscribe(OnMessageReceived);

	// connect to the server
	client.connect("127.0.0.1", 7777);

	while(client.isRunning())
	{
		// do other stuff here

		// stopping client:
		// client.disconnect();
		
		// send message to the server:
		// client.send(msg);
	}
	
	Socket::Quit();
	return 0;
}
```

### Use of the old version :
Simple server side application:
```cpp
#include <Network.h>

using namespace net;

int main()
{
	Socket::Init();
	
	Server server;
	server.launch("127.0.0.1", 7777);
	
	// Store client sender from pollMsg
	Socket* client = nullptr;
	
	bool loop = true;
	while(loop) // main loop
	{
		// Loop until there's no message in queue
		// Store recieved data in data and client from which we recieve
		// Data is a basic type (int, float, char, etc.), a string, a vector, or an ISerializable class
		while(server.pollMsg(data, client))
		{
			// Process your data
			// ...
			
			server.send(client, otherData);
		}
	}
	
	server.shutdown();
	Socket::Quit();
	return 0;
}
```
<br>Simple client side application:<br>

```cpp 

#include <Network.h>

using namespace net;

int main()
{
	Socket::Init();
	
	Client client;
	client.connect("127.0.0.1", 7777);
	
	bool loop = true;
	while(loop) // main loop
	{
		client.send(data);

		// block until a msg is recieved from server
		client.waitMsg(otherData); 
		
		// Process data
		// ...
	}
	
	client.shutdown();
	Socket::Quit();
	return 0;
}
```

### Diagrams :
![Explanation of new client-server architecture](images/libnet_new_architecture.jpg?raw=true "New architecture diagram")<br>

![Explanation of old client-server architecture](images/libnet_architecture.png?raw=true "Old architecture diagram")
