# Simple C++ Network API

![GitHub License](https://img.shields.io/github/license/BenPyton/LibNetwork)
[![Inactively Maintained](https://img.shields.io/badge/Maintenance%20Level-Inactively%20Maintained-yellowgreen.svg)](https://gist.github.com/cheerfulstoic/d107229326a01ff0f333a1d3476e068d)

## Description

Simple client-server network API.  

> [!WARNING]
> Works only on Windows currently. It will need some additions to manage other platform-specific sockets.  

Client and server use non-blocking sockets to improve performances (reduce number of threads).  
The reception of messages, as well as connections and disconnections, are processed by events: you have to subscribe to the event with a callback function, in which you do what you want.

A message class is used to cut strings in fragments that will be sent independently, then reconstruct the initial message in the other side.  
That was done for the use of UDP protocol, but actually only TCP is used.

There is a binary serializer you can use to convert any serializable data into a string to send it through the network.

![Explanation of client-server architecture](images/libnet_new_architecture.jpg?raw=true "Client-server architecture diagram")

## Demo Video

[![Demo Video](http://img.youtube.com/vi/TTELfeXA8a0/0.jpg)](http://www.youtube.com/watch?v=TTELfeXA8a0 "Simple Chat Demo")

## Usage

Simple server side application:

```cpp
#include <Network.h>

void OnMessageReceived(net::Socket* _client, std::string _msg)
{
    // Process _msg here 
    // ...
}

int main()
{
    net::Socket::Init();

    net::Server server;
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

    net::Socket::Quit();
    return 0;
}
```

Simple client side application:

```cpp
#include <Network.h>

void OnMessageReceived(net::Socket* _sock, std::string _msg)
{
    // _sock will always be the server socket 

    // Process _msg here 
    // ...
}

int main()
{
    net::Socket::Init();
    
    net::Client client;
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

    net::Socket::Quit();
    return 0;
}
```

## License

This simple library is under the [Boost Software License 1.0](LICENSE) (BSL-1.0).
