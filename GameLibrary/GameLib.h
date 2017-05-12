#pragma once
#include <tchar.h>
#include <Windows.h>

typedef struct Game {
	int status;
	int batatas;
}game;
#ifdef DLL_EXPORTS
#define GAME_API __declspec(dllexport)
#else
#define GAME_API __declspec(dllimport)
#endif

extern "C"
{
	GAME_API void Teste();
}