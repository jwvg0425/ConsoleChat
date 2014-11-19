#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <Windows.h>
#include <process.h>
#include <conio.h>
#include "../PacketType.h"
#include "Buffer.h"

#pragma comment(lib,"ws2_32.lib")
#define BUF_SIZE 1024

void ErrorHandling(char* message);
unsigned int WINAPI PrintThreadMain(LPVOID param);
void printMessage();
void printInputMessage();
void parsingMessage(SOCKET hSocket);
void printTemplate();

char history[2000][BUF_SIZE];
int message_num = 0;
int input_num = 0;
char inputMessage[BUF_SIZE];
char name[BUF_SIZE];
CRITICAL_SECTION globalCriticalSection;

void gotoxy(int x, int y)
{
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	SOCKET hSocket;
	SOCKADDR_IN servAddr;

	int strLen = 0, readLen = 0;

	InitializeCriticalSection(&globalCriticalSection);

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		ErrorHandling("WSAStartup() error!");
	}

	hSocket = socket(PF_INET, SOCK_STREAM, 0);

	if (hSocket == INVALID_SOCKET)
	{
		ErrorHandling("socket() error!");
	}

	memset(&servAddr, 0, sizeof(servAddr));

	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servAddr.sin_port = htons(atoi("41026"));

	if (connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
	{
		ErrorHandling("connect() error!");
	}

	printf("input your name:");
	scanf("%s", name);
	sprintf(inputMessage + 2, "%s", name);
	inputMessage[0] = strlen(name);
	inputMessage[1] = PKT_CONNECT;
	send(hSocket, inputMessage, strlen(inputMessage), 0);
	system("cls");

	_beginthreadex(NULL, 0, PrintThreadMain, (LPVOID)hSocket, 0, NULL);

	while (1)
	{
		inputMessage[0] = '\0';
		while (1)
		{
			char ch;
			EnterCriticalSection(&globalCriticalSection);
			printInputMessage();
			LeaveCriticalSection(&globalCriticalSection);

			ch = getch();
			if (ch == 8 && input_num>0)
			{
				input_num--;
				inputMessage[input_num] = '\0';
			}
			else
			{
				inputMessage[input_num++] = ch;
				inputMessage[input_num] = '\0';
			}

			if (ch == '\r')
			{
				inputMessage[input_num - 1] = '\n';
				inputMessage[input_num] = 0;
				input_num = 0;
				break;
			}
		}

		parsingMessage(hSocket);
	}

	DeleteCriticalSection(&globalCriticalSection);

	closesocket(hSocket);
	WSACleanup();
	return 0;
}

void ErrorHandling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

unsigned int WINAPI PrintThreadMain(LPVOID param)
{
	SOCKET hSocket = (SOCKET)param;
	Buffer buffer(BUF_SIZE);
	char message[BUF_SIZE] = { 0, };
	int readLen = 0;

	while (true)
	{
		readLen = recv(hSocket, message, BUF_SIZE - 1, 0);
		if (readLen <= 0)
			continue;

		buffer.write(message, readLen);

		char stringLength = 0;
		char string[BUF_SIZE] = { 0, };
		PacketType type;

		buffer.peek(&stringLength, 1);

		while (stringLength <= (int)buffer.getUsingBytes() - 2)
		{
			buffer.read(&stringLength, 1);
			buffer.read(&type, 1);

			//패킷 완성되는 크기만큼 읽어들인다.
			buffer.read(string, stringLength);

			switch (type)
			{
			case PKT_CHAT:
				strcpy(history[message_num], string);
				message_num++;

				EnterCriticalSection(&globalCriticalSection);
				printMessage();
				LeaveCriticalSection(&globalCriticalSection);
				break;
			case PKT_LIST:
				break;
			case PKT_JOIN:
				break;
			case PKT_OUT:
				break;
			}

			stringLength = 0;
			buffer.peek(&stringLength, 1);
		}
	}

	return 0;
}

void printMessage()
{
	printTemplate();

	int startNum = ((message_num >= 18) ? message_num - 18 : 0);

	for (int i = startNum; i < message_num; i++)
	{
		gotoxy(3, 2 + i - startNum);
		printf("%58c", ' ');
		gotoxy(3, 2 + i - startNum);
		printf("%s",history[i]);
	}
	printInputMessage();
}

void printInputMessage()
{
	gotoxy(3, 23);
	printf("%58c", ' ');
	gotoxy(3, 23);
	printf("%s  ", inputMessage);
}

void parsingMessage(SOCKET hSocket)
{
	char buffer[BUF_SIZE];
	int strLen;

	//종료
	if (!strncmp(inputMessage, "/quit", 5))
	{
		exit(0);
	}
	else if (!strncmp(inputMessage, "/name ", 6))
	{
		inputMessage[1] = PKT_CHANGE_NAME;
		sprintf(buffer, "%s", inputMessage + 6);
		buffer[strlen(buffer) - 1] = '\0';
		inputMessage[0] = strlen(buffer);
		sprintf(inputMessage + 2, "%s", buffer);
	}
	else if (!strncmp(inputMessage, "/help", 5))
	{
	}
	else
	{
		sprintf(buffer, "%s", inputMessage);
		sprintf(inputMessage + 2, "%s", buffer);
		inputMessage[0] = strlen(buffer);
		inputMessage[1] = PKT_CHAT;
	}

	strLen = strlen(inputMessage + 2) + 2;
	send(hSocket, inputMessage, strlen(inputMessage), 0);
}

void printTemplate()
{
	gotoxy(0, 1);
	printf("┌");
	for (int i = 0; i < 30; i++)
	{
		printf("─");
	}
	printf("┐");
	for (int i = 0; i < 20; i++)
	{
		gotoxy(0, 2 + i);
		printf("│");
		gotoxy(62, 2 + i);
		printf("│");
	}
	gotoxy(0, 21);
	printf("└");
	for (int i = 0; i < 30; i++)
	{
		printf("─");
	}
	printf("┘");

	gotoxy(0, 22);
	printf("┌");
	for (int i = 0; i < 30; i++)
	{
		printf("─");
	}
	printf("┐");
	gotoxy(0, 23);
	printf("│");
	gotoxy(62, 23);
	printf("│");
	gotoxy(0, 24);
	printf("└");
	for (int i = 0; i < 30; i++)
	{
		printf("─");
	}
	printf("┘");
}