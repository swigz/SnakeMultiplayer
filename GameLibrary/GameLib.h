#pragma once
#include <tchar.h>

#include <windows.h>
#include <sddl.h>
#include <memory.h> 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <io.h>
#include <fcntl.h>
#include "Conts.h"
#include <conio.h>


typedef struct Cell {
	int type;
}Cell;
typedef struct Board {
	int Objects = 0;
	Cell cell[MAP_ROWS][MAP_COLUMNS];
}Board;

typedef struct Players {
	HANDLE hPipe;
	BOOL inGame = FALSE;
	BOOL isDead = FALSE;
	BOOL isMulti;
	int score;
	TCHAR name[NAMESIZE];
	int speed=SNAKE_SPEED;
	int direction = RIGHT;
	BOOL drunk = FALSE;
	int headX;
	int headY;
	int tailX[100], tailY[100];
	int ntail = 0;
	int id;

}jogadores;

typedef struct Game {
	BOOL running;
	BOOL open;
	int bots=0;
	int maxPlayers=MAX_PLAYERS;
	Board board;
}game;

typedef struct Message {
	int aux;
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
jogadores players[MAX_PLAYERS];
//BOOL gameOver;
int libMaxPlayers=2;


//OBJ EFEITOS

int getPlayerId(int x, int y) {
	for (int i = 0; i<libMaxPlayers; i++)
		if (players[i].inGame)
		{
			if (players[i].headX == x && players[i].headY == y)
				return players[i].id;
			else {
				for (int j = 0; j<players[i].ntail; j++)
					if (players[i].tailX[j] == x && players[i].tailY[j] == y)
						return players[i].id;
			}
		}


}

DWORD WINAPI DurationThread(DWORD i) {
	Sleep(DURATION * 1000);

	players[i].speed = SNAKE_SPEED;
	
	return 0;
}

DWORD WINAPI O_DurationThread(DWORD i) {
	Sleep(DURATION * 1000);

	for (int c = 0; c<libMaxPlayers; c++)
		if (c != i)
			players[c].speed = SNAKE_SPEED;

	return 0;
}


DWORD WINAPI DrunkThread(DWORD i) {
	Sleep(DURATION * 1000);

	players[i].drunk=FALSE;

	return 0;
}

DWORD WINAPI O_DrunkThread(DWORD i) {
	Sleep(DURATION * 1000);

	for (int c = 0; c < libMaxPlayers; c++)
		if (c != i)
			players[c].drunk = FALSE;

	return 0;

}



void OilEffect(int i) {
	HANDLE hOilThread = NULL;
	DWORD dwOilThreadId = 0;
	
	
	players[i].speed=SNAKE_SPEED +10;


	hOilThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)DurationThread, (LPVOID)i, 0, &dwOilThreadId);
	if (hOilThread == NULL)
	{
		_tprintf(TEXT("\nError during oil thread creation"));
		system("pause");
	}
	else
		CloseHandle(hOilThread);
}
void O_OilEffect(int i) {
	HANDLE hOilThread = NULL;
	DWORD dwOilThreadId = 0;

	for(int c=0;c<libMaxPlayers;c++)
		if(c!=i)
			players[c].speed = SNAKE_SPEED + 10;


	hOilThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)O_DurationThread, (LPVOID)i, 0, &dwOilThreadId);
	if (hOilThread == NULL)
	{
		_tprintf(TEXT("\nError during o_oil thread creation"));
		system("pause");

	}
	else
		CloseHandle(hOilThread);
}


void GlueEffect(int i) {
	HANDLE hGlueThread = NULL;
	DWORD dwGlueThreadId = 0;


	players[i].speed = SNAKE_SPEED - 10;


	hGlueThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)DurationThread, (LPVOID)i, 0, &dwGlueThreadId);
	if (hGlueThread == NULL)
	{
		_tprintf(TEXT("\nError during oil thread creation"));
		system("pause");

	}
	else
		CloseHandle(hGlueThread);
}
void O_GlueEffect(int i) {
	HANDLE hGlueThread = NULL;
	DWORD dwGlueThreadId = 0;


	for (int c = 0; c<libMaxPlayers; c++)
		if (c != i)
			players[c].speed = SNAKE_SPEED - 10;


	hGlueThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)O_DurationThread, (LPVOID)i, 0, &dwGlueThreadId);
	if (hGlueThread == NULL)
	{
		_tprintf(TEXT("\nError during oil thread creation"));
		system("pause");

	}
	else
		CloseHandle(hGlueThread);
}



void VodkaEffect(int i) {
	HANDLE hVodkaThread = NULL;
	DWORD dwVodkaThreadId = 0;


	players[i].drunk=TRUE;


	hVodkaThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)DrunkThread, (LPVOID)i, 0, &dwVodkaThreadId);
	if (hVodkaThread == NULL)
	{
		_tprintf(TEXT("\nError during Vodka thread creation"));
		system("pause");

	}
	else
		CloseHandle(hVodkaThread);
}
void O_VodkaEffect(int i) {
	HANDLE hVodkaThread = NULL;
	DWORD dwVodkaThreadId = 0;


	for (int c = 0; c<libMaxPlayers; c++)
		if (c != i)
	players[i].drunk = TRUE;


	hVodkaThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)O_DrunkThread, (LPVOID)i, 0, &dwVodkaThreadId);
	if (hVodkaThread == NULL)
	{
		_tprintf(TEXT("\nError during Vodka thread creation"));
		system("pause");

	}
	else
		CloseHandle(hVodkaThread);
}



BOOL setgameOver() {
	
	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (players[i].isDead == FALSE)
			return true;
		
	}
	return false;

}
game randomObject(game g) {
	time_t t;
	int x=0, y=0,prob=0;
	x = rand() % MAP_SIZE - 2;
	y = rand() % MAP_SIZE - 2;
	while (g.board.cell[x][y].type != BLANK) {
		x = rand() % MAP_COLUMNS - 2;
		y = rand() % MAP_ROWS - 2;
		
	}
	prob = rand() % 99;
	if(prob <=40)
		g.board.cell[x][y].type = FOOD;
	if(prob > 40 && prob <=60)
		g.board.cell[x][y].type = ICE;
	if (prob > 60 && prob <=70)
		g.board.cell[x][y].type = GRENADE;
	if (prob > 70 && prob <= 80)
		g.board.cell[x][y].type = VODKA;
	if (prob > 80 && prob <= 85)
		g.board.cell[x][y].type = OIL;
	if (prob > 85 && prob <= 90)
		g.board.cell[x][y].type = GLUE;
	if (prob > 90 && prob <= 93)
		g.board.cell[x][y].type = O_VODKA;
	if (prob > 93 && prob <= 96)
		g.board.cell[x][y].type = O_OIL;
	if (prob > 96)
		g.board.cell[x][y].type = O_GLUE;
	return g;
}


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

game CriaMapaNormal(game g)
{
	int n=0;
	int x, y;

	for (short int i = 0; i < MAP_COLUMNS; i++)
		for (short int j = 0; j < MAP_ROWS; j++)
			if (j == 0 || j == MAP_COLUMNS - 1 || i == 0 || i == MAP_ROWS - 1) {
				g.board.cell[i][j].type = WALL;
				_tprintf(TEXT("CRIOU OBJETOS RIJOS \n"));
			}
			else
				g.board.cell[i][j].type = BLANK;



	while (n < MAX_OBJECTS)
	{
		
		g = randomObject(g);
		n++;

	}
	_tprintf(TEXT("CRIOU OBJETOS RIJOS \n"));


	return g;
}


game SnakesSetup(game g) {

	int x, y;
	
	for (int i = 0; i < libMaxPlayers; i++) {
		_tprintf(TEXT("prioca"));
		srand(time(NULL));
		
		x = rand() % MAP_COLUMNS - 2;
		y = rand() % MAP_ROWS - 2;
	
		while (g.board.cell[x][y].type != BLANK) {
			_tprintf(TEXT("prioca"));
			x = rand() % MAP_COLUMNS - 2;
			y = rand() % MAP_ROWS - 2;

		}
		
		players[i].headX = x;
		players[i].headY = y;
		g.board.cell[x][y].type = SNAKEHEAD;

	}
	return g;
}
game gameLogic(game g) {

	for (int c = 0; c < MAX_PLAYERS; c++) {
		if (players[c].isDead== FALSE) {
			g.board.cell[players[c].headX][players[c].headY].type = BLANK;
			g.board.cell[players[c].tailX[players[c].ntail - 1]][players[c].tailY[players[c].ntail - 1]].type = BLANK;


			int prevX = players[c].tailX[0];
			int prevY = players[c].tailY[0];
			int prev2X, prev2Y;
			players[c].tailX[0] = players[c].headX;
			players[c].tailY[0] = players[c].headY;
			for (int i = 1; i < players[c].ntail; i++) {
				prev2X = players[c].tailX[i];
				prev2Y = players[c].tailY[i];
				players[c].tailX[i] = prevX;
				players[c].tailY[i] = prevY;
				prevX = prev2X;
				prevY = prev2Y;

			}


			switch (players[c].direction) {
			case UP:

				players[c].direction = UP;
				players[c].headX--;

				break;
			case DOWN:

				players[c].direction = DOWN;
				players[c].headX++;

				break;
			case RIGHT:

				players[c].direction = RIGHT;
				players[c].headY++;

				break;
			case LEFT:
				players[c].direction = LEFT;
				players[c].headY--;
				break;
			default:
				break;
			}
			if (g.board.cell[players[c].headX][players[c].headY].type == FOOD) {
				players[c].ntail++;
				players[c].score++;
				randomObject(g);

			}
			if (g.board.cell[players[c].headX][players[c].headY].type == ICE) {
				players[c].ntail--;
				players[c].score -= 2;
				randomObject(g);
			}
			if (g.board.cell[players[c].headX][players[c].headY].type == GRENADE) {
				players[c].isDead = TRUE;
				randomObject(g);
			}
			if (g.board.cell[players[c].headX][players[c].headY].type == VODKA) {
				VodkaEffect(c);
				randomObject(g);
			}
			if (g.board.cell[players[c].headX][players[c].headY].type == OIL) {
				OilEffect(c);
				randomObject(g);
			}
			if (g.board.cell[players[c].headX][players[c].headY].type == GLUE) {
				GlueEffect(c);
				randomObject(g);
			}
			if (g.board.cell[players[c].headX][players[c].headY].type == O_VODKA) {
				O_VodkaEffect(c);
				randomObject(g);
			}
			if (g.board.cell[players[c].headX][players[c].headY].type == O_OIL) {
				O_OilEffect(c);
				randomObject(g);
			}
			if (g.board.cell[players[c].headX][players[c].headY].type == O_GLUE) {
				O_GlueEffect(c);
				randomObject(g);
			}
			if (g.board.cell[players[c].headX][players[c].headY].type == WALL) {

				_tprintf(TEXT("\nJOGADOR %d PERDEU\n"), c + 1);
				players[c].isDead = TRUE;




			}
			if (g.board.cell[players[c].headX][players[c].headY].type == SNAKE ||
				g.board.cell[players[c].headX][players[c].headY].type == SNAKEHEAD) {
				_tprintf(TEXT("\nJOGADOR %d PERDEU\n"), c + 1);
				players[c].isDead = TRUE;

			}



			g.board.cell[players[c].headX][players[c].headY].type = SNAKEHEAD;

			for (int i = 0; i < players[c].ntail; i++) {
				g.board.cell[players[c].tailX[i]][players[c].tailY[i]].type = SNAKE;
			}
		}

		for (short int i = 0; i < MAP_COLUMNS; i++)
			for (short int j = 0; j < MAP_ROWS; j++)
				if (j == 0 || j == MAP_COLUMNS - 1 || i == 0 || i == MAP_ROWS - 1)
					g.board.cell[i][j].type = WALL;
		
	}
	if(!setgameOver())
	_tprintf(TEXT("UPDATING...\n"));
	else 
		_tprintf(TEXT("DAT BOI DIEDEDED\n"));
	return g;
}

//void  draw() {
//	int x, y;
//	system("cls");
//	//ciclo de preenchimento
//	for (short int i = 0; i < MAP_COLUMNS; i++) {
//		for (short int j = 0; j < MAP_ROWS; j++) {
//			switch (g.board.cell[i][j].type) {
//			case WALL:
//				_tprintf(TEXT("X"));
//				break;
//			case BLANK:
//				_tprintf(TEXT(" "));
//				break;
//			case FOOD:
//				_tprintf(TEXT("o"));
//				break;
//
//			case SNAKE:
//
//				for (int c = 0; c < g.maxPlayers; c++) {
//					if (i == players[c].headX && j == players[c].headY)
//						_tprintf(TEXT("%d"), c + 1);
//					else
//						for (int k = 0; k < players[c].ntail; k++)
//							if (i == players[c].tailX[k] && j == players[c].tailY[k])
//								_tprintf(TEXT("%d"), c + 1);
//				}
//				break;
//			}
//		}
//		_tprintf(TEXT("\n"));
//	}
//}

//void input() {
//	if (_kbhit()) {
//		switch (_getch())
//		{
//		case 'a':
//			if (g.p[0].direction != RIGHT)
//				if(g.p[0].drunk)
//					g.p[0].direction = RIGHT;
//				else
//				g.p[0].direction = LEFT;
//
//			break;
//		case 'w':
//			if (g.p[0].direction != DOWN)
//				if (g.p[0].drunk)
//				g.p[0].direction = DOWN;
//				else
//					g.p[0].direction = UP;
//			break;
//		case 's':
//			if (g.p[0].direction != UP)
//				if (g.p[0].drunk)
//				g.p[0].direction = UP;
//				else
//				g.p[0].direction = DOWN;
//			break;
//		case 'd':
//			if (g.p[0].direction != LEFT)
//				if (g.p[0].drunk)
//					g.p[0].direction = LEFT;
//				else
//				g.p[0].direction = RIGHT;
//			break;
//
//		case 'j':
//			if (g.p[1].direction != RIGHT)
//				g.p[1].direction = LEFT;
//
//			break;
//		case 'i':
//			if (g.p[1].direction != DOWN)
//				g.p[1].direction = UP;
//			break;
//		case 'k':
//			if (g.p[1].direction != UP)
//				g.p[1].direction = DOWN;
//			break;
//		case 'l':
//			if (g.p[1].direction != LEFT)
//				g.p[1].direction = RIGHT;
//			break;
//		default:
//			break;
//		}
//	}
//
//}
//Exportação
extern "C"
{

	GAME_API void Teste();
}

