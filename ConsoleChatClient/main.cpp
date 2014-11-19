#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <Windows.h>
#include <process.h>
#include <conio.h>
#include "../PacketType.h"
#include "Buffer.h"
#include <deque>

#pragma comment(lib,"ws2_32.lib")
#define BUF_SIZE 1024
#define MAX_INPUT_NUM 40
#define MAX_NAME_LENGTH 10
#define LOG_WIDTH 60
#define LOG_HEIGHT 20
#define HELP_NUM 7

void ErrorHandling(char* message);
unsigned int WINAPI PrintThreadMain(LPVOID param);
void printMessage();
void printInputMessage();
void parsingMessage(SOCKET hSocket);
void printTemplate();

std::deque<std::string> logs;
int inputNum = 0;
char inputMessage[BUF_SIZE];
char name[BUF_SIZE];
CRITICAL_SECTION globalCriticalSection;

char* helpList[HELP_NUM] =
{
	"*********************************************",
	"/name <name> : change your name",
	"/help : show how to use this program",
	"/quit : exit this program",
	"have a good chatting!",
	"made by Nam HyeonWook in NHN NEXT, 2014.",
	"*********************************************",
};

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
		return -1;
	}

	hSocket = socket(PF_INET, SOCK_STREAM, 0);

	if (hSocket == INVALID_SOCKET)
	{
		ErrorHandling("socket() error!");
		return -1;
	}

	memset(&servAddr, 0, sizeof(servAddr));

	servAddr.sin_family = AF_INET;

#ifdef _DEBUG
	servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servAddr.sin_port = htons(atoi("41026"));
#else
	if (argc != 3)
	{
		printf("USAGE : %s <IP> <PORT>", argv[0]);
		return -1;
	}
	servAddr.sin_addr.s_addr = inet_addr(argv[1]);
	servAddr.sin_port = htons(atoi(argv[2]));

#endif

	if (connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
	{
		ErrorHandling("connect() error!");
		return -1;
	}

	printf("input your name:");
	scanf("%s", name);

	while (strlen(name) > MAX_NAME_LENGTH + 1)
	{
		printf("Name length must be less than %d.\n", MAX_NAME_LENGTH);
		printf("input your name:");
		scanf("%s", name);
	}
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
			if (ch == 8 && inputNum>0)
			{
				inputNum--;
				inputMessage[inputNum] = '\0';
			}
			else if (inputNum < MAX_INPUT_NUM)
			{
				inputMessage[inputNum++] = ch;
				inputMessage[inputNum] = '\0';
			}

			if (ch == '\r')
			{
				inputMessage[inputNum - 1] = '\n';
				inputMessage[inputNum] = 0;
				inputNum = 0;
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
				logs.push_back(string);

				EnterCriticalSection(&globalCriticalSection);
				printMessage();
				LeaveCriticalSection(&globalCriticalSection);
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

	// 최대 저장 로그 개수 넘어가면 오래된것부터 제거.
	while (logs.size() > LOG_HEIGHT - 2)
	{
		logs.pop_front();
	}

	for (int i = 0; i < logs.size(); i++)
	{
		gotoxy(3, 2 + i);
		printf("%58c", ' ');
		gotoxy(3, 2 + i);
		printf("%s",logs[i].c_str());
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

		//이름의 길이가 긴 경우 거부.
		if (strlen(buffer) > MAX_NAME_LENGTH + 1)
		{
			sprintf(buffer, "Name length must be less than %d.", MAX_NAME_LENGTH);
			logs.push_back(buffer);

			EnterCriticalSection(&globalCriticalSection);
			printMessage();
			LeaveCriticalSection(&globalCriticalSection);
			return;
		}
		buffer[strlen(buffer) - 1] = '\0';
		inputMessage[0] = strlen(buffer);
		sprintf(inputMessage + 2, "%s", buffer);
	}
	else if (!strncmp(inputMessage, "/help", 5))
	{
		for (int i = 0; i < HELP_NUM; i++)
		{
			logs.push_back(helpList[i]);
		}
		EnterCriticalSection(&globalCriticalSection);
		printMessage();
		LeaveCriticalSection(&globalCriticalSection);
		return;
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
	for (int i = 0; i < LOG_WIDTH / 2; i++)
	{
		printf("─");
	}
	printf("┐");
	for (int i = 0; i < LOG_HEIGHT; i++)
	{
		gotoxy(0, 2 + i);
		printf("│");
		gotoxy(LOG_WIDTH + 2, 2 + i);
		printf("│");
	}
	gotoxy(0, LOG_HEIGHT + 1);
	printf("└");
	for (int i = 0; i < LOG_WIDTH / 2; i++)
	{
		printf("─");
	}
	printf("┘");

	gotoxy(0, LOG_HEIGHT + 2);
	printf("┌");
	for (int i = 0; i < LOG_WIDTH / 2; i++)
	{
		printf("─");
	}
	printf("┐");
	gotoxy(0, LOG_HEIGHT + 3);
	printf("│");
	gotoxy(LOG_WIDTH + 2, LOG_HEIGHT + 3);
	printf("│");
	gotoxy(0, LOG_HEIGHT + 4);
	printf("└");
	for (int i = 0; i < LOG_WIDTH / 2; i++)
	{
		printf("─");
	}
	printf("┘");
}