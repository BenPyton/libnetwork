# Simple C++ Network API

### Description :
Simple client-server network API.<br>
Work only on Windows for now, but I will implement cross-platform socket in future.<br>

If you have issues with compilation, check the version of windows' SDK in project properties. <br>

### Use :
Simple server side application:
```cpp
#include <Network.h>

using namespace net;

int main()
{
	Socket::Init();
	
	Server server;
	server.launch(L"127.0.0.1", 7777);
	
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
	Socket::Close();
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
	client.connect(L"127.0.0.1", 7777);
	
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
	Socket::Close();
	return 0;
}
```
### Diagram :
![Explanation of client-server architecture](images/libnet_architecture.png?raw=true "Architecture diagram")
