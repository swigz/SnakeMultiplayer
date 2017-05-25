#pragma once
#include <tchar.h>
#include <windows.h> 
#include <memory.h> 
#include <stdio.h>
#include <stdlib.h>

//DEFINES DE OBJECTOS E MAPAS
#define WALL 1
#define BLANK 0
#define TP_WALL 101


#define SHMEMSIZE 4096 
typedef struct Board {
	int* cells;
}board;
typedef struct Players {
	int score;
	char* name;
	float speed;

}players;
typedef struct Game {
	int status;
	board board;
	players* players;
}game;
#ifdef DLL_EXPORTS
#define GAME_API __declspec(dllexport)
#else
#define GAME_API __declspec(dllimport)
#endif
static LPVOID lpvMem = NULL;      // pointer to shared memory
static HANDLE hMapObject = NULL;  // handle to file mapping


BOOL APIENTRY DllMain(HMODULE hModule,DWORD ul_reason_for_call, LPVOID lpReserved)
{
	BOOL fInit, fIgnore;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:

		/*LARGE_INTEGER tam;
		HANDLE f1, objMap1;
		char * map1;
		f1 = CreateFile(TEXT("ola.txt"), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (f1 == INVALID_HANDLE_VALUE) {
			_tprintf(TEXT("[Erro] Abrir ficheiro (%d)\n"), GetLastError());
			return -1;
		}
		_tprintf(TEXT("Criei file\n"));
		GetFileSizeEx(f1, &tam);


		objMap1 = CreateFileMapping(f1, NULL, PAGE_READONLY, 0, 0, NULL);
		if (objMap1 == NULL) {
			_tprintf(TEXT("[Erro]Mau mapeamento(%d)\n"), GetLastError());
			return -1;
		}
		_tprintf(TEXT("Criei mapping\n"));
		map1 = (char *)MapViewOfFile(objMap1, FILE_MAP_READ, 0, 0, 0);
		if (map1 == NULL) {
			_tprintf(TEXT("[Erro]Mapear para memória(%d)\n"), GetLastError());
			return -1;
		}
		_tprintf(TEXT("mapeeie\n"));
		UnmapViewOfFile(map1);
		CloseHandle(objMap1);
		CloseHandle(f1);
		_tprintf(TEXT("Fechando as alocações\n"));

		*/

		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
		return TRUE;
}
void CriaMapaNormal(int x, int y, board &b) {
	_tprintf(TEXT("vou alocar"));
	b.cells = (int*)malloc(x * y);
	int i, j;
	_tprintf(TEXT("Vou preencher %d \n"), x*y);
	//ciclo de preenchimento
	for (j = 0; j < y; j++) {
		for (i = 0; i < x; i++) {
			if (j == 0 || j == y - 1 || i == 0 || i == x - 1) {
				b.cells[(y*j) + i] = 1;
				//_tprintf(TEXT("parede \n"));
			}
			else {
				b.cells[(y*j) + i] = 0;
				//_tprintf(TEXT("vazio \n"));
			}
		}
	}

	
}


//Exportação

extern "C"
{

	GAME_API players p;
	GAME_API void Teste();
}

