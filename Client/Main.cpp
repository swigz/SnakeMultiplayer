#include "ClientComm.h"
HINSTANCE hInstance;
TCHAR *szProgName = TEXT("Base");
HWND initialMenu[INITIAL_MENU_SIZE];
HWND mainMenu[MAIN_MENU_SIZE];
HWND gameMenu[GAME_MENU_SIZE];
HWND lobbyMenu[1];

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {
	
	HWND hWnd;
	MSG lpMsg;		
	WNDCLASSEX wcApp;	
	BOOL ret;	
	clientStatus = NOT_LOGGED;
	wcApp.cbSize = sizeof(WNDCLASSEX);	
	wcApp.hInstance = hInst;
	wcApp.lpszClassName = szProgName;
	wcApp.lpfnWndProc = MainEvent;
	wcApp.style = CS_HREDRAW | CS_VREDRAW;
	wcApp.hIcon = LoadIcon(NULL, IDI_APPLICATION);
												 
	wcApp.hIconSm = LoadIcon(NULL, IDI_INFORMATION);
	wcApp.hCursor = LoadCursor(NULL, IDC_ARROW);	
	wcApp.lpszMenuName = NULL;			
	wcApp.cbClsExtra = 0;				
	wcApp.cbWndExtra = 0;		
	wcApp.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	hInstance = hInst;
	if (!RegisterClassEx(&wcApp))
		return(0);
	hWnd = CreateWindow(
		szProgName,		
		TEXT("Snek"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		(HWND)HWND_DESKTOP,	
		(HMENU)NULL,			
		(HINSTANCE)hInst,
		0);

	ShowWindow(hWnd, nCmdShow);	
	UpdateWindow(hWnd);		

	while ((ret = GetMessage(&lpMsg, NULL, 0, 0)) != 0) {
		if (ret != -1) {
			TranslateMessage(&lpMsg);	
			DispatchMessage(&lpMsg);	
		}
	}
	
	return((int)lpMsg.wParam);	
}

//HIDE AND SHOW ELEMENTS AND MENUS
void hideSingleElement(HWND hWnd)
{
	if (IsWindowVisible(hWnd))
		ShowWindow(hWnd, SW_HIDE);
	return;
}

void showSingleElement(HWND hWnd)
{
		if (!IsWindowVisible(hWnd))
			ShowWindow(hWnd, SW_SHOW);
}


void showMultipleElement(HWND hWnd[], UINT dim)
{
	if (dim < 1) return;
	for (short int i = 0; i < dim; i++)
		if (!IsWindowVisible(hWnd[i]))
			ShowWindow(hWnd[i], SW_SHOW);
}


void drawMap(HWND hWnd, HDC hDC, PAINTSTRUCT pt) {
	HDC MemDC;
	HBITMAP bm[28];
	TCHAR error[100];
	BOOL accessible;

	bm[0] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP29)); //BLANK
	bm[1] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP30)); //DRUNK
	bm[2] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP31)); //FOOD
	bm[3] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP32)); //GLUE
	bm[4] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP33)); //GRENADE
	bm[5] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP34)); //ICE
	bm[6] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP35)); //O_GLUE
	bm[7] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP36)); //O_OIL
	bm[8] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP37)); //0_VODKA
	bm[9] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP38)); //OIL
	bm[10] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP39)); //S1
	bm[11] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP40)); //S2
	bm[12] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP41)); //S3
	bm[13] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP42)); //S4
	bm[14] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP43)); //S5
	bm[15] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP44)); //S6
	bm[16] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP45)); //S7
	bm[17] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP46)); //S8
	bm[18] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP47)); //SH1
	bm[19] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP48)); //SH2
	bm[20] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP49)); //SH3
	bm[21] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP50)); //SH4
	bm[22] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP51)); //SH5
	bm[23] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP52)); //SH6
	bm[24] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP53)); //SH7
	bm[25] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP54)); //SH8
	bm[26] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP55)); //VODKA
	bm[27] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP56)); //WALL

	for (short int i = 0; i < 28; i++)
		if (bm[i] == NULL)
		{
			_stprintf(error, TEXT("Could not load IDB_BITMAP%d!"), i);
			MessageBox(hWnd, error, TEXT("Error"), MB_OK | MB_ICONEXCLAMATION);
			return;
		}
	MemDC = CreateCompatibleDC(hDC);
	CriaMapaNormal();


	for (int i = 0; i < MAP_ROWS; i++)
	{
		for (int j = 0; j < MAP_COLUMNS; j++) {
			switch (g.board.cell[i][j].type) {
			case WALL:
				SelectObject(MemDC, bm[27]);
				break;			
			case BLANK:
				SelectObject(MemDC, bm[0]);
				break;
			case FOOD:
				SelectObject(MemDC, bm[2]);
				break;
			case GLUE:
				SelectObject(MemDC, bm[3]);
				break;
			case GRENADE:
				SelectObject(MemDC, bm[4]);
				break;
			case ICE:
				SelectObject(MemDC, bm[5]);
				break;
			case VODKA:
				SelectObject(MemDC, bm[26]);
				break;
			case O_GLUE:
				SelectObject(MemDC, bm[6]);
				break;
			case O_OIL:
				SelectObject(MemDC, bm[7]);
				break;
			case O_VODKA:
				SelectObject(MemDC, bm[8]);
				break;
			case OIL:
				SelectObject(MemDC, bm[9]);
				break;
			case SNAKEHEAD:
				SelectObject(MemDC, bm[18+getPlayerId(i,j)]);
				break;
			case SNAKE:
				SelectObject(MemDC, bm[10 + getPlayerId(i, j)]);
				break;
			}
			StretchBlt(hDC, MAP_Y + ((j - 1)*IMG_SIZE + 50), MAP_X + ((i - 1)*IMG_SIZE + 25), 70, 70, MemDC, 0, 0, 35, 35, SRCCOPY);
		}
	
		
	}
	
	//// Restore the old bitmap
	DeleteDC(MemDC);
	for (short int i = 0; i < 28; i++)
		DeleteObject(bm[i]);



}


