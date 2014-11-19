#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <winsock2.h>
#include <Windows.h>
#include <map>
#include "ClientManager.h"


#pragma comment(lib,"ws2_32.lib")

#define BUF_SIZE 1024

void ErrorHandling(char* message);

unsigned int WINAPI IOCPThreadMain(LPVOID completionPortIO);
CRITICAL_SECTION globalCriticalSection;

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	HANDLE hComPort;

	SOCKET hServSock;
	SOCKADDR_IN servAdr;
	DWORD flags = 0;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		ErrorHandling("WSAStartup() error!");
		return -1;
	}

	hComPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	_beginthreadex(NULL, 0, IOCPThreadMain, (LPVOID)hComPort, 0, NULL);

	hServSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	
	if (hServSock == INVALID_SOCKET)
	{
		ErrorHandling("WSASocket() error!");
		return -1;
	}
	
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
#ifdef _DEBUG
	servAdr.sin_port = htons(atoi("41026"));
#else
	if (argc != 2)
	{
		printf("USAGE : %s <PORT>", argv[0]);
		return;
	}

	servAdr.sin_port = htons(atoi(argv[1]));
#endif

	char option = 1;
	if (setsockopt(hServSock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)))
	{
		ErrorHandling("setsockopt() error!");
		return -1;
	}

	if (bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr)))
	{
		ErrorHandling("bind() error!");
		return -1;
	}

	if (listen(hServSock, 5))
	{
		ErrorHandling("bind() error!");
		return -1;
	}

	while (true)
	{
		SOCKET hClntSock;
		SOCKADDR_IN clntAdr;
		int addrLen = sizeof(clntAdr);

		hClntSock = accept(hServSock, (SOCKADDR*)&clntAdr, &addrLen);

		if (hClntSock == INVALID_SOCKET)
		{
			continue;
		}

		Client* client = new Client(hClntSock);

		if (CreateIoCompletionPort((HANDLE)hClntSock, hComPort, (ULONG_PTR)client, 0) != hComPort)
		{
			ErrorHandling("CreateIoCompletionPort() error!");
			continue;
		}

		ClientManager::getInstance()->addClient(client);

		client->connect();
	}

	ClientManager::releaseInstance();
	CloseHandle(hComPort);
	closesocket(hServSock);
	WSACleanup();

	return 0;

}

unsigned WINAPI IOCPThreadMain(LPVOID pComPort)
{
	HANDLE hComPort = (HANDLE)pComPort;
	DWORD bytesTrans;
	Client* client;
	IOContext* context;
	char buffer[BUF_SIZE] = { 0, };

	while (true)
	{
		if (!GetQueuedCompletionStatus(hComPort, &bytesTrans,
			(PULONG_PTR)&client, (LPOVERLAPPED*)&context, INFINITE))
		{
			int error = GetLastError();

			if (error != ERROR_NETNAME_DELETED)
			{
				ErrorHandling("GetQueuedCompletionStatus() error!");
				break;
			}
		}

		if (context->recv)
		{
			if (bytesTrans == 0)
			{
				ClientManager::getInstance()->removeClient(client);
			}
			else
			{
				client->update(context->buf, bytesTrans);

				client->recv();
			}
		}

		delete context;
	}
	return 0;
}

void ErrorHandling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}