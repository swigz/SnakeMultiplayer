#pragma once
#include <tchar.h>
#include <windows.h> 
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
	int status;
	bool isDead = FALSE;
	int score;
	TCHAR name[NAMESIZE];
	int speed=SNAKE_SPEED;
	int direction = RIGHT;
	bool drunk = FALSE;
	int headX;
	int headY;
	int tailX[100], tailY[100];
	int ntail = 0;

}players;
typedef struct Game {
	BOOL running;
	BOOL open;
	int maxPlayers;
	Board board;
	players p[MAX_PLAYERS];
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

game g;
//BOOL gameOver;
int Njogadores = MAX_PLAYERS;


//OBJ EFEITOS

DWORD WINAPI DurationThread(DWORD i) {
	Sleep(DURATION * 1000);

	g.p[i].speed = SNAKE_SPEED;
	
	return 0;
}

DWORD WINAPI O_DurationThread(DWORD i) {
	Sleep(DURATION * 1000);

	for (int c = 0; c<g.maxPlayers; c++)
		if (c != i)
			g.p[c].speed = SNAKE_SPEED;

	return 0;
}


DWORD WINAPI DrunkThread(DWORD i) {
	Sleep(DURATION * 1000);

	g.p[i].drunk=FALSE;

	return 0;
}

DWORD WINAPI O_DrunkThread(DWORD i) {
	Sleep(DURATION * 1000);

	for (int c = 0; c < g.maxPlayers; c++)
		if (c != i)
			g.p[c].drunk = FALSE;

	return 0;

}



void OilEffect(int i) {
	HANDLE hOilThread = NULL;
	DWORD dwOilThreadId = 0;
	
	
	g.p[i].speed=SNAKE_SPEED +10;


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

	for(int c=0;c<g.maxPlayers;c++)
		if(c!=i)
			g.p[c].speed = SNAKE_SPEED + 10;


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


	g.p[i].speed = SNAKE_SPEED - 10;


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


	for (int c = 0; c<g.maxPlayers; c++)
		if (c != i)
			g.p[c].speed = SNAKE_SPEED - 10;


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


	g.p[i].drunk=TRUE;


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


	for (int c = 0; c<g.maxPlayers; c++)
		if (c != i)
	g.p[c].drunk = TRUE;


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
	
	for (int i = 0; i < Njogadores; i++) {
		if (g.p[i].isDead == FALSE)
			return true;
		
	}
	return false;

}
void randomObject() {
	srand(time(NULL));
	int x, y,prob;
	x = rand() % MAP_COLUMNS - 2;
	y = rand() % MAP_ROWS - 2;
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

void CriaMapaNormal()
{

	int x, y;
	for (short int i = 0; i < MAP_COLUMNS; i++)
		for (short int j = 0; j < MAP_ROWS; j++)
			if (j == 0 || j == MAP_COLUMNS - 1 || i == 0 || i == MAP_ROWS - 1)
				g.board.cell[i][j].type = WALL;
			else
				g.board.cell[i][j].type = BLANK;

	while (g.board.Objects < MAX_OBJECTS)
	{
		srand(time(NULL));
		randomObject();
		g.board.Objects++;

	}

}


void SnakesSetup() {

	int x, y;


	for (int i = 0; i < Njogadores; i++) {
		srand(time(NULL));
		x = rand() % MAP_COLUMNS - 2;
		y = rand() % MAP_ROWS - 2;
		while (g.board.cell[x][y].type != BLANK) {
			x = rand() % MAP_COLUMNS - 2;
			y = rand() % MAP_ROWS - 2;

		}

			g.p[i].headX = x;
			g.p[i].headY = y;
			g.board.cell[x][y].type = SNAKE;

	}
}
void gameLogic(LPVOID param, Message answer) {

	for (int c = 0; c < Njogadores; c++) {
		if (g.p[c].hPipe == param && g.p[c].isDead== FALSE) {
			g.board.cell[g.p[c].headX][g.p[c].headY].type = BLANK;
			g.board.cell[g.p[c].tailX[g.p[c].ntail - 1]][g.p[c].tailY[g.p[c].ntail - 1]].type = BLANK;


			int prevX = g.p[c].tailX[0];
			int prevY = g.p[c].tailY[0];
			int prev2X, prev2Y;
			g.p[c].tailX[0] = g.p[c].headX;
			g.p[c].tailY[0] = g.p[c].headY;
			for (int i = 1; i < g.p[c].ntail; i++) {
				prev2X = g.p[c].tailX[i];
				prev2Y = g.p[c].tailY[i];
				g.p[c].tailX[i] = prevX;
				g.p[c].tailY[i] = prevY;
				prevX = prev2X;
				prevY = prev2Y;

			}


			switch (g.p[c].direction) {
			case UP:

				g.p[c].direction = UP;
				g.p[c].headX--;

				break;
			case DOWN:

				g.p[c].direction = DOWN;
				g.p[c].headX++;

				break;
			case RIGHT:

				g.p[c].direction = RIGHT;
				g.p[c].headY++;

				break;
			case LEFT:
				g.p[c].direction = LEFT;
				g.p[c].headY--;
				break;
			default:
				break;
			}
			if (g.board.cell[g.p[c].headX][g.p[c].headY].type == FOOD) {
				g.p[c].ntail++;
				g.p[c].score++;
				randomObject();

			}
			if (g.board.cell[g.p[c].headX][g.p[c].headY].type == ICE) {
				g.p[c].ntail--;
				g.p[c].score -= 2;
				randomObject();
			}
			if (g.board.cell[g.p[c].headX][g.p[c].headY].type == GRENADE) {
				g.p[c].isDead = TRUE;
				randomObject();
			}
			if (g.board.cell[g.p[c].headX][g.p[c].headY].type == VODKA) {
				VodkaEffect(c);
				randomObject();
			}
			if (g.board.cell[g.p[c].headX][g.p[c].headY].type == OIL) {
				OilEffect(c);
				randomObject();
			}
			if (g.board.cell[g.p[c].headX][g.p[c].headY].type == GLUE) {
				GlueEffect(c);
				randomObject();
			}
			if (g.board.cell[g.p[c].headX][g.p[c].headY].type == O_VODKA) {
				O_VodkaEffect(c);
				randomObject();
			}
			if (g.board.cell[g.p[c].headX][g.p[c].headY].type == O_OIL) {
				O_OilEffect(c);
				randomObject();
			}
			if (g.board.cell[g.p[c].headX][g.p[c].headY].type == O_GLUE) {
				O_GlueEffect(c);
				randomObject();
			}
			if (g.board.cell[g.p[c].headX][g.p[c].headY].type == WALL) {

				_tprintf(TEXT("\nJOGADOR %d PERDEU\n"), c + 1);
				g.p[c].isDead = TRUE;




			}
			if (g.board.cell[g.p[c].headX][g.p[c].headY].type == SNAKE ||
				g.board.cell[g.p[c].headX][g.p[c].headY].type == SNAKE) {
				_tprintf(TEXT("\nJOGADOR %d PERDEU\n"), c + 1);
				g.p[c].isDead = TRUE;

			}



			g.board.cell[g.p[c].headX][g.p[c].headY].type = SNAKE;

			g.board.cell[g.p[c].headX][g.p[c].headY].type = SNAKE;
			for (int i = 0; i < g.p[c].ntail; i++) {
				g.board.cell[g.p[c].tailX[i]][g.p[c].tailY[i]].type = SNAKE;
			}
		}

		for (short int i = 0; i < MAP_COLUMNS; i++)
			for (short int j = 0; j < MAP_ROWS; j++)
				if (j == 0 || j == MAP_COLUMNS - 1 || i == 0 || i == MAP_ROWS - 1)
					g.board.cell[i][j].type = WALL;

	}
	setgameOver();
}

void  draw() {
	int x, y;
	system("cls");
	//ciclo de preenchimento
	for (short int i = 0; i < MAP_COLUMNS; i++) {
		for (short int j = 0; j < MAP_ROWS; j++) {
			switch (g.board.cell[i][j].type) {
			case WALL:
				_tprintf(TEXT("X"));
				break;
			case BLANK:
				_tprintf(TEXT(" "));
				break;
			case FOOD:
				_tprintf(TEXT("o"));
				break;

			case SNAKE:

				for (int c = 0; c < Njogadores; c++) {
					if (i == g.p[c].headX && j == g.p[c].headY)
						_tprintf(TEXT("%d"), c + 1);
					else
						for (int k = 0; k < g.p[c].ntail; k++)
							if (i == g.p[c].tailX[k] && j == g.p[c].tailY[k])
								_tprintf(TEXT("%d"), c + 1);
				}
				break;
			}
		}
		_tprintf(TEXT("\n"));
	}

	



}

void input() {
	if (_kbhit()) {
		switch (_getch())
		{
		case 'a':
			if (g.p[0].direction != RIGHT)
				if(g.p[0].drunk)
					g.p[0].direction = RIGHT;
				else
				g.p[0].direction = LEFT;

			break;
		case 'w':
			if (g.p[0].direction != DOWN)
				if (g.p[0].drunk)
				g.p[0].direction = DOWN;
				else
					g.p[0].direction = UP;
			break;
		case 's':
			if (g.p[0].direction != UP)
				if (g.p[0].drunk)
				g.p[0].direction = UP;
				else
				g.p[0].direction = DOWN;
			break;
		case 'd':
			if (g.p[0].direction != LEFT)
				if (g.p[0].drunk)
					g.p[0].direction = LEFT;
				else
				g.p[0].direction = RIGHT;
			break;

		case 'j':
			if (g.p[1].direction != RIGHT)
				g.p[1].direction = LEFT;

			break;
		case 'i':
			if (g.p[1].direction != DOWN)
				g.p[1].direction = UP;
			break;
		case 'k':
			if (g.p[1].direction != UP)
				g.p[1].direction = DOWN;
			break;
		case 'l':
			if (g.p[1].direction != LEFT)
				g.p[1].direction = RIGHT;
			break;
		default:
			break;
		}
	}

}
//Exportação
extern "C"
{

	GAME_API players p;
	GAME_API void Teste();
}

