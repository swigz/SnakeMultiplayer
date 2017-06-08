#pragma once
#include <tchar.h>
#include <windows.h> 
#include <memory.h> 
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
#include "Conts.h"

typedef struct Board {
	int* cells;
}board;
typedef struct Players {
	HANDLE hPipe;
	int status;
	int score;
	TCHAR name[NAMESIZE];
	float speed;
	int direction;
	int head, tail;
}players;

typedef struct Game {
	BOOL running;
	BOOL open;
	int maxPlayers;
	board board;
	players* players;
}game;

typedef struct Message {
	int playerNumber;
	TCHAR info[NAMESIZE];
	TCHAR name[NAMESIZE];
	TCHAR player2Name[NAMESIZE];
	int code;
	game game;
}Message;


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
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
		return TRUE;
}
//int MovePlayer(players pl, int dir, int key) {
//	switch (dir) {
//	case 0: //cima
//		if(key )
//	}
//}
void CriaMapaNormal(int x, int y, board &b) {
	b.cells = (int*)malloc(x * y);
	int i, j;
	//ciclo de preenchimento
	for (j = 0; j < y; j++) {
		for (i = 0; i < x; i++) {
			if (j == 0 || j == y - 1 || i == 0 || i == x - 1) {
				b.cells[(y*j) + i] = WALL;
			}
			else {
				b.cells[(y*j) + i] = BLANK;
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

