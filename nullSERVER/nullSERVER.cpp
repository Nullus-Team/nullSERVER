#include "pch.h"
#include "framework.h"
#include "NullServer.h"
#include "MessagesProcessing.h"
void _tmain()
{
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		return;
	}
	if (!AfxSocketInit())
	{
		_tprintf(_T("Fatal Error: Can't init socket\n"));
		return;
	}
	CSocket NServer, client;
	NServer.Create(PORT);
	DWORD threadID;
	HANDLE threadStatus;
	while (1)
	{
		cout << "Loop in!\n";
		NServer.Listen(MAX_CLIENTS);
		NServer.Accept(client);
		cout << "Accepted!\n";
		SOCKET* connectedClient = new SOCKET();
		*connectedClient = client.Detach();
		threadStatus = CreateThread(NULL, 0, accessProcessing, connectedClient, 0, &threadID);
		cout << "Thread Created!\n";
	}

}