void clearMap(HWND hWnd) {

	ReleaseDC(hWnd, GetDC(hWnd));
}





void showInitialMenu(HWND hWnd) {
	showMultipleElement(initialMenu, INITIAL_MENU_SIZE);
}

void showGameMenu(HWND hWnd) {
	for (int i = 0; i < MAIN_MENU_SIZE; i++) {
		hideSingleElement(mainMenu[i]);
	}
	showMultipleElement(gameMenu, GAME_MENU_SIZE);
}

void showMainMenu(HWND hWnd) {
	for (int i = 0; i < INITIAL_MENU_SIZE; i++) {
		hideSingleElement(initialMenu[i]);
	}
	showMultipleElement(mainMenu, MAIN_MENU_SIZE);
}

void showLobbyMenu(HWND hWnd) {
	for (short int i = 0; i < GAME_MENU_SIZE; i++) {
		hideSingleElement(gameMenu[i]);
	}
	showSingleElement(lobbyMenu[0]);
}

void showInGameScreen(HWND hWnd, HDC hDC, PAINTSTRUCT pt) {
	hideSingleElement(lobbyMenu[0]);
	drawMap(hWnd, hDC, pt);

}


//CREATE MENU BUTTONS

void setupMainMenu(HWND hWnd) {
	mainMenu[0] = CreateWindow(TEXT("STATIC"), TEXT("Welcome to snek"), WS_CHILD, 450, 25, 150, 25, hWnd, NULL, NULL, NULL);
	mainMenu[1] = CreateWindow(TEXT("BUTTON"), TEXT("Play a Game"), WS_CHILD, 450, 100, 150, 25, hWnd, (HMENU)GAME_MENU_BUTTON, NULL, NULL);
	mainMenu[2] = CreateWindow(TEXT("BUTTON"), TEXT("Help??"), WS_CHILD, 450, 125, 150, 25, hWnd, (HMENU)HELP_BUTTON, NULL, NULL);
	mainMenu[3] = CreateWindow(TEXT("BUTTON"), TEXT("Logout"), WS_CHILD, 450, 150, 150, 25, hWnd, (HMENU)LOGOUT_BUTTON, NULL, NULL);
}

