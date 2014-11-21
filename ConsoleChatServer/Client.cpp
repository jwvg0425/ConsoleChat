#include "Client.h"
#include "ClientManager.h"


Client::Client() : m_Buffer(BUF_SIZE)
{
	m_Socket = INVALID_SOCKET;
	m_IsEntered = false;
	m_Name = "anonymous";
}

Client::Client(SOCKET socket) : m_Buffer(BUF_SIZE)
{
	m_Socket = socket;
	m_IsEntered = false;
	m_Name = "anonymous";
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
	char stringLength = 0;
	char sendBuffer[BUF_SIZE];
	m_Buffer.peek(&stringLength, 1);

	while (stringLength <= (int)m_Buffer.getUsingBytes() - 2)
	{
		char string[BUF_SIZE] = { 0, };
		char temp[BUF_SIZE] = { 0, };
		int permission = 0;
		PacketType type = 0;
		m_Buffer.read(&stringLength, 1);
		m_Buffer.read(&type, 1);
		
		//패킷 완성되는 크기만큼 읽어들인다.
		m_Buffer.read(string, stringLength);

		switch (type)
		{
		case PKT_CONNECT:
			m_Name = string;
			
			//접속 가능 여부 확인. 불가능한 경우 denied.
			permission = ClientManager::getInstance()->isPossibleConnect(this);

			sendBuffer[LENGTH_BIT] = 1;
			sendBuffer[TYPE_BIT] = PKT_PERMISSION;
			sendBuffer[TYPE_BIT + 1] = permission;
			send(sendBuffer, 3);

			if (permission == PERMISSION_OK)
			{
				sprintf(sendBuffer + 2, "User [ %s ] entered.\n", m_Name.c_str());
				sendBuffer[LENGTH_BIT] = strlen(sendBuffer + 2);
				sendBuffer[TYPE_BIT] = PKT_CHAT;
				m_IsEntered = true;
				sendNameList();
				ClientManager::getInstance()->broadcast(sendBuffer, strlen(sendBuffer));
			}
			break;
		case PKT_CHANGE_NAME:
			if (strlen(string) >= MAX_NAME_LENGTH)
			{
				sprintf(sendBuffer + 2, "name must be less than %d.\n", MAX_NAME_LENGTH);
				sendBuffer[LENGTH_BIT] = strlen(sendBuffer + 2);
				sendBuffer[TYPE_BIT] = PKT_CHAT;
				send(sendBuffer, strlen(sendBuffer));
			}
			else if (ClientManager::getInstance()->existEqualName(string))
			{
				sprintf(sendBuffer + 2, "already exist equal name.\n");
				sendBuffer[LENGTH_BIT] = strlen(sendBuffer + 2);
				sendBuffer[TYPE_BIT] = PKT_CHAT;
				send(sendBuffer, strlen(sendBuffer));
			}
			else
			{
				sprintf(temp, "%s", m_Name.c_str());
				m_Name = string;
				sprintf(sendBuffer + 2, "User [ %s ] change name to [%s].\n", temp, m_Name.c_str());
				sendBuffer[LENGTH_BIT] = strlen(sendBuffer + 2);
				sendBuffer[TYPE_BIT] = PKT_CHAT;
				sendNameList();
				ClientManager::getInstance()->broadcast(sendBuffer, strlen(sendBuffer));
			}
			break;
		case PKT_CHAT:
			sprintf(sendBuffer + 2, "[ %s ] : %s", m_Name.c_str(), string);
			sendBuffer[LENGTH_BIT] = strlen(sendBuffer + 2);
			sendBuffer[TYPE_BIT] = PKT_CHAT;
			ClientManager::getInstance()->broadcast(sendBuffer, strlen(sendBuffer));
			break;
		case PKT_DISCONNECT:
			ClientManager::getInstance()->removeClient(this);
			return;
		}

		stringLength = 0;
		m_Buffer.peek(&stringLength, 1);
	}
}

void Client::connect()
{
	recv();
}

void Client::disconnect()
{
	if (m_IsEntered)
	{
		char sendBuffer[BUF_SIZE];

		sprintf(sendBuffer + 2, "User [ %s ] disconnected.\n", m_Name.c_str());
		sendBuffer[LENGTH_BIT] = strlen(sendBuffer + 2);
		sendBuffer[TYPE_BIT] = PKT_CHAT;
		ClientManager::getInstance()->broadcast(sendBuffer, strlen(sendBuffer));
		sendNameList();
	}
}

bool Client::isConnected()
{
	return m_IsEntered;
}

std::string Client::getName()
{
	return m_Name;
}

void Client::sendNameList()
{
	char sendBuffer[BUF_SIZE];

	sprintf(sendBuffer + 2, "%s", ClientManager::getInstance()->getNames().c_str());
	sendBuffer[LENGTH_BIT] = strlen(sendBuffer + 2);
	sendBuffer[TYPE_BIT] = PKT_UPDATE_LIST;
	ClientManager::getInstance()->broadcast(sendBuffer, strlen(sendBuffer));
}