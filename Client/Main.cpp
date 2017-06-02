#include "ClientComm.h"

TCHAR *szProgName = TEXT("Base");
HWND initialMenu[INITIAL_MENU_SIZE];
HWND mainMenu[5];
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {
	
	HWND hWnd;
	MSG lpMsg;		
	WNDCLASSEX wcApp;	
	BOOL ret;	
	
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


void showSingleElement(HWND hWnd, UINT dim)
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

void SetupInitialMenu(HWND hWnd) {
	initialMenu[0] = CreateWindow(TEXT("STATIC"), TEXT("Servers - Online"), WS_CHILD, 450, 150, 150, 25, hWnd, NULL, NULL, NULL);
	initialMenu[1] = CreateWindow(TEXT("EDIT"), TEXT(""), WS_CHILD | WS_BORDER, 450, 175, 150, 25, hWnd, NULL, NULL, NULL);
	initialMenu[2] = CreateWindow(TEXT("BUTTON"), TEXT("Entrar"), WS_CHILD, 450, 200, 150, 25, hWnd, (HMENU) LOGIN_BUTTON, NULL, NULL);
}

LRESULT CALLBACK MainEvent(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam) {
	

	switch (messg) {
	case WM_CREATE:
		SetupInitialMenu(hWnd);
		showMultipleElement(initialMenu, 3);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case LOGIN_BUTTON:
			GetWindowText(initialMenu[1], username, GetWindowTextLength(initialMenu[1] + 1));
			break;
		}
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