void SetupInitialMenu(HWND hWnd) {
	initialMenu[0] = CreateWindow(TEXT("STATIC"), TEXT("Type your username"), WS_CHILD, 450, 150, 150, 25, hWnd, NULL, NULL, NULL);
	initialMenu[1] = CreateWindow(TEXT("EDIT"), TEXT(""), WS_CHILD | WS_BORDER, 450, 175, 150, 25, hWnd, NULL, NULL, NULL);
	initialMenu[2] = CreateWindow(TEXT("BUTTON"), TEXT("Connect Local"), WS_CHILD, 450, 200, 150, 25, hWnd, (HMENU) LOGIN_LOCAL_BUTTON, NULL, NULL);
	initialMenu[3] = CreateWindow(TEXT("EDIT"), TEXT(""), WS_CHILD | WS_BORDER, 450, 250, 150, 25, hWnd, NULL, NULL, NULL);
	initialMenu[4] = CreateWindow(TEXT("BUTTON"), TEXT("Connect Remote"), WS_CHILD, 450, 275, 150, 25, hWnd, (HMENU)LOGIN_REMOTE_BUTTON, NULL, NULL);
}

void setupGameMenu(HWND hWnd){
	gameMenu[0] = CreateWindow(TEXT("STATIC"), TEXT("Select an option"), WS_CHILD, 450, 150, 150, 25, hWnd, NULL, NULL, NULL);
	gameMenu[1] = CreateWindow(TEXT("BUTTON"), TEXT("Join Game"), WS_CHILD, 450, 175, 150, 25, hWnd, (HMENU)JOIN_GAME_BUTTON, NULL, NULL);
	gameMenu[2] = CreateWindow(TEXT("BUTTON"), TEXT("Create Game"), WS_CHILD, 450, 200, 150, 25, hWnd, (HMENU)CREATE_GAME_BUTTON, NULL, NULL);
	gameMenu[3] = CreateWindow(TEXT("BUTTON"), TEXT("Refresh server Status"), WS_CHILD, 450, 225, 150, 25, hWnd, (HMENU)REFRESH_STATUS_BUTTON, NULL, NULL);
	gameMenu[4] = CreateWindow(TEXT("BUTTON"), TEXT("Back"), WS_CHILD, 450, 250, 150, 25, hWnd, (HMENU)GAMEMENU_BACK_BUTTON, NULL, NULL);
}
void setupLobbyMenu(HWND hWnd) {
	lobbyMenu[0] = CreateWindow(TEXT("BUTTON"), TEXT("Start"), WS_CHILD, 450, 200, 150, 25, hWnd, (HMENU)LOBBYMENU_START_BUTTON, NULL, NULL);
}

void createGameDialog(HWND hWnd) {
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, (DLGPROC)createGameDialogProc);
}


void exitApp(HWND hWnd) {
	DestroyWindow(hWnd);
}


//WINDOWS EVENTS

void buttonClickEvent(HWND hWnd, WPARAM wParam) {
	int nameLength = 0;
	switch (LOWORD(wParam)) {
	case LOGIN_LOCAL_BUTTON:
		if (clientStatus == SERVERS_OFFLINE) {
			if (connectToServer(hWnd) == -1) {
				MessageBox(hWnd, TEXT("No servers are online at the time."), TEXT("Error"), MB_ICONEXCLAMATION);
				break;
			}
		}
		nameLength = GetWindowTextLength(initialMenu[1]) + 1;
		if (nameLength <= 1) {
			MessageBox(hWnd, TEXT("Please type a name"), TEXT("Error"), MB_ICONERROR);
			break;
		}
		GetWindowText(initialMenu[1], username, nameLength);
		sendRequest(hWnd, R_CONNECT);
		break;

	case GAME_MENU_BUTTON:
		clientStatus = INGAME_MENU;
		sendRequest(hWnd, R_CHECK_GAME_STATUS);
		InvalidateRect(hWnd, NULL, 1);
		break;
	case CREATE_GAME_BUTTON:
		if (gameStatus != SERVER_NO_GAME_RUNNING) {
			MessageBox(hWnd, TEXT("There is a game running or a game accepting players. Hit refresh to refresh."), TEXT("Warning"), MB_ICONHAND);
			break;
		}
		createGameDialog(hWnd);
		break;
	case REFRESH_STATUS_BUTTON:
		sendRequest(hWnd, R_CHECK_GAME_STATUS);
		MessageBox(hWnd, TEXT("Server Status refreshed!"), TEXT("YAY!"), MB_ICONEXCLAMATION);
		break;
	case GAMEMENU_BACK_BUTTON:
		for (short int i = 0; i < GAME_MENU_SIZE; i++) {
			hideSingleElement(gameMenu[i]);
		}
		clientStatus = LOGGED_IN;
		InvalidateRect(hWnd, NULL, 1);
		break;
	case LOGOUT_BUTTON:
		sendRequest(hWnd, R_LOGOUT);
		break;
	case JOIN_GAME_BUTTON:
		sendRequest(hWnd, R_JOINGAME);
		break;
	case LOBBYMENU_START_BUTTON:
		//sendRequest(hWnd, R_STARTGAME);
		clientStatus = PLAYING;
		InvalidateRect(hWnd, NULL, 1);
		break;
	default:
		break;
	}
}

