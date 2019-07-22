// SocketPenduServer.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <vector>
#include "Types.h"
#include <map>
#include <Network.h>

// Protocol
// 1er msg S->C char = nb de lettre
// msg C->S char (lettre)
// msg S->V bool[char] (gagné? et tableau de position)

using namespace std;

struct ClientData {
	vector<bool> discoveries;
	string pseudo = "";
	int points = 0;
	bool finish = false;
	int tries = 5;
};

vector<char> compareWord(string word, char c); 
bool checkCompletness(const vector<bool>& _arr);
void lowercase(string& _str);
void ClientAdded(net::Socket* client);
void ClientDisconneted(net::Socket* client);

string word; // Not accessed by thread
map<net::Socket*, ClientData> clientData; // not accessed by threads, just global for callback functions
mutex mutClientData;
net::Server server;

int main()
{
	wstring addr = L"127.0.0.1";
	unsigned short port = 50000;

	net::Socket::Init();

	int nbClient = 0;

	cout << "How many players ? ";
	cin >> nbClient;
	cin.clear();
	cin.ignore(INT_MAX, '\n');

	cout << "What is the word ? ";
	cin >> word;
	cin.clear();
	cin.ignore(INT_MAX, '\n');
	lowercase(word);
	DebugLog("Word: %s\n\n\n", word.c_str());

	server.launch(addr, port, net::Protocol::TCP);
	server.OnClientConnection().subscribe(ClientAdded);
	server.OnClientDisconnection().subscribe(ClientDisconneted);

	char letter;
	net::Socket* client = nullptr; // store client socket from msg polling

	// wait for all clients
	DebugLog("Wait for all client connection...\n");
	while (server.nbClient() < nbClient) { Sleep(1); }

	size_t size = word.size();
	DebugLog("Broadcast word length of %d...\n", size);
	server.broadcast(size);

	while (true)
	{
		while (server.pollMsg(letter, client))
		{
			DebugLog("Recieved letter from client: %c\n", letter);

			Data data;

			// compare letter in the word
			data.pos = compareWord(word, letter);
			if (data.pos.size() > 0)
			{
				DebugLog("Letters are at positions : ");
				for (size_t i = 0; i < data.pos.size(); i++)
				{
					DebugLog("%d, ", (int)data.pos[i]);
					clientData[client].discoveries[(int)data.pos[i]] = true;
				}
				DebugLog("\n");

				// Check if word is fully discovered
				data.success = checkCompletness(clientData[client].discoveries);
			}
			else
			{
				DebugLog("There is no occurency of this letter in the word.\n");
				data.success = false;
				clientData[client].tries--;
			}

			DebugLog("Sending letter positions to client...\n");
			server.send(client, data);
		} 

		Sleep(1);
	}

	printf("\n\n\t\tEnd game!\n\n");

	system("pause");

	server.shutdown();
	net::Socket::Quit();
    return 0;
}

void ClientAdded(net::Socket* client)
{
	string pseudo;

	// Get client pseudo
	DebugLog("[Callback ClientAdded] Waiting client pseudo...\n");
	server.waitMsg(pseudo, client);

	DebugLog("[Callback ClientAdded] Connected client: %s\n", pseudo.c_str());

	mutClientData.lock();
	clientData[client] = {
		vector<bool>(word.size(), false),
		pseudo,
		0,
		false,
		5
	};
	mutClientData.unlock();

	DebugLog("[Callback ClientAdded] There are currently %d connected clients.\n", server.nbClient());
}

void ClientDisconneted(net::Socket* client)
{
	DebugLog("[Callback ClientDisconnected] Client disconnected: %s\n", clientData[client].pseudo.c_str());
	mutClientData.lock();
	clientData.erase(client);
	mutClientData.unlock();
	DebugLog("[Callback ClientDisconnected] There are currently %d connected clients.\n", server.nbClient());
}


//////////////////////////////////////////////////
//			Specifique au Jeu du Pendu			//
//////////////////////////////////////////////////


vector<char> compareWord(string word, char c)
{
	vector<char> pos;
	for (size_t i = 0; i < word.length(); i++)
	{
		if (c == word[i])
		{
			pos.push_back((char)i);
		}
	}
	return pos;
}


bool checkCompletness(const vector<bool>& _arr)
{
	bool complete = true;
	for (size_t i = 0; i < _arr.size(); i++)
	{
		if (_arr[i] == false)
		{
			complete = false;
			break;
		}
	}
	return complete;
}

void lowercase(string& _str)
{
	for (size_t i = 0; i < _str.length(); i++)
	{
		_str[i] = tolower(_str[i]);
	}
}
