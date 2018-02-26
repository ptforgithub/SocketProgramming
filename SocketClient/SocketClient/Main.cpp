// Windows Socket program using regular sockets (not raw)
// Create a TCP client that binds to a specific port
// Usually clients do not bother which port is assigned to them by the OS
// Send a GET request to google


#include "Main.h"
#include <iostream>>

// link with Ws2_32.lib
#pragma comment(lib,"Ws2_32.lib")
#include <winsock2.h>
#include <ws2tcpip.h>

int main()
{

	WSADATA wsaData = { 0 };
	int iResult = 0;
	SOCKET sock = INVALID_SOCKET;
	int iFamily = AF_INET;
	int iType = SOCK_STREAM;
	int iProtocol = IPPROTO_TCP;
	
	BOOL bOptVal = FALSE;
	int bOptLen = sizeof(BOOL);
	int iOptVal = 0;
	int iOptLen = sizeof(int);


	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		wprintf(L"WSAStartup failed: %d\n", iResult);
		return 1;
	}

	sock = socket(iFamily, iType, iProtocol);
	if (sock == INVALID_SOCKET)
		wprintf(L"socket function failed with error = %d\n", WSAGetLastError());
	else {

		setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&iOptVal, iOptLen);
		if (iResult == SOCKET_ERROR) {
			wprintf(L"setsockopt for SO_REUSEADDR failed with error: %u\n", WSAGetLastError());
		}

		// The socket address to be passed to bind
		sockaddr_in localBind;

		//----------------------
		// The sockaddr_in structure specifies the address family,
		// IP address, and port for the socket that is being bound.
		localBind.sin_family = AF_INET;
		//localBind.sin_addr.s_addr = inet_addr("127.0.0.1");
		localBind.sin_addr.s_addr = INADDR_ANY;
		//localBind.sin_port = htons(27015);
		localBind.sin_port = htons(50015);


		//----------------------
		// Bind the socket.
		iResult = bind(sock, (SOCKADDR *)&localBind, sizeof(localBind));
		if (iResult == SOCKET_ERROR) {
			wprintf(L"bind failed with error %u\n", WSAGetLastError());
			closesocket(sock);
			WSACleanup();
			return 1;
		}

		//----------------------
		// Connect to server.
		sockaddr_in remoteServer;
		remoteServer.sin_family = AF_INET;
		//remoteServer.sin_addr.S_un.S_addr = inet_addr("216.58.203.68"); // This is www.google.com
		remoteServer.sin_addr.S_un.S_addr = inet_addr("172.217.27.99"); // This is www.google.co.in
		
		remoteServer.sin_port = htons(80);
		iResult = connect(sock, (SOCKADDR *)& remoteServer, sizeof(remoteServer));
		if (iResult == SOCKET_ERROR) {
			wprintf(L"connect function failed with error: %ld\n", WSAGetLastError());
			iResult = closesocket(sock);
			if (iResult == SOCKET_ERROR)
				wprintf(L"closesocket function failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}

		char *sendbuf = "GET /\r\n";
		char recvbuf[512 + 1] = { 0, };

		//----------------------
		// Send an initial buffer
		iResult = send(sock, sendbuf, (int)strlen(sendbuf), 0);
		if (iResult == SOCKET_ERROR) {
			wprintf(L"send failed with error: %d\n", WSAGetLastError());
			closesocket(sock);
			WSACleanup();
			return 1;
		}

		printf("Bytes Sent: %d\n", iResult);


		// Receive until the peer closes the connection
		do {
			memset((void *)recvbuf, 0, sizeof(recvbuf));
			iResult = recv(sock, recvbuf, 512, 0);
			if (iResult > 0) {
				//wprintf(L"Bytes received: %d\n", iResult);
				printf("%s", recvbuf);
			}
			else if (iResult == 0)
				wprintf(L"Connection closed\n");
			else
				wprintf(L"recv failed with error: %d\n", WSAGetLastError());

		} while (iResult > 0);
	}


	// close the socket
	iResult = closesocket(sock);
	if (iResult == SOCKET_ERROR) {
		wprintf(L"close failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	WSACleanup();

	return 0;
}


Main::Main()
{
}


Main::~Main()
{
}
