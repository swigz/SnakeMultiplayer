#include "Resource.h"
#include "resource1.h"
HANDLE hPipe, canWrite;
BOOL fSuccess = FALSE;
DWORD  cbRead, cbToWrite, cbWritten, dwMode;
LPTSTR pipename = TEXT("\\\\.\\pipe\\pipename");
TCHAR username[256];
int clientStatus;
int gameStatus;

void closeClient(HANDLE hPipe) {
	CloseHandle(hPipe);
}


void ProcessServerMessage(HWND hWnd, Message answer) {
	int code = answer.code;
	switch (code) {
	case SERVER_LOGIN_SUCCESS:
		clientStatus = LOGGED_IN;
		break;
	case SERVER_ERROR_NAME_EXISTS:
		MessageBox(hWnd, TEXT("There is a user with that name"), TEXT("Error"), MB_ICONEXCLAMATION);
		break;
	case SERVER_GAME_ACCEPTING:
	case SERVER_GAME_IS_RUNNING:
	case SERVER_NO_GAME_RUNNING:
		gameStatus = code;
		break;
	case SERVER_DISCONNECT:
		//exitApp();
		break;
	default:
		break;
	}
	InvalidateRect(hWnd, NULL, 1);
}

DWORD WINAPI ClientThread(LPVOID param) {
	DWORD cbBytesRead = 0;
	BOOL fSuccess = FALSE;
	HANDLE canRead;
	OVERLAPPED OverlRd = { 0 };
	HWND hWnd = (HWND)param;
	TCHAR str[NAMESIZE];
	Message answer;


	canRead = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (canRead == NULL)
	{
		_stprintf(str, TEXT("Error while creating the event. Error: %d"), GetLastError());
		MessageBox(hWnd, str, TEXT("Error"), MB_ICONERROR);
		DestroyWindow(hWnd);
	}


	while (1)
	{
		ZeroMemory(&OverlRd, sizeof(OverlRd));
		OverlRd.hEvent = canRead;
		ResetEvent(canRead);

		fSuccess = ReadFile(hPipe, &answer, sizeof(Message), &cbBytesRead, &OverlRd);

		WaitForSingleObject(canRead, INFINITE);
		GetOverlappedResult(hPipe, &OverlRd, &cbBytesRead, FALSE);
		if (cbBytesRead < sizeof(Message))
		{
			_stprintf(str, TEXT("ReadFile failed. Error: %d"), GetLastError());
			MessageBox(hWnd, str, TEXT("Error"), MB_ICONERROR);
			DestroyWindow(hWnd);
		}
		ProcessServerMessage(hWnd, answer);
		if (answer.code == R_LOGOUT)
			break;
	}
	return 1;
	
}
void writeClientRequest(HWND hWnd, Message request) {
	BOOL fSuccess = FALSE;
	DWORD cbWritten;
	OVERLAPPED OverlWr = { 0 };
	TCHAR str[NAMESIZE];

	
	ZeroMemory(&OverlWr, sizeof(OverlWr));
	ResetEvent(canWrite);
	OverlWr.hEvent = canWrite;


	fSuccess = WriteFile(hPipe, &request, sizeof(Message), &cbWritten, &OverlWr);
	GetOverlappedResult(hPipe, &OverlWr, &cbWritten, FALSE);

	if (cbWritten < sizeof(Message))
	{
		_stprintf(str, TEXT("File writing crash: %d"), GetLastError());
		MessageBox(hWnd, str, TEXT("Error"), MB_ICONERROR);
		DestroyWindow(hWnd);
	}
}

void sendGameParameters(HWND hWnd) {
	Message request;
	TCHAR str[NAMESIZE];


	request.game.bots = TRUE;
	request.playerNumber = 1;
	_tcscpy(request.player2Name, TEXT("unknown"));
	if (IsDlgButtonChecked(hWnd, IDC_CHECK2)) {
		GetDlgItemText(hWnd, IDC_EDIT1, request.player2Name, NAMESIZE);
		request.playerNumber = 2;
	}
	if (IsDlgButtonChecked(hWnd, IDC_CHECK3)) {
		request.game.bots = FALSE;
	}
	GetDlgItemText(hWnd, IDC_EDIT2, str, NAMESIZE);
	request.game.maxPlayers = _wtoi(str);
	GetDlgItemText(hWnd, IDC_EDIT3, str, NAMESIZE);
	request.game.bots = _wtoi(str);

	writeClientRequest(hWnd, request);
}

void sendRequest(HWND hWnd, int command) {
	int dim;
	TCHAR str[NAMESIZE];
	TCHAR aux[NAMESIZE];
	Message request;
	if (command == 1000) {
		GetDlgItemText(hWnd, IDC_EDIT1, str, 80);
		MessageBox(hWnd, str, TEXT("prioca"), MB_ICONEXCLAMATION);
	}

	_stprintf(aux, TEXT("%s"), username);
	_tcscpy(request.name, aux);
	request.code = command;
	writeClientRequest(hWnd, request);
}

int connectToServer(HWND hWnd) {
	HANDLE hThread;
	DWORD dwMode, dwThreadId = 0;
	TCHAR str[256];
	while (1)
	{
		hPipe = CreateFile(pipename, GENERIC_READ | GENERIC_WRITE, 0 | FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0 | FILE_FLAG_OVERLAPPED, NULL);
		if (hPipe != INVALID_HANDLE_VALUE) 
			break;

		if (GetLastError() != ERROR_PIPE_BUSY)
		{
			_stprintf(str, TEXT("No servers are online..."));
			MessageBox(hWnd, str, TEXT("Error"), MB_ICONERROR);
		}

		// Exit if an error other than ERROR_PIPE_BUSY occurs. 

		if (GetLastError() != ERROR_PIPE_BUSY)
		{
			return -1;
		}

		// All pipe instances are busy, so wait for 20 seconds. 

		if (!WaitNamedPipe(pipename, 20000))
		{
			return -1;
		}
	}
	dwMode = PIPE_READMODE_MESSAGE;
	fSuccess = SetNamedPipeHandleState(
		hPipe,    // pipe handle 
		&dwMode,  // new pipe mode 
		NULL,     // don't set maximum bytes 
		NULL);    // don't set maximum time 
	if (!fSuccess)
	{
		return -1;
	}
	hThread = CreateThread(NULL, 0, ClientThread, (LPVOID)hWnd, 0, &dwThreadId);
	if (hThread == NULL)
	{
		_stprintf(str, TEXT("Error while thread was being created. Error: %d\n "), GetLastError());
		MessageBox(hWnd, str, TEXT("Error"), MB_ICONERROR);
		DestroyWindow(hWnd);
	}

	canWrite = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (canWrite == NULL)
	{
		_stprintf(str, TEXT("It was not possible to create the event"));
		MessageBox(hWnd, str, TEXT("Error"), MB_ICONERROR);
		DestroyWindow(hWnd);
	}
}