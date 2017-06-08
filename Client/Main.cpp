#include "ClientComm.h"
HINSTANCE hInstance;
TCHAR *szProgName = TEXT("Base");
HWND initialMenu[3];
HWND mainMenu[4];
HWND gameMenu[4];

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


void showInitialMenu(HWND hWnd) {
	showMultipleElement(initialMenu, 3);
}

void showGameMenu(HWND hWnd) {
	for (int i = 0; i < 4; i++) {
		hideSingleElement(mainMenu[i]);
	}
	showMultipleElement(gameMenu, 4);
}

void showMainMenu(HWND hWnd) {
	for (int i = 0; i < 3; i++) {
		hideSingleElement(initialMenu[i]);
	}
	showMultipleElement(mainMenu, 4);
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
	initialMenu[2] = CreateWindow(TEXT("BUTTON"), TEXT("GO!"), WS_CHILD, 450, 200, 150, 25, hWnd, (HMENU) LOGIN_BUTTON, NULL, NULL);
}

void setupGameMenu(HWND hWnd){
	gameMenu[0] = CreateWindow(TEXT("STATIC"), TEXT("Select an option"), WS_CHILD, 450, 150, 150, 25, hWnd, NULL, NULL, NULL);
	gameMenu[1] = CreateWindow(TEXT("BUTTON"), TEXT("Join Game"), WS_CHILD, 450, 175, 150, 25, hWnd, (HMENU)JOIN_GAME_BUTTON, NULL, NULL);
	gameMenu[2] = CreateWindow(TEXT("BUTTON"), TEXT("Create Game"), WS_CHILD, 450, 200, 150, 25, hWnd, (HMENU)CREATE_GAME_BUTTON, NULL, NULL);
	gameMenu[3] = CreateWindow(TEXT("BUTTON"), TEXT("Back"), WS_CHILD, 450, 225, 150, 25, hWnd, (HMENU)GAMEMENU_BACK_BUTTON, NULL, NULL);
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
	case LOGIN_BUTTON:
		nameLength = GetWindowTextLength(initialMenu[1]) + 1;
		if (nameLength <= 1) {
			MessageBox(hWnd, TEXT("Please type a name"), TEXT("Error"), MB_ICONERROR);
			break;
		}
		if (connectToServer(hWnd) == -1) {
			MessageBox(hWnd, TEXT("No servers are online."), TEXT("Error"), MB_ICONERROR);
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
		createGameDialog(hWnd);
		break;
	case LOGOUT_BUTTON:
		sendRequest(hWnd, R_LOGOUT);
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
			GetDlgItemText(hWnd, IDC_EDIT1, str, 80);
			MessageBox(hWnd, str, TEXT("prioca"), MB_ICONEXCLAMATION);
			sendRequest(hWnd, 1000);
			//clientStatus = INGAME_LOBBY;
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
		if (connectToServer(hWnd) == -1) {
			//MessageBox(hWnd, TEXT("No servers are online at the time."), TEXT("Error"), MB_ICONEXCLAMATION);
			//clientStatus = SERVERS_OFFLINE;
		}
		SetupInitialMenu(hWnd);
		setupMainMenu(hWnd);
		setupGameMenu(hWnd);
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
		default:
			break;
		}
		EndPaint(hWnd, &pt);
		break;
	case WM_KEYDOWN:
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
