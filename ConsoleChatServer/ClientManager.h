

#ifndef __CLIENT_MANAGER_H_
#define __CLIENT_MANAGER_H_

#include "TypeDefine.h"
#include "Client.h"
#include <map>

class ClientManager
{
public:
	static ClientManager*		getInstance();
	void						releaseInstance();
	bool						addClient(Client* client);
	bool						removeClient(Client* client);
	Client*						getClient(SOCKET socket);

private:
	static ClientManager*		m_Instance;

	std::map<SOCKET, Client*>	m_Clients;
	ClientManager();
	~ClientManager();
};

#endif