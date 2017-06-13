#pragma once

int writeClientResponse(HANDLE hPipe, Message answer);
TCHAR* getClientName(LPVOID param);
void sendMsgLogger(LPVOID param, int code);
BOOL usernameExists(TCHAR* name);
void login(LPVOID param, TCHAR* name, Message answer);
void cleanClientHandles(LPVOID param);
int writeServerBroadcast(Message answer);
void disconnectClient(LPVOID param, Message answer);
void initGame(Message answer);