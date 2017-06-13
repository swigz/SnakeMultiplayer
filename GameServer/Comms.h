#pragma once
#include "..\GameLibrary\GameLib.h";
#include "resource.h"


game g;
int ConnectedClients=0;
HANDLE hPipe = INVALID_HANDLE_VALUE, hThread = NULL, hMutex, canWrite;
LPTSTR pipename = TEXT("\\\\.\\pipe\\pipename");
int gameStatus = SERVER_NO_GAME_RUNNING;
HANDLE hTimeThread[MAX_PLAYERS];



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
DWORD WINAPI TimeThread(DWORD i)
{
	Message answer;
	_tprintf(TEXT("BOSD\n"));
	_tprintf(TEXT("%d\n"), i);

	while (g.running && players[i].inGame) {
		Sleep(1000);
		answer.code = SERVER_UPDATE_MAP;
		sendMsgLogger(players[i].hPipe, answer.code);
		g = gameLogic(g);
		answer.game = g;
		writeClientResponse(players[i].hPipe, answer);
	}
	return 0;
}
BOOL isPlayerInGame(LPVOID param)
{
	for (int i = 0; i < MAX_PLAYERS; i++)
		if (players[i].hPipe == param && players[i].inGame)
			return TRUE;

	return FALSE;
}
void startTicking()
{
	DWORD dwTimeThreadId = 0;
	for (short int i = 0; i < 1 ; i++)
	{
		hTimeThread[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TimeThread, (LPVOID)i, 0, &dwTimeThreadId);
		if (hTimeThread[i] == NULL)
		{
			_tprintf(TEXT("\nError during time's thread creation"));
			system("pause");
			return;
		}
		else
			CloseHandle(hTimeThread[i]);
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
	
	libMaxPlayers = 2;
	g.bots = 0;
	g.open = TRUE;
	g.running = FALSE;
	
	g = CriaMapaNormal(g);
	_tprintf(TEXT("---> %d \n\n"), sizeof(g));
	for (short int i = 0; i < MAX_PLAYERS; i++) {
		if (players[i].hPipe == param) {
			players[i].inGame = TRUE;
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

void processMovement(Message request, LPVOID param) {

	for (short int i = 0; i < MAX_PLAYERS; i++) {
		if (players[i].hPipe == param && players[i].inGame) {
			switch (request.code)
			{
			case R_1_MOVEDOWN:
				players[i].direction = DOWN;
				break;
			case R_1_MOVELEFT:
				players[i].direction = LEFT;
				break;
			case R_1_MOVERIGHT:
				players[i].direction = RIGHT;
				break;
			case R_1_MOVEUP:
				players[i].direction = UP;
				break;
			default:
				break;
			}
		}
	}
	gameLogic(g);
	//draw();

}

void ProcessClientMessage(LPVOID param, Message request, Message answer) {
	_tprintf(TEXT("[SERVER] Received command from: %s, --- Command: %d\n"), request.name, request.code);
	if (param == NULL) {
		_tprintf(TEXT("cliente died \n"));
	}
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
		initGame(answer);
		break;
	case R_1_MOVEDOWN:
	case R_1_MOVELEFT:
	case R_1_MOVERIGHT:
	case R_1_MOVEUP:
	case R_2_MOVEDOWN:
	case R_2_MOVELEFT:
	case R_2_MOVERIGHT:
	case R_2_MOVEUP:
		processMovement(request, param);
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



void initGame(Message answer) {

		// Verifica se o jogador está logado no jogo
			// Ativar a flag
	_tprintf(TEXT("LET THE GAME BEGIN\n"));
		g.running = TRUE;
		//CriaMapaNormal();
		_tprintf(TEXT("LET  BE SNAKES\n"));
		g = SnakesSetup(g);
	_tprintf(TEXT("LET THE TICKING BEGIN\n"));
		// Começar o tempo
		startTicking();

		
		answer.code = SERVER_GAME_STARTED;
		writeServerBroadcast(answer);
		gameStatus = SERVER_GAME_IS_RUNNING;
}