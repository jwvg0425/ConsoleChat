#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <winsock2.h>
#include <Windows.h>
#include <map>


#pragma comment(lib,"ws2_32.lib")

#define BUF_SIZE 1024

void ErrorHandling(char* message);

unsigned int WINAPI IOCPThreadMain(LPVOID completionPortIO);

int main(int argc, char* argv[])
{
	return 0;
}

unsigned WINAPI IOCPThreadMain(LPVOID pComPort)
{
	return 0;
}

void ErrorHandling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}