#pragma once
#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include <strsafe.h>
#define BUFSIZE 512
int ConnectedClients=0;




int writeClientResponse(HANDLE hPipe, Message answer)
{
	DWORD cbWritten = 0;
	BOOL fSuccess = FALSE;
	OVERLAPPED OverlWr = { 0 };

	ZeroMemory(&OverlWr, sizeof(OverlWr));
	//ResetEvent(WriteReady);
	//OverlWr.hEvent = WriteReady;

	fSuccess = WriteFile(hPipe, &answer, sizeof(Message), &cbWritten, &OverlWr);
	//WaitForSingleObject(WriteReady, INFINITE);

	//GetOverlappedResult(hPipe, &OverlWr, &cbWritten, FALSE);
	if (cbWritten < sizeof(Message))
		_tprintf(TEXT("\nCouldn't write all the information. Error: %d\nCode:%d"), GetLastError(), answer.code);
	return 1;
}

void login(LPVOID param, TCHAR* name, Message answer) {
	for (int i = 0; i < ConnectedClients; i++) {
		if (players[i].hPipe != NULL && _tcscmp(name, players[i].name) == 0) {
			//_stprintf(answer.info, TEXT("%sJá existe um jogador com esse nome, escolha outro\n"), answer.info);
			answer.code = ERROR_NAME_EXISTS;
			writeClientResponse(players[i].hPipe, answer);
			return;
		}
		else {
			ConnectedClients++;
			if (players[ConnectedClients].hPipe == NULL) {
				players[ConnectedClients].hPipe = param;
				players[ConnectedClients].logged = TRUE;
				players[ConnectedClients].inGame = FALSE;
			}
		}
	}
}

void ProcessClientMessage(LPVOID param, Message request, Message answer) {
	switch (request.code) {
	case R_CONNECT: {
		login(param, request.name, answer);
		break;
	}
	}

}
DWORD WINAPI InstanceThread(LPVOID param)

{
	HANDLE hHeap = GetProcessHeap();
	TCHAR* pchRequest = (TCHAR*)HeapAlloc(hHeap, 0, BUFSIZE * sizeof(TCHAR));
	TCHAR* pchReply = (TCHAR*)HeapAlloc(hHeap, 0, BUFSIZE * sizeof(TCHAR));
	Message request, answer;
	DWORD cbBytesRead = 0, cbReplyBytes = 0, cbWritten = 0;
	BOOL fSuccess = FALSE;
	HANDLE hPipe = NULL;
	if (param == NULL)
	{

		if (pchReply != NULL) HeapFree(hHeap, 0, pchReply);
		if (pchRequest != NULL) HeapFree(hHeap, 0, pchRequest);
		return (DWORD)-1;
	}

	if (pchRequest == NULL)
	{
		printf("\nERROR - Pipe Server Failure:\n");
		printf("   InstanceThread got an unexpected NULL heap allocation.\n");
		printf("   InstanceThread exitting.\n");
		if (pchReply != NULL) HeapFree(hHeap, 0, pchReply);
		return (DWORD)-1;
	}

	if (pchReply == NULL)
	{
		printf("\nERROR - Pipe Server Failure:\n");
		printf("   InstanceThread got an unexpected NULL heap allocation.\n");
		printf("   InstanceThread exitting.\n");
		if (pchRequest != NULL) HeapFree(hHeap, 0, pchRequest);
		return (DWORD)-1;
	}
	hPipe = (HANDLE)param;

	while (1)
	{
		fSuccess = ReadFile(hPipe, &request, sizeof(Message), &cbBytesRead, NULL);

		if (!fSuccess || cbBytesRead == 0)
		{
			if (GetLastError() == ERROR_BROKEN_PIPE)
			{
				_tprintf(TEXT("InstanceThread: client disconnected.\n"), GetLastError());
			}
			else
			{
				_tprintf(TEXT("InstanceThread ReadFile failed, GLE=%d.\n"), GetLastError());
			}
			break;
		}

		ProcessClientMessage(param, request, answer);
	}
	FlushFileBuffers(hPipe);
	DisconnectNamedPipe(hPipe);
	CloseHandle(hPipe);

	HeapFree(hHeap, 0, pchRequest);
	HeapFree(hHeap, 0, pchReply);

	printf("InstanceThread exitting.\n");
	return 1;
}


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