#pragma once
#define BUFSIZE 512
#define SHMEMSIZE 4096 
//status de jogo

#define INITIAL_MENU_SIZE 4

#define NO_SERVER 704

#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <memory.h> 
#include <conio.h>
#include "..\GameLibrary\GameLib.h"


LRESULT CALLBACK MainEvent(HWND, UINT, WPARAM, LPARAM);
void SetupInitialMenu(HWND hWnd);
void buttonClickEvent(HWND hWnd, WPARAM wParam);
void setupMainMenu(HWND hWnd);
void showMultipleElement(HWND hWnd[], UINT dim);
void showInitialMenu(HWND hWnd);
void showMainMenu(HWND hWnd);
