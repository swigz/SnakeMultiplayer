#pragma once
#include "..\GameLibrary\GameLib.h";
int ConnectedClients=0;
players pl[MAX_PLAYERS];
HANDLE hPipe = INVALID_HANDLE_VALUE, hThread = NULL, hMutex, canWrite;
LPTSTR pipename = TEXT("\\\\.\\pipe\\pipename");
int gameStatus;

int writeClientResponse(HANDLE hPipe, Message answer)
{

	DWORD cbWritten = 0;
	BOOL fSuccess = FALSE;
	OVERLAPPED OverlWr = { 0 };

	ZeroMemory(&OverlWr, sizeof(OverlWr));
	ResetEvent(canWrite);
	OverlWr.hEvent = canWrite;

	fSuccess = WriteFile(hPipe, &answer, sizeof(Message), &cbWritten, &OverlWr);
	WaitForSingleObject(canWrite, INFINITE);

	GetOverlappedResult(hPipe, &OverlWr, &cbWritten, FALSE);
	if (cbWritten < sizeof(Message))
		_tprintf(TEXT("\nCouldn't write all the information. Error: %d\nCode:%d"), GetLastError(), answer.code);
	return 1;
}

TCHAR* getClientName(LPVOID param) {
	for (short int i = 0; i < MAX_PLAYERS; i++) {
		if (pl[i].hPipe = param) {
			return pl[i].name;
		}
	}
	return TEXT("");
}
void sendMsgLogger(LPVOID param, int code) {
	_tprintf(TEXT("[SERVER] Sending command to: %s, --- Command: %d\n"),getClientName(param), code);
}

BOOL usernameExists(TCHAR* name) {
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (pl[i].hPipe != NULL && _tcscmp(name, pl[i].name) == 0)
			return TRUE;
	}
	return FALSE;
}

void login(LPVOID param, TCHAR* name, Message answer) {

	if (!usernameExists(name)) {
		for (int i = 0; i < MAX_PLAYERS; i++) {
			if (pl[i].hPipe == NULL) {
				_tcscpy(pl[i].name, name);
				pl[i].hPipe = param;
				pl[i].status = LOGGED_IN;
				answer.code = SERVER_LOGIN_SUCCESS;
				sendMsgLogger(param, answer.code);
				writeClientResponse(pl[i].hPipe, answer);
				return;
			}
		}
	}
	else {
		answer.code = SERVER_ERROR_NAME_EXISTS;
		sendMsgLogger(param, answer.code);
		writeClientResponse(param, answer);
		return;
	}
}

void cleanClientHandles(LPVOID param)
{
	FlushFileBuffers(param);
	DisconnectNamedPipe(param);
	CloseHandle(param);
}

int writeServerBroadcast(Message answer)
{
	// Devolve o número de respostas enviadas (clientes que receberam resposta)
	int num = 0;

	for (int i = 0; i < MAX_PLAYERS; i++)
		if (pl[i].hPipe != NULL)
			num += writeClientResponse(pl[i].hPipe, answer);
	return num;
}
void returnGameStatus(LPVOID param, Message answer) {
	answer.code = gameStatus;
	sendMsgLogger(getClientName(param), answer.code);
	writeClientResponse(param, answer);
}

void disconnectClient(LPVOID param, Message answer) {
	//remove da lista
	answer.code = SERVER_DISCONNECT;
	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (pl[i].hPipe == param) {
			sendMsgLogger(pl[i].name, answer.code);
			writeClientResponse(pl[i].hPipe, answer);
			pl[i] = { 0 };
			pl[i].hPipe = NULL;
			pl[i].status = DISCONNECTED;
		}
	}
	cleanClientHandles(param);
	return;
}


//
//
//switch (gameStatus) {
//case SERVER_GAME_ACCEPTING:
//	break;
//case SERVER_NO_GAME_RUNNING:
//	break;
//case SERVER_GAME_IS_RUNNING:
//	break;
//default:
//	break;
//}

void ProcessClientMessage(LPVOID param, Message request, Message answer) {
	_tprintf(TEXT("[SERVER] Received command from: %s, --- Command: %d\n"), request.name, request.code);
	switch (request.code) {
	case R_CONNECT: 
		login(param, request.name, answer);
		break;
	case R_CHECK_GAME_STATUS:
		returnGameStatus(param, answer);
		break;
	case R_LOGOUT:
		disconnectClient(param, answer);
		break;
	defaul:
		break;
	}
}
DWORD WINAPI InstanceThread(LPVOID param)
{
	HANDLE hHeap = GetProcessHeap();
	TCHAR* pchRequest = (TCHAR*)HeapAlloc(hHeap, 0, BUFSIZE * sizeof(TCHAR));
	TCHAR* pchReply = (TCHAR*)HeapAlloc(hHeap, 0, BUFSIZE * sizeof(TCHAR));
	Message request, answer = { 0 };
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
	return 1;
}


