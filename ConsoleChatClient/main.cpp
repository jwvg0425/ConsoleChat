#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <Windows.h>
#include <process.h>
#include <conio.h>

#pragma comment(lib,"ws2_32.lib")
#define BUF_SIZE 1024

void ErrorHandling(char* message);
unsigned int WINAPI EchoThreadMain(LPVOID param);
void printMessage();
void printInputMessage();
char history[2000][BUF_SIZE];
int message_num = 0;
int input_num = 0;
char inputMessage[BUF_SIZE];
char name[BUF_SIZE];

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
	inputMessage[1] = 3;
	send(hSocket, inputMessage, strlen(inputMessage), 0);
	system("cls");

	_beginthreadex(NULL, 0, EchoThreadMain, (LPVOID)hSocket, 0, NULL);

	while (1)
	{
		char buffer[BUF_SIZE];
		while (1)
		{
			char ch;

			printInputMessage();
			ch = getch();
			if (ch == 8)
			{
				input_num--;
			}
			else
			{
				inputMessage[input_num++] = ch;
			}

			if (ch == '\r')
			{
				fflush(stdin);
				inputMessage[input_num - 1] = '\n';
				inputMessage[input_num] = 0;
				input_num = 0;
				break;
			}
		}

		if (!strcmp(inputMessage, "q\n") || !strcmp(inputMessage, "Q\n"))
		{
			break;
		}

		sprintf(buffer, "%s", inputMessage);
		sprintf(inputMessage + 2, "%s", buffer);
		inputMessage[0] = strlen(buffer);
		inputMessage[1] = 1;
		strLen = strlen(inputMessage);
		send(hSocket, inputMessage, strlen(inputMessage), 0);
	}

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

unsigned int WINAPI EchoThreadMain(LPVOID param)
{
	int readLen = 0;
	SOCKET hSocket = (SOCKET)param;
	char message[BUF_SIZE] = { 0, };

	while (1)
	{
		readLen = 0;
		while (1)
		{
			readLen += recv(hSocket, &message[readLen], BUF_SIZE - 1, 0);

			for (int i = 0; i < readLen; i++)
			{
				if (message[i] == '\n')
				{
					goto PRINT;
				}
			}
		}
	PRINT:
		message[readLen] = 0;

		strcpy(history[message_num], message);
		message_num++;

		printMessage();
	}

	return 0;
}

void printMessage()
{
	system("cls");
	for (int i = ((message_num >= 18) ? message_num - 18 : 0); i < message_num; i++)
	{
		fputs(history[i], stdout);
	}
	printInputMessage();
}

void printInputMessage()
{
	gotoxy(0, 20);
	fputs("Input message(Q to quit): ", stdout);
	for (int i = 0; i < input_num; i++)
	{
		fputc(inputMessage[i], stdout);
	}
}