#include<iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<thread>
#include<string>

using namespace std;

#pragma comment(lib, "ws2_32.lib")

/*

	initialize winsock

	create socket

	connect to the server

	send/receive

	close the socket

*/

bool Initialize() {

	WSADATA data;
	return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}

void SendMessageToServer(SOCKET s) {

	cout << "Enter your chat name : " << endl;
	string name;
	getline(cin, name);
	string message;

	while (1) {
		getline(cin, message);
		string msg = name + " : " + message;
		int bytesent = send(s, msg.c_str(), msg.length(), 0);
		if (bytesent == SOCKET_ERROR) {
			cout << "Error sending message\n";
			break;
		}

		if (message == "quit") {
			cout << "Stopping the application!\n";
			break;
		}
	}

	closesocket(s);
	WSACleanup();
}

void ReceiveMessageFromServer(SOCKET s) {

	char buffer[4096];
	int recvlength;
	string msg = "";

	while (1) {
		recvlength = recv(s, buffer, sizeof(buffer), 0);
		if (recvlength <= 0) {
			cout << "Disconnected from the server\n";
			break;
		}
		else {
			msg = string(buffer, recvlength);
			cout << msg << endl;
		}
	}

	closesocket(s);
	WSACleanup();
}

int main() {

	if (!Initialize()) {
		cout << "initialize winsock failed\n";
		return 1;
	}

	SOCKET s;
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET) {
		cout << "invalid socket created\n";
		return 1;
	}

	string serveraddress = "127.0.0.1";
	int port = 12345;
	sockaddr_in serveraddr;
	
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	inet_pton(AF_INET, serveraddress.c_str(), &(serveraddr.sin_addr));

	if(connect(s, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR){
		cout << "Not be able to connect to the server\n";
		closesocket(s);
		WSACleanup();
		return 1;
	}

	cout << "Successfully connected to the server!\n";

	thread senderThread(SendMessageToServer, s);
	thread receiverThread(ReceiveMessageFromServer, s);
	 
	senderThread.join();
	receiverThread.join();

	/*/send/receive
	string message = "Hello there!";
	int bytesent = send(s, message.c_str(), message.length(), 0);
	if (bytesent == SOCKET_ERROR) {
		cout << "Send failed!\n";
	}
	*/
	closesocket(s);
	WSACleanup();

	return 0;
}