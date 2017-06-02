// GameLibrary.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "GameLib.h"

int ola=0;
void Teste() {
	if (ola == 0) {
		_tprintf(TEXT("TESTE -- 1"));
		ola++;
	}
	else if (ola == 1) {
		_tprintf(TEXT("TESTE -- 2"));
		ola++;
	}
	else {
		_tprintf(TEXT("TESTE -- 3"));
	}
	return;
}

