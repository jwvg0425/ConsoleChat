

//클라이언트 접속, 접속해제, 패킷 전송 등 클라 관련 모든 처리 수행

#ifndef __CLIENT_H_
#define __CLIENT_H_

#include "TypeDefine.h"
#include "Buffer.h"

class Client
{
public:
private:
	SOCKET socket;
	Buffer m_Buffer;
	bool m_IsConnected;
	char name[MAX_NAME_LENGTH];
};

#endif