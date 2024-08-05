#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <tchar.h>
#include<thread>
#include<vector>

using namespace std;

#pragma comment(lib, "ws2_32.lib")
/*

	// initialize winsock library
		
	// create the socket
	
	// get ip and port
	
	// bind ip/port with the socket
	
	// listen on the socket
	
	// accept
	
	// receive and send
	
	// close the socket

	// cleanup the winsock
*/


bool Initialize() {

	WSADATA data; 
	return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}

void InteractWithClient(SOCKET clientSocket, vector<SOCKET>& clients) {
	//send/receive client

	cout << "client connected\n";
	char buffer[4096];

	while (1) {
		int byterecvd = recv(clientSocket, buffer, sizeof(buffer), 0);

		if (byterecvd <= 0) {
			cout << "client disconnected\n";
			break;
		}
		//1. pokazivac na prvi element
		//2. broj bajtova koji su primljeni
		string message(buffer, byterecvd);
		cout << "Message from client : " << message << endl;

		for (auto client : clients) {
			if (client != clientSocket) {
				send(client, message.c_str(), message.length(), 0);
			}	
		}
	}
	
	auto it = find(clients.begin(), clients.end(), clientSocket);
	if (it != clients.end()) {
		clients.erase(it);
	}
	closesocket(clientSocket);
}
int main() {
	
	if (!Initialize()) {
		cout << "winsock initializaztion failed\n";
		return 1;
	}

	cout << "server program\n";

	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET) {
		cout << "socket creation failed\n";
		return 1;
	}

	//Postavljanje adrese i porta 
	int port = 12345;
	sockaddr_in serveraddr;	//kreiranje strukture
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);	// htons konvertuje broj porta iz host byte order u network byte order 

	//convert the ipaddress (0.0.0.0) put it inside sin_family in binary format
	if(InetPton(AF_INET, _T("0.0.0.0"), &serveraddr.sin_addr) != 1) {
		cout << "settings address structure failed " << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	//bind
	if (bind(listenSocket, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR) {
		cout << "bind failed\n";
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	//listen
	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		cout << "listen failed\n";
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	cout << "Server has started listening on port : " << port << endl;
	vector<SOCKET> clients;

	while (1) {
		//accept
		SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
		if (clientSocket == INVALID_SOCKET) {
			cout << "Invalid client socket\n";
		}
		clients.push_back(clientSocket);
		thread t1(InteractWithClient, clientSocket, ref(clients));
		t1.detach();
	}
	

	closesocket(listenSocket);
	WSACleanup();

	return 0;
}