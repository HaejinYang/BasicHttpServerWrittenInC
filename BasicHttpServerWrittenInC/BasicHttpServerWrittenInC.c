#include <WS2tcpip.h>
#include <Windows.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

#define LISTEN_PORT	(50001)
#define LISTEN_IP	L"0.0.0.0"

int main()
{
	WSADATA wsa;
	int wsa_startup_result = WSAStartup(MAKEWORD(2, 2), &wsa);
	if (wsa_startup_result != 0)
	{
		int error = WSAGetLastError();
		wprintf(L"Failed WSAStartup. code : %d", error);

		return 1;
	}

	SOCKADDR_IN server_address;
	memset(&server_address, 0, sizeof server_address);
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(LISTEN_PORT);
	int inet_pton_result = InetPton(AF_INET, LISTEN_IP, &server_address.sin_addr);
	if (inet_pton_result != 1)
	{
		if (inet_pton_result == 0)
		{
			wprintf(L"Failed InetPton. invalid IP string");
		}
		else
		{
			int error = WSAGetLastError();
			wprintf(L"Failed InetPton. code : %d", error);
		}

		return 1;
	}

	SOCKET listen_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_socket == INVALID_SOCKET)
	{
		int error = WSAGetLastError();
		wprintf(L"Failed socket. code : %d", error);

		return 1;
	}

	int bind_result = bind(listen_socket, (struct sockaddr*)&server_address, sizeof server_address);
	if (bind_result != 0)
	{
		int error = WSAGetLastError();
		wprintf(L"Failed bind. code : %d", error);

		return 1;
	}

	int listen_result = listen(listen_socket, SOMAXCONN);
	if (listen_result != 0)
	{
		int error = WSAGetLastError();
		wprintf(L"Failed listen. code : %d", error);

		return 1;
	}

	while (1)
	{
		SOCKADDR_IN client_address;
		int client_address_size = sizeof client_address;
		SOCKET connected_socket = accept(listen_socket, (struct sockaddr*)&client_address, &client_address_size);
		if (connected_socket == INVALID_SOCKET)
		{
			int error = WSAGetLastError();
			wprintf(L"Failed accept. code : %d", error);

			continue;
		}

		char* body = "<html><body><p>Hello world</p></body></html>";
		char* header = "HTTP/1.1 200 OK\nContent-Length: 45\nContent-Type: text/html\n\n";

		send(connected_socket, header, strlen(header), 0);
		send(connected_socket, body, strlen(body), 0);
		
		Sleep(10);
		closesocket(connected_socket);
	}

	return 0;
}