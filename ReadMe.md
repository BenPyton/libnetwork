# Simple C++ Network API

```diff
- WARNING: This project is under development, some features may not work yet.
```

### Description :
Simple client-server network API.<br>
Work only on Windows for now, but I will implement cross-platform socket in future.<br>

If you have issues with compilation, check the version of windows' SDK in project properties. <br>

### Use :
Simple server side application:<br>
```cpp
#include <Network.h>

using namespace net;

int main()
{
	Socket::Init();
	
	Server server;
	server.launch(L"127.0.0.1", 7777);
	
	while(server.pollMsg(data, client))
	{
		// Process your data
		server.send(client, otherData);
	}
	
	server.shutdown();
	Socket::Close();
	return 0;
}
```
<br>
Simple client side application:<br>
```cpp
#include <Network.h>

using namespace net;

int main()
{
	Socket::Init();
	
	Client client;
	client.connect(L"127.0.0.1", 7777);
	
	client.send(data);
	
	// block until a msg is recieved from server
	client.waitMsg(otherData); 
	
	client.shutdown();
	Socket::Close();
	return 0;
}
```


