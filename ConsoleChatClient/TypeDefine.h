
//구조체, 매크로, enum등 전체적으로 이용하는 타입, 상수들 정의

#ifndef __TYPE_DEFINE_H_
#define __TYPE_DEFINE_H_

#include <winsock2.h>
#include "../PacketType.h"

//출력용 인수 표시
#define OUT
#define BUF_SIZE 1024
#define MAX_NAME_LENGTH 16

struct IOContext
{
	IOContext()
		:buf(nullptr), recv(false)
	{
		ZeroMemory(&overlapped, sizeof(WSAOVERLAPPED));
	}

	~IOContext()
	{
		if (buf)
			delete[] buf;
	}

	WSAOVERLAPPED overlapped;
	char* buf;
	bool recv;
};


#endif