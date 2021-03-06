﻿

#ifndef __CLIENT_MANAGER_H_
#define __CLIENT_MANAGER_H_

#include "TypeDefine.h"
#include "Client.h"
#include <map>

class ClientManager
{
public:
	static ClientManager*		getInstance();
	static void					releaseInstance();
	bool						addClient(Client* client);
	bool						removeClient(Client* client);
	void						broadcast(char* buf, size_t bytes);
	Client*						getClient(SOCKET socket);
	std::string					getNames();
	int							getClientNum();
	int							isPossibleConnect(Client* client);
	bool						existEqualName(std::string name);

private:
	static ClientManager*		m_Instance;

	std::map<SOCKET, Client*>	m_Clients;
	ClientManager();
	~ClientManager();
};

#endif