#include "Resource.h"
HANDLE hPipe;
BOOL fSuccess = FALSE;
DWORD  cbRead, cbToWrite, cbWritten, dwMode;
LPTSTR lpszPipename = TEXT("\\\\.\\pipe\\pipename");
TCHAR username[256];
int clientStatus;

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
	default:
		break;
	}
	InvalidateRect(hWnd, NULL, 1);
}

DWORD WINAPI ClientThread(LPVOID param) {
	DWORD cbBytesRead = 0;
	BOOL fSuccess = FALSE;
	HANDLE ReadReady;
	OVERLAPPED OverlRd = { 0 };
	HWND hWnd = (HWND)param;
	TCHAR str[NAMESIZE];
	Message answer;

	while (1)
	{
		fSuccess = ReadFile(hPipe, &answer, sizeof(Message), &cbBytesRead, &OverlRd);

		//WaitForSingleObject(ReadReady, INFINITE);
		//GetOverlappedResult(hPipe, &OverlRd, &cbBytesRead, FALSE);
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


	fSuccess = WriteFile(hPipe, &request, sizeof(Message), &cbWritten, &OverlWr);
	if (cbWritten < sizeof(Message))
	{
		_stprintf(str, TEXT("WriteFile maybe failed. Error: %d"), GetLastError());
		MessageBox(hWnd, str, TEXT("Error"), MB_ICONERROR);
		DestroyWindow(hWnd);
	}
}

void sendRequest(HWND hWnd, int command) {
	int dim;
	TCHAR aux[NAMESIZE];
	Message request;

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
		hPipe = CreateFile(
			lpszPipename,   // pipe name 
			GENERIC_READ |  // read and write access 
			GENERIC_WRITE,
			0,              // no sharing 
			NULL,           // default security attributes
			OPEN_EXISTING,  // opens existing pipe 
			0,              // default attributes 
			NULL);          // no template file 

							// Break if the pipe handle is valid. 

		if (hPipe != INVALID_HANDLE_VALUE)
			break;

		// Exit if an error other than ERROR_PIPE_BUSY occurs. 

		if (GetLastError() != ERROR_PIPE_BUSY)
		{
			return -1;
		}

		// All pipe instances are busy, so wait for 20 seconds. 

		if (!WaitNamedPipe(lpszPipename, 20000))
		{
			return -1;
		}
	}
	hThread = CreateThread(NULL, 0, ClientThread, (LPVOID)hWnd, 0, &dwThreadId);
	if (hThread == NULL)
	{
		_stprintf(str, TEXT("Error while thread was being created. Error: %d\n "), GetLastError());
		MessageBox(hWnd, str, TEXT("Error"), MB_ICONERROR);
		DestroyWindow(hWnd);
	}
	// The pipe connected; change to message-read mode. 

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
	return 1;
}