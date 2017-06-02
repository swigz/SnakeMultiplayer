// GameServer.cpp : Defines the entry point for the console application.
//
#include "..\GameLibrary\GameLib.h";
#include "Comms.h"

Players* players;

int _tmain(int argc, TCHAR *argv[])
{
	listenForClients();

}

