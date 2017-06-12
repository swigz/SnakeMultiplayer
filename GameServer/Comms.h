#pragma once
#include "..\GameLibrary\GameLib.h";
#include "resource.h"

int ConnectedClients=0;
HANDLE hPipe = INVALID_HANDLE_VALUE, hThread = NULL, hMutex, canWrite;
LPTSTR pipename = TEXT("\\\\.\\pipe\\pipename");
int gameStatus = SERVER_NO_GAME_RUNNING;




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
		if (players[i].hPipe == param) {
			return players[i].name;
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
		if (players[i].hPipe != NULL && _tcscmp(name, players[i].name) == 0)
			return TRUE;
	}
	return FALSE;
}

void login(LPVOID param, TCHAR* name, Message answer) {

	if (!usernameExists(name)) {
		for (int i = 0; i < MAX_PLAYERS; i++) {
			if (players[i].hPipe == NULL) {
				_tcscpy(players[i].name, name);
				players[i].hPipe = param;
				answer.code = SERVER_LOGIN_SUCCESS;
				sendMsgLogger(param, answer.code);
				writeClientResponse(players[i].hPipe, answer);
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
		if (players[i].hPipe != NULL)
			num += writeClientResponse(players[i].hPipe, answer);
	return num;
}
void returnGameStatus(LPVOID param, Message answer) {
	answer.code = gameStatus;
	sendMsgLogger(param, answer.code);
	writeClientResponse(param, answer);
}

void disconnectClient(LPVOID param, Message answer) {
	//remove da lista
	answer.code = SERVER_DISCONNECT;
	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (players[i].hPipe == param) {
			sendMsgLogger(players[i].name, answer.code);
			writeClientResponse(players[i].hPipe, answer);
			players[i].hPipe = NULL;
		}
	}
	cleanClientHandles(param);
	return;
}


void initClient(int i) {
	players[i].score = 0;
	players[i].speed = SNAKE_SPEED;
	//fazer o resto
}

void joinGameLobby(LPVOID param, Message request, Message answer) {
	for (short int i = 0; i < MAX_PLAYERS; i++) {
		if (players[i].hPipe == param) {
			players[i].inGame = TRUE;
		}
	}
	ConnectedClients++;
	answer.code = SERVER_GAME_JOIN_SUCCESS;
	writeClientResponse(param, answer);
	answer.code = BR_PLAYER_JOINED;
	writeServerBroadcast(answer);
}

void createGameLobby(LPVOID param, Message request, Message answer) {
	g.maxPlayers = request.game.maxPlayers;
	g.bots = request.game.bots;
	g.open = TRUE;
	g.running = FALSE;

	for (short int i = 0; i < MAX_PLAYERS; i++) {
		if (players[i].hPipe == param) {
			
			if (request.playerNumber == 2) {
				players[i].isMulti = TRUE;
			}
			//initClient(i);
			_tprintf(players[i].name);
		}
	}
	ConnectedClients++;
	answer.code = SERVER_GAME_CREATE_SUCCESS;
	writeClientResponse(param, answer);
	answer.code = BR_GAME_CREATED;
	int br = writeServerBroadcast(answer);
	_tprintf(TEXT("[SERVER] %d broadcasts sent..."), br);
}



void ProcessClientMessage(LPVOID param, Message request, Message answer) {
	_tprintf(TEXT("[SERVER] Received command from: %s, --- Command: %d\n"), request.name, request.code);
	switch (request.code) {
	case R_CONNECT: 
		login(param, request.name, answer);
		break;
	case R_CHECK_GAME_STATUS:
		returnGameStatus(param, answer);
		break;
	case R_CREATEGAME:
		gameStatus = SERVER_GAME_ACCEPTING;
		createGameLobby(param, request, answer);
		break;
	case R_JOINGAME:
		if (gameStatus == SERVER_GAME_ACCEPTING) {
			joinGameLobby(param, request, answer);
		}
		break;
	case R_LOGOUT:
		disconnectClient(param, answer);
		break;
	case R_STARTGAME:

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
				cleanClientHandles(param);
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


