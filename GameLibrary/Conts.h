#pragma once
#define NAMESIZE 256
#define MAX_PLAYERS 8

//DEFINES DE OBJECTOS E MAPAS
#define WALL 101
#define BLANK 100
#define TP_WALL 102

//butoes e afins
#define LOGIN_BUTTON 400
#define HELP_BUTTON 401
#define LOGOUT_BUTTON 402
#define GAME_MENU_BUTTON 403

//responses comando do cliente
#pragma once
#define R_CONNECT 600
#define R_LOGOUT 601
#define R_CHECK_GAME_STATUS 602

//status do cliente
#define NOT_LOGGED 700
#define LOGGED_IN 701
#define PLAYING 702
#define WAITING_GAME 703
#define SERVERS_OFFLINE 704

//response do servidor
#define SERVER_ERROR_NAME_EXISTS 800
#define SERVER_LOGIN_SUCCESS 801
#define SERVER_LOGIN_FAILED 802
#define SERVER_NO_GAME_RUNNING 803
#define SERVER_GAME_ACCEPTING 804
#define SERVER_GAME_IS_RUNNING 805
