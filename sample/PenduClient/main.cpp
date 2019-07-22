// SocketsPendu.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <vector>
#include "Types.h"
#include <Network.h>

using namespace std;

int main()
{
	wstring addr = L"127.0.0.1";
	unsigned short port = 50000;

	net::Socket::Init();

	string pseudo;
	cout << "Pseudo: ";
	cin >> pseudo;
	cin.clear();
	cin.ignore(INT_MAX, '\n');

	DebugLog("Pseudo size: %d\n", pseudo.size());

	net::Client client;
	client.connect(addr, port, net::Protocol::TCP);

	Data data;
	data.success = false;
	char letter;

	DebugLog("Sending pseudo...\n");
	client.send(pseudo);

	// Get word length from server
	DebugLog("Recieving word length...\n");
	size_t size;
	client.waitMsg(size);
	DebugLog("Recieved word length: %d\n", size);

	// Create a masked word from word length
	string maskedWord(size, '_');

	do
	{
		cout << "\n\nWord: " << maskedWord << endl;
		//cout << "Tries left: " << tries << endl;
		cout << "\nEnter a letter: ";
		cin >> letter;
		cin.clear();
		cin.ignore(INT_MAX, '\n');
		letter = tolower(letter); // avoid maj and min comparison


		// send letter to server
		DebugLog("Sending letter to server...\n");
		client.send(letter);

		// Get letters pos from the server
		DebugLog("Recieving letter positions...\n");
		client.waitMsg(data);

		DebugLog("Recieved: [%s] [nb pos: %d] ", data.success ? "true" : "false", data.pos.size());
		for (size_t i = 0; i < data.pos.size(); i++)
		{
			if (data.pos[i] != -1)
			{
				DebugLog("%d, ", (int)data.pos[i]);
				maskedWord[data.pos[i]] = letter;
			}
		}
		DebugLog("\n");

	} while (!data.success);


	if (data.success)
	{
		printf("\n\n\t\t\tYou win!\n\t\tThe word was: %s\n\n", maskedWord.c_str());
	}

	client.shutdown();

	system("pause");

	net::Socket::Quit();

    return 0;
}
