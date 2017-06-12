// GameServer.cpp : Defines the entry point for the console application.
//
#define _WIN32_WINNT 0x0500
#include "Comms.h"


BOOL CreateMyDACL(SECURITY_ATTRIBUTES * pSA)
{
	// Define the SDDL for the DACL. This example sets 
	// the following access:
	//     Built-in guests are denied all access.
	//     Anonymous logon is denied all access.
	//     Authenticated users are allowed 
	//     read/write/execute access.
	//     Administrators are allowed full control.
	// Modify these values as needed to generate the proper
	// DACL for your application. 
	TCHAR * szSD = TEXT("D:")       // Discretionary ACL
		TEXT("(D;OICI;GA;;;BG)")     // Deny access to 
									 // built-in guests
		TEXT("(D;OICI;GA;;;AN)")     // Deny access to 
									 // anonymous logon
		TEXT("(A;OICI;GRGWGX;;;AU)") // Allow 
									 // read/write/execute 
									 // to authenticated 
									 // users
		TEXT("(A;OICI;GA;;;BA)");    // Allow full control 
									 // to administrators

	if (NULL == pSA)
		return FALSE;

	return ConvertStringSecurityDescriptorToSecurityDescriptor(
		szSD,
		SDDL_REVISION_1,
		&(pSA->lpSecurityDescriptor),
		NULL);
}

int _tmain(int argc, TCHAR *argv[])
{
	BOOL fConnected = false, isAccepting = true;
	DWORD dwThreadId = 0;
	SECURITY_ATTRIBUTES sa, *pSA;
	TCHAR * szSD = TEXT("D:")       // Discretionary ACL
		TEXT("(D;OICI;GA;;;BG)")     // Deny access to 
									 // built-in guests
		TEXT("(D;OICI;GA;;;AN)")     // Deny access to 
									 // anonymous logon
		TEXT("(A;OICI;GRGWGX;;;AU)") // Allow 
									 // read/write/execute 
									 // to authenticated 
									 // users
		TEXT("(A;OICI;GA;;;BA)");    // Allow full control 
									 // to administrators
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = FALSE;

	if (!CreateMyDACL(&sa)) {
		// Error encountered; generate message and exit.
		printf("Failed CreateMyDACL\n");
		exit(1);
	}

	hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, TEXT("Single Instance Mutex"));
	if (hMutex == NULL)
		hMutex = CreateMutex(NULL, FALSE, TEXT("Single Instance Mutex"));
	else
	{
		_tprintf(TEXT("There is already a server running!\n\n"));
		system("pause");
		return 1;
	}

	canWrite = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (canWrite == NULL) {
		_tprintf(TEXT("Erro ao criar evento de writeready\n"));
		return 1;
	}
	while (isAccepting) {
		hPipe = CreateNamedPipe(pipename, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, BUFSIZE, BUFSIZE, 5000, &sa);
		if (hPipe == INVALID_HANDLE_VALUE)
		{
			_tprintf(TEXT("\nNamed pipe creation failed! Error: %d"), GetLastError());
			return 1;
		}
		//verifica se alguem connectou
		fConnected = ConnectNamedPipe(hPipe, NULL) ?
			TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
		if (fConnected)
		{
			printf("----> Client connected, creating a processing thread.\n");

			// Create a thread for this client. 
			hThread = CreateThread(
				NULL,              // no security attribute 
				0,                 // default stack size 
				(LPTHREAD_START_ROUTINE)InstanceThread,    // thread proc
				(LPVOID)hPipe,    // thread parameter 
				0,                 // not suspended 
				&dwThreadId);      // returns thread ID 

			if (hThread == NULL)
			{
				_tprintf(TEXT("CreateThread failed, GLE=%d.\n"), GetLastError());
				return 1;
			}
			else CloseHandle(hThread);
		}
		else
			// The client could not connect, so close the pipe. 
			CloseHandle(hPipe);
	}
	return 1;
	
}

