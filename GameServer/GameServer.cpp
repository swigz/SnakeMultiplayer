// GameServer.cpp : Defines the entry point for the console application.
//

#include "Comms.h"


int _tmain(int argc, TCHAR *argv[])
{
	BOOL fConnected = false, isAccepting = true;
	DWORD dwThreadId = 0;

	hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, TEXT("Single Instance Mutex"));
	if (hMutex == NULL)
		hMutex = CreateMutex(NULL, FALSE, TEXT("Single Instance Mutex"));
	else
	{
		_tprintf(TEXT("There is already a server running!\n\n"));
		system("pause");
		return 1;
	}

	canWrite = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (canWrite == NULL) {
		_tprintf(TEXT("Erro ao criar evento de writeready\n"));
		return 1;
	}
	while (isAccepting) {
		hPipe = CreateNamedPipe(pipename, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, BUFSIZE, BUFSIZE, 5000, NULL);
		if (hPipe == INVALID_HANDLE_VALUE)
		{
			_tprintf(TEXT("\nNamed pipe creation failed! Error: %d"), GetLastError());
			return 1;
		}
		//verifica se alguem connectou
		fConnected = ConnectNamedPipe(hPipe, NULL) ?
			TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
		if (fConnected)
		{
			printf("----> Client connected, creating a processing thread.\n");

			// Create a thread for this client. 
			hThread = CreateThread(
				NULL,              // no security attribute 
				0,                 // default stack size 
				(LPTHREAD_START_ROUTINE)InstanceThread,    // thread proc
				(LPVOID)hPipe,    // thread parameter 
				0,                 // not suspended 
				&dwThreadId);      // returns thread ID 

			if (hThread == NULL)
			{
				_tprintf(TEXT("CreateThread failed, GLE=%d.\n"), GetLastError());
				return 1;
			}
			else CloseHandle(hThread);
		}
		else
			// The client could not connect, so close the pipe. 
			CloseHandle(hPipe);
	}
	return 1;
	
}

