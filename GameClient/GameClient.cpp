#include "..\GameLibrary\GameLib.h"
#include "Janelas.h"
#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <memory.h> 
#include <conio.h>
typedef struct {
	int id;
}parametro;
#define BUFSIZE 512
#define SHMEMSIZE 4096 
#define N 2
DWORD WINAPI Thread(LPVOID param);
DWORD WINAPI FThread(LPVOID param);
HANDLE MutexOcupado;
void gotoxy(int x, int y);

int _tmain(int argc, TCHAR *argv[]) {
	int lin = 7, col=7;

	/*Teste();
	board board = { 0 };
	CriaMapaNormal(lin, col, board);
	int i, j;
	for (j = 0; j < col; j++) {
		for (i = 0; i < lin; i++) {
			_tprintf(TEXT("%d "), board.cells[(col*j) + i]);
		}
		_tprintf(TEXT("\n"));
	}*/
	HANDLE hPipe;
	LPTSTR lpvMessage = TEXT("Default message from client.");
	TCHAR  chBuf[BUFSIZE];
	BOOL   fSuccess = FALSE;
	DWORD  cbRead, cbToWrite, cbWritten, dwMode;
	LPTSTR lpszPipename = TEXT("\\\\.\\pipe\\pipename");

	if (argc > 1)
		lpvMessage = argv[1];

	// Try to open a named pipe; wait for it, if necessary. 

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
			_tprintf(TEXT("Could not open pipe. GLE=%d\n"), GetLastError());
			return -1;
		}

		// All pipe instances are busy, so wait for 20 seconds. 

		if (!WaitNamedPipe(lpszPipename, 20000))
		{
			printf("Could not open pipe: 20 second wait timed out.");
			return -1;
		}
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
		_tprintf(TEXT("SetNamedPipeHandleState failed. GLE=%d\n"), GetLastError());
		return -1;
	}

	// Send a message to the pipe server. 

	cbToWrite = (lstrlen(lpvMessage) + 1) * sizeof(TCHAR);
	_tprintf(TEXT("Sending %d byte message: \"%s\"\n"), cbToWrite, lpvMessage);

	fSuccess = WriteFile(
		hPipe,                  // pipe handle 
		lpvMessage,             // message 
		cbToWrite,              // message length 
		&cbWritten,             // bytes written 
		NULL);                  // not overlapped 

	if (!fSuccess)
	{
		_tprintf(TEXT("WriteFile to pipe failed. GLE=%d\n"), GetLastError());
		return -1;
	}

	printf("\nMessage sent to server, receiving reply as follows:\n");

	do
	{
		// Read from the pipe. 

		fSuccess = ReadFile(
			hPipe,    // pipe handle 
			chBuf,    // buffer to receive reply 
			BUFSIZE * sizeof(TCHAR),  // size of buffer 
			&cbRead,  // number of bytes read 
			NULL);    // not overlapped 

		if (!fSuccess && GetLastError() != ERROR_MORE_DATA)
			break;

		_tprintf(TEXT("\"%s\"\n"), chBuf);
	} while (!fSuccess);  // repeat loop if ERROR_MORE_DATA 

	if (!fSuccess)
	{
		_tprintf(TEXT("ReadFile from pipe failed. GLE=%d\n"), GetLastError());
		return -1;
	}

	printf("\n<End of message, press ENTER to terminate connection and exit>");
	_getch();

	CloseHandle(hPipe);

	return 0;


	
}

