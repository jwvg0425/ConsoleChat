﻿#include "ClientManager.h"


ClientManager* ClientManager::m_Instance = nullptr;


ClientManager* ClientManager::getInstance()
{
	if (m_Instance == nullptr)
	{
		m_Instance = new ClientManager();
	}

	return m_Instance;
}

void ClientManager::releaseInstance()
{
	if (m_Instance != nullptr)
	{
		delete m_Instance;
		m_Instance = nullptr;
	}
}

bool ClientManager::addClient(Client* client)
{
	if (client == nullptr)
		return false;

	//이미 해당 소켓에 클라 존재하는지 확인
	if (m_Clients.find(client->getSocket()) == m_Clients.end())
	{
		m_Clients[client->getSocket()] = client;
		return true;
	}

	return false;
}

bool ClientManager::removeClient(Client* client)
{
	if (client == nullptr)
		return false;

	//해당 소켓에 클라 존재하는지 확인
	if (m_Clients.find(client->getSocket()) != m_Clients.end())
	{
		//해당 소켓에 존재하는 클라가 인자로 넘어온 클라랑 동일 -> 올바른 삭제.
		if (m_Clients[client->getSocket()] == client)
		{
			m_Clients.erase(client->getSocket());
			delete client;

			return true;
		}
	}

	return false;
}

Client* ClientManager::getClient(SOCKET socket)
{
	return m_Clients[socket];
}

ClientManager::ClientManager()
{
	m_Clients.clear();
}

ClientManager::~ClientManager()
{
	for (auto& client : m_Clients)
	{
		removeClient(client.second);
	}

	m_Clients.clear();
}

void ClientManager::broadcast(char* buf, size_t bytes)
{
	if (buf[TYPE_BIT] != PKT_UPDATE_LIST)
	{
		printf("%s", buf + 2);
	}

	for (auto& client : m_Clients)
	{
		if (client.second->isConnected())
		{
			client.second->send(buf, bytes);
		}
	}
}

std::string ClientManager::getNames()
{
	std::string names = "";
	for (auto& client : m_Clients)
	{
		if (client.second->isConnected())
		{
			names += client.second->getName() + '\n';
		}
	}

	return names;
}

int ClientManager::getClientNum()
{
	int num = 0;

	for (auto& client : m_Clients)
	{
		if (client.second->isConnected())
		{
			num++;
		}
	}

	return num;
}

int ClientManager::isPossibleConnect(Client* client)
{
	if (getClientNum() >= MAX_CLIENT_NUM)
	{
		return PERMISSION_SERVER_FULL;
	}

	if (strlen(client->getName().c_str()) >= MAX_NAME_LENGTH)
	{
		return PERMISSION_TOO_LONG_NAME;
	}

	if (existEqualName(client->getName()))
	{
		return PERMISSION_EQUAL_NAME;
	}

	return PERMISSION_OK;
}

bool ClientManager::existEqualName(std::string name)
{
	for (auto& client : m_Clients)
	{
		if (client.second->isConnected())
		{
			if (name == client.second->getName())
			{
				return true;
			}
		}
	}

	return false;
}
