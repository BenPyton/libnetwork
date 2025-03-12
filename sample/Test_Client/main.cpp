// Network API Example: Simple Chat Client

#include <Network.h>
#include <regex> // to check IP format

bool connected = false;

void OnMessageReceived(net::Socket* _sock, std::string _msg)
{
	// Simply print the received message
	std::cout << _msg << std::endl;
}

void OnConnection(net::Socket* _server)
{
	std::cout << "Connected to the server!" << std::endl << std::endl << std::endl;
}

void OnDisconnection(net::Socket* _server)
{
	std::cout << "Lost connection with the server..." << std::endl;
	connected = false;
}

// Ask the user to enter a value of a certain type
template<typename T>
T Ask(std::string question)
{
	T value = 0;
	bool good = false;

	// We loop until the user enter a number
	do
	{
		// We ask the question and get the user input
		std::cout << question;
		std::cin >> value;

		// we clear cin when a number is not entered
		good = !std::cin.fail();
		std::cin.clear();
		std::cin.ignore(INT_MAX, '\n');
	} while (!good);

	return value;
}
std::string Ask(std::string question)
{
	std::string value;
	bool good = false;

	// We loop until the user enter a number
	do
	{
		// We ask the question and get the user input
		std::cout << question;
		getline(std::cin, value);

		// we clear cin when a number is not entered
		good = !std::cin.fail();
	} while (!good);

	return value;
}
std::string AskIP(std::string question)
{
	std::string value;
	bool good = false;

	regex r("[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}");

	// We loop until the user enter a number
	do
	{
		// We ask the question and get the user input
		std::cout << question;
		getline(std::cin, value);

		good = regex_match(value, r);
	} while (!good);

	return value; // wstring(value.begin(), value.end());
}

int main()
{
	net::Socket::Init();

	net::Client client;

	client.onConnection().subscribe(OnConnection);
	client.onDisconnection().subscribe(OnDisconnection);
	client.onMessageReceived().subscribe(OnMessageReceived);
	std::string username = Ask("username: ");

	do // Try to connect to the server
	{
		std::string addr = AskIP("Enter server address: ");
		UINT16 port = Ask<UINT16>("Enter server port: ");

		try
		{
			client.connect(addr, port);
			connected = true;
		}
		catch (net::SocketException _e)
		{
			std::cout << "Can't connect to " << addr << ":" << port << std::endl;
			connected = false;
		}
	} while (!connected);

	std::string message;
	while (client.isRunning()) // main loop
	{
		getline(std::cin, message);

		// An empty string is entered or connection is lost, then disconnect the client properly
		if (message.empty() || !connected)
		{
			client.disconnect();
		}
		else // send the message to the server
		{
			client.send("[" + username + "] " + message);
		}
	}

	net::Socket::Quit();

	std::cout << "Program Terminated." << std::endl;

	system("pause");
	return 0;
}
