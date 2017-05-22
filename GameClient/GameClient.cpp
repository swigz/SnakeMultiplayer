#include "..\GameLibrary\GameLib.h"
#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <memory.h> 
typedef struct {
	int id;
}parametro;

#define SHMEMSIZE 4096 
#define N 2
DWORD WINAPI Thread(LPVOID param);
DWORD WINAPI FThread(LPVOID param);
HANDLE MutexOcupado;
void gotoxy(int x, int y);

int _tmain(int argc, TCHAR *argv[]) {
	int lin = 5, col=5;

	Teste();
	board board = { 0 };
	CriaMapaNormal(lin, col, board);
	int i, j;
	for (j = 0; j < col; j++) {
		for (i = 0; i < lin; i++) {
			_tprintf(TEXT("%d"),board.cells);
		}
		_tprintf(TEXT("\n"));
	}
	
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