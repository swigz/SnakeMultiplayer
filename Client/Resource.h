#pragma once
#define SHMEMSIZE 4096 
//status de jogo

#define INITIAL_MENU_SIZE 5
#define MAIN_MENU_SIZE 4
#define GAME_MENU_SIZE 5

#define NO_SERVER 704

#include "..\GameLibrary\GameLib.h"


LRESULT CALLBACK MainEvent(HWND, UINT, WPARAM, LPARAM);
void SetupInitialMenu(HWND hWnd);
void buttonClickEvent(HWND hWnd, WPARAM wParam);
void setupMainMenu(HWND hWnd);
void showMultipleElement(HWND hWnd[], UINT dim);
void showInitialMenu(HWND hWnd);
void showMainMenu(HWND hWnd);
BOOL CALLBACK createGameDialogProc(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam);
void sendGameParameters(HWND hWnd);