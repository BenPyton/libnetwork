# Simple C++ Network API

### Description :
Simple client-server network API.<br>
Work only on Windows for now, but I will implement cross-platform socket in future.<br>

If you have issues with compilation in Visual Studio, check the version of windows' SDK in project properties. <br>

Client and server use non-blocking sockets to improve performances (reduce number of threads). \
The reception of messages, as well as connections and disconnections, are processed by events: \
you have to subscribe to the event with a callback function, in which you do what you want.

A message class is used to cut strings in fragments that will be sent independently, then reconstruct the initial message in the other side. \
That was done for the use of UDP protocol, but actually only TCP is used.

There is a binary serializer you can use to convert any serializable data into a string to send it throught the network.

![Explanation of client-server architecture](images/libnet_new_architecture.jpg?raw=true "Client-server architecture diagram")<br>

### Example in video:
[![](http://img.youtube.com/vi/TTELfeXA8a0/0.jpg)](http://www.youtube.com/watch?v=TTELfeXA8a0 "Simple Chat Demo")<br>

### Use :
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

