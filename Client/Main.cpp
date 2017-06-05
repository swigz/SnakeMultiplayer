#include "ClientComm.h"

TCHAR *szProgName = TEXT("Base");
HWND initialMenu[3];
HWND mainMenu[4];
HWND gameMenu[5];

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

void showMainMenu(HWND hWnd) {
	for (int i = 0; i < 3; i++) {
		hideSingleElement(initialMenu[i]);
	}
	showMultipleElement(mainMenu, 4);
}
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
void setupGameMenu(HWND hWnd) {

}

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
		sendRequest(hWnd, R_CHECK_GAME_STATUS);
		break;
	case LOGOUT_BUTTON:
		sendRequest(hWnd, R_LOGOUT);
		break;
	default:
		break;
	}
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
		default:
			break;
		}
		EndPaint(hWnd, &pt);
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