void keyboardPressed(HWND hWnd, WPARAM param) {
	if (clientStatus == PLAYING) {
		switch (param) {
		case VK_RIGHT: //players 1 direita
			sendRequest(hWnd, R_1_MOVERIGHT);
			break;
		case VK_LEFT: //player 1 esquerda
			sendRequest(hWnd, R_1_MOVELEFT);
			break;
		case VK_DOWN: //player 1 baixo
			sendRequest(hWnd, R_1_MOVEDOWN);
			break;
		case VK_UP: //player 1 cima
			sendRequest(hWnd, R_1_MOVEUP);
			break;
		case VK_NUMPAD8: //player 2 cima
			sendRequest(hWnd, R_2_MOVEUP);
			break;
		case VK_NUMPAD4: //player 2 esquerda
			sendRequest(hWnd, R_2_MOVELEFT);
			break;
		case VK_NUMPAD2: //player 2 baixo
			sendRequest(hWnd, R_2_MOVEDOWN);
			break;
		case VK_NUMPAD6: //player 2 direita
			sendRequest(hWnd, R_2_MOVERIGHT);
			break;
		default:
			break;
		}
	}
	
}


BOOL CALLBACK createGameDialogProc(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam) {
	TCHAR str[NAMESIZE];
	switch (messg) {
	case WM_INITDIALOG:
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			sendGameParameters(hWnd);
			EndDialog(hWnd, 0);
			break;
		case IDCANCEL:
			EndDialog(hWnd, 0);
			return 0;
			break;
		default:
			break;
		}
	
	}
	return FALSE;
}
LRESULT CALLBACK MainEvent(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam) {
	HDC hDC;
	PAINTSTRUCT pt;

	switch (messg) {
	case WM_CREATE:
		if (connectToServer(hWnd) == -1) 
			clientStatus = SERVERS_OFFLINE;
		SetupInitialMenu(hWnd);
		setupMainMenu(hWnd);
		setupGameMenu(hWnd);
		setupLobbyMenu(hWnd);
		InvalidateRect(hWnd, NULL, 1);
		break;
	case WM_COMMAND:
		buttonClickEvent(hWnd, wParam);
		break;
	case WM_PAINT:
		//Atualizador das vistas
		hDC = BeginPaint(hWnd, &pt);

		switch (clientStatus)
		{
		case NOT_LOGGED:
		case SERVERS_OFFLINE:
			showInitialMenu(hWnd);
			break;
		case LOGGED_IN:
			showMainMenu(hWnd);
			break;
		case INGAME_MENU:
			showGameMenu(hWnd);
			break;
		case INGAME_LOBBY:
			showLobbyMenu(hWnd);
			break;
		case PLAYING:
			showInGameScreen(hWnd, hDC, pt);
			break;
		default:
			break;
		}
		EndPaint(hWnd, &pt);
		break;
	case WM_KEYDOWN:
		keyboardPressed(hWnd, wParam);
		break;
	case WM_DESTROY:
		closeClient(hPipe);
		PostQuitMessage(0);
		break;
	default:		
		return(DefWindowProc(hWnd, messg, wParam, lParam));
	}
	return(0);
}
