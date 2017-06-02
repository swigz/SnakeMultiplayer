#pragma once
#define N_LIN 15
#define N_COL 50
#define NAMESIZE 256

//DEFINES DE OBJECTOS E MAPAS
#define WALL 101
#define BLANK 100
#define TP_WALL 102

//responses comando do cliente
#pragma once
#define R_CONNECT 600
#define R_LOGOUT 601

//status do cliente
#define NOT_LOGGED 700
#define LOGGED_IN 701
#define PLAYING 702
#define WAITING_GAME 703

//response do servidor
#define ERROR_NAME_EXISTS 800
