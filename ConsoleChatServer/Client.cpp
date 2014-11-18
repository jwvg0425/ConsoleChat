#include "Client.h"
#include "ClientManager.h"


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
	disconnect();
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
	IOContext* context = new IOContext();

	WSABUF wsaBuf;
	
	wsaBuf.buf = buffer;
	wsaBuf.len = bytes;

	WSASend(m_Socket, &wsaBuf, 1, NULL, 0, &context->overlapped, nullptr);
}

void Client::update(char* buffer, size_t bytes)
{
	m_Buffer.write(buffer, bytes);

	//packet 길이 읽어 옴.
	char packetLength = 0;
	char sendBuffer[BUF_SIZE];
	m_Buffer.peek(&packetLength, 1);

	while (packetLength <= m_Buffer.getUsingBytes() - 1)
	{
		char string[BUF_SIZE] = { 0, };
		PacketType type;
		m_Buffer.read(&packetLength, 1);
		m_Buffer.read(&type, 1);
		
		//패킷 완성되는 크기만큼 읽어들인다.
		m_Buffer.read(string, packetLength - 2);

		switch (type)
		{
		case PKT_CONNECT:
			m_Name = string;
			m_IsConnected = true;
			break;
		case PKT_CHANGE_NAME:
			m_Name = string;
			break;
		case PKT_CHAT:
			sprintf_s(sendBuffer, "[ %s ] : %s", m_Name.c_str(), string);
			ClientManager::getInstance()->broadcast(sendBuffer, strlen(sendBuffer));
			break;
		case PKT_DISCONNECT:
			ClientManager::getInstance()->removeClient(this);
			return;
		}

		packetLength = 0;
		m_Buffer.peek(&packetLength, 1);
	}
}

void Client::connect()
{
	recv();
}

void Client::disconnect()
{

}
