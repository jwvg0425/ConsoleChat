#include "Client.h"


Client::Client() : m_Buffer(BUF_SIZE)
{
	m_Socket = INVALID_SOCKET;
	m_IsConnected = false;
	m_Name = "a​nonymous";
}

Client::Client(SOCKET socket) : m_Buffer(BUF_SIZE)
{
	m_Socket = socket;
	m_IsConnected = false;
	m_Name = "a​nonymous";
}

Client::~Client()
{

}

SOCKET Client::getSocket() const
{
	return m_Socket;
}

void Client::recv()
{
	DWORD flags = 0;
	IOContext* context = new IOContext;
	context->buf = new char[BUF_SIZE];
	context->recv = true;

	WSABUF wsaBuf;
	wsaBuf.buf = context->buf;
	wsaBuf.len = BUF_SIZE;

	WSARecv(m_Socket, &wsaBuf, 1, NULL, &flags, &context->overlapped, nullptr);
}

void Client::send(char* buffer, size_t bytes)
{

}

void Client::update(char* buffer, size_t bytes)
{

}