int criarThread() {
	SYSTEMTIME instantei, instantef;
	TCHAR resp;
	DWORD threadId, tid; //Id da thread a ser criada
	HANDLE hT; //HANDLE/ponteiro para a thread a ser criada
	HANDLE hThreads[N];
	parametro myArg;
#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif
	srand((int)time(NULL));
	_tprintf(TEXT("Lançar Clientes (S/N)?"));
	_tscanf_s(TEXT("%c"), &resp, 1);
	if (resp == 'S' || resp == 's') {
		_tprintf(TEXT("Sou uma Thread com id %d...\n"), GetCurrentThreadId());

		MutexOcupado = CreateMutex(NULL, FALSE, TEXT("Meu mutex"));
		GetSystemTime(&instantei);
		for (int i = 0; i < N; i++) {
			myArg.id = i;
			_tprintf(TEXT("\n --- CLIENTE %d ----\n"), i + 1);
			hThreads[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)FThread, (LPVOID)&myArg, 0, &tid);
			if (hThreads[i] == INVALID_HANDLE_VALUE) {
				_tprintf(TEXT("Nao ha memoria para criar threads\n"));
				return -1;
			}

			Sleep(100);
		}
		SetThreadPriority(hThreads[0], THREAD_PRIORITY_LOWEST);
		TCHAR letra;
		do {
			fflush(stdin);
			letra = _gettch();
			for (int i = 0; i < N; i++)
				SuspendThread(hThreads[i]);
			letra = _gettch();
			for (int i = 0; i < N; i++)
				ResumeThread(hThreads[i]);
		} while (letra != 'b' && letra != 'B');

		WaitForMultipleObjects(N, hThreads, TRUE, INFINITE);
		gotoxy(0, 32);


		GetSystemTime(&instantef);
		_tprintf(TEXT("%02d:%02d:%d:%03d\n"), instantei.wHour, instantei.wMinute, instantei.wSecond, instantei.wMilliseconds);
		_tprintf(TEXT("%02d:%02d:%d:%03d\n"), instantef.wHour, instantef.wMinute, instantef.wSecond, instantef.wMilliseconds);
		FILETIME fti, ftf, fdiff;
		SystemTimeToFileTime(&instantei, &fti);
		SystemTimeToFileTime(&instantef, &ftf);
		ULARGE_INTEGER ui, uf, udiff;
		ui.HighPart = fti.dwHighDateTime;
		ui.LowPart = fti.dwLowDateTime;
		uf.HighPart = ftf.dwHighDateTime;
		uf.LowPart = ftf.dwLowDateTime;
		udiff.QuadPart = uf.QuadPart - ui.QuadPart;
		fdiff.dwHighDateTime = udiff.HighPart;
		fdiff.dwLowDateTime = udiff.LowPart;
		FileTimeToSystemTime(&fdiff, &instantei);
		_tprintf(TEXT("%02d:%02d:%d:%03d\n"), instantei.wHour, instantei.wMinute, instantei.wSecond, instantei.wMilliseconds);
		if (hThreads != NULL) {


			//_tprintf(TEXT("Lancei uma thread com id %d\n"), tid);
			Sleep(1000);
		}
		else
			_tprintf(TEXT("Erro ao criar Thread\n"));
	}
	_tprintf(TEXT("[Thread Principal %d]Vou terminar..."), GetCurrentThreadId());
	return 0;
}



DWORD WINAPI Thread(LPVOID param) {
	int i;
	int y = ((int)param);
	_tprintf(TEXT("TESTE da Thread %d \n Vou começar a trabalhar\n"), GetCurrentThreadId());
	Sleep(100);
	/*for (i = 0; i<LIM; i++) {

		gotoxy(y, y);
		_tprintf(TEXT("[%d]Thread %5d "), GetCurrentThreadId(), i);
	}*/

	Teste();

	return 0;
}

DWORD WINAPI FThread(LPVOID param) {

	parametro *ptr = (parametro*)param;
	parametro varlocal = *ptr;

	
		WaitForSingleObject(MutexOcupado, INFINITE);

		_tprintf(TEXT("\nThread [%d] TESTE \n"), varlocal.id);
		Teste();
		ReleaseMutex(MutexOcupado);


	return 0;
}

void gotoxy(int x, int y) {
	static HANDLE hStdout = NULL;
	COORD coord;
	coord.X = x;
	coord.Y = y;
	if (!hStdout)
		hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hStdout, coord);
}