
//구조체, 매크로, enum등 전체적으로 이용하는 타입, 상수들 정의

#ifndef __TYPE_DEFINE_H_
#define __TYPE_DEFINE_H_

#include <winsock2.h>

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


//패킷 타입 정의
typedef char PacketType;

#define PKT_CHANGE_NAME 0 //이름 변경 패킷
#define PKT_CHAT		1 //말한 내용
#define	PKT_DISCONNECT	2 //연결 종료 요청
#define PKT_CONNECT		3 //연결 요청


#endif