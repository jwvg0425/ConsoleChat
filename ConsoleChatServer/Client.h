﻿

//클라이언트 접속, 접속해제, 패킷 전송 등 클라 관련 모든 처리 수행

#ifndef __CLIENT_H_
#define __CLIENT_H_

#include "TypeDefine.h"
#include "Buffer.h"
#include <string>

class Client
{
public:
	Client();
	Client(SOCKET socket);
	~Client();

	SOCKET		getSocket() const;
	void		connect();
	void		disconnect();
	void		recv();
	void		send(char* buffer, size_t bytes);
	void		update(char* buffer, size_t bytes);
	bool		isConnected();
	void		sendNameList();
	std::string getName();

private:
	SOCKET		m_Socket;
	Buffer		m_Buffer;
	bool		m_IsEntered;
	std::string m_Name;
};

#endif