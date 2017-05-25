#pragma once
#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include <strsafe.h>
#define BUFSIZE 512

void listenForClients() {
	BOOL fConnected = false, isAccepting = true;
	DWORD dwThreadId = 0;
	HANDLE hPipe = INVALID_HANDLE_VALUE, hThread = NULL;
	LPTSTR pipename = TEXT("\\\\.\\pipe\\pipename");



	while (isAccepting) {
			_tprintf(TEXT("\n[SERVER] Pipe - %s"), pipename);
			hPipe = CreateNamedPipe(
				pipename,             // pipe name 
				PIPE_ACCESS_DUPLEX,       // read/write access 
				PIPE_TYPE_MESSAGE |       // message type pipe 
				PIPE_READMODE_MESSAGE |   // message-read mode 
				PIPE_WAIT,                // blocking mode 
				PIPE_UNLIMITED_INSTANCES, // max. instances  
				BUFSIZE,                  // output buffer size 
				BUFSIZE,                  // input buffer size 
				0,                        // client time-out 
				NULL);                    // default security attribute 
			if (hPipe == INVALID_HANDLE_VALUE)
			{
				_tprintf(TEXT("[SERVER] Criacao do pipe falhada, GLE=%d.\n"), GetLastError());
				return;
			}
			//verifica se alguem connectou
			fConnected = ConnectNamedPipe(hPipe, NULL) ?
				TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
			if (fConnected)
			{
				printf("Client connected, creating a processing thread.\n");

				// Create a thread for this client. 
				hThread = CreateThread(
					NULL,              // no security attribute 
					0,                 // default stack size 
					InstanceThread,    // thread proc
					(LPVOID)hPipe,    // thread parameter 
					0,                 // not suspended 
					&dwThreadId);      // returns thread ID 

				if (hThread == NULL)
				{
					_tprintf(TEXT("CreateThread failed, GLE=%d.\n"), GetLastError());
					return;
				}
				else CloseHandle(hThread);
			}
			else
				// The client could not connect, so close the pipe. 
				CloseHandle(hPipe);
		}
	return;

}


	
