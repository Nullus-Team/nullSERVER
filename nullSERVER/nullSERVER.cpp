#include "pch.h"
#include "framework.h"
#include "NullServer.h"
#include "MessagesProcessing.h"
#include "Routing.h"
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
	cout << "nullSERVER/0.0.1\n\nWritten by @cngthnh and @tiendat101001\nFIT@HCMUS - 19CTT2 - Computer Network\n\n";
	CSocket NServer, client;
	NServer.Create(PORT);
	cout << "Server started on port " << PORT << "\n";
	DWORD threadID;
	HANDLE threadStatus;
	while (1)
	{
		//cout << "Loop in!\n";
		NServer.Listen(MAX_CLIENTS);
		NServer.Accept(client);
		//cout << "Accepted!\n";
		SOCKET* connectedClient = new SOCKET();
		*connectedClient = client.Detach();
		threadStatus = CreateThread(NULL, 0, accessProcessing, connectedClient, 0, &threadID);
		//cout << "Thread Created!\n";
	}

}
