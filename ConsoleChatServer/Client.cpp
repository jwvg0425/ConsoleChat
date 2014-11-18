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

