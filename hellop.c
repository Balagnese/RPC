#undef UNICODE

#include <stdio.h>  
#include "hello.h"


void HelloProc(unsigned char* pszString)
{
	printf("%s\n", pszString);
}

void Shutdown(void)
{
	RPC_STATUS status;

	status = RpcMgmtStopServerListening(NULL);

	printf("RpcMgmtStopServerListening returned 0x%x\n", status);

	if (status)
		exit(status);

	/*status = RPCServerUnregisterIf(NULL, NULL, FALSE);

	printf("RPCServerUnregisterIf returned 0x%x\n", status);

	if (status)
		exit(status);*/
}

int Login(unsigned char* userName)
{
	return 0;
}

void Logout(int id) {

}

int GetUsersList(unsigned char usersList[100][50])
{
	return 0;
}

int SendMyMessage(int id, unsigned char* message, unsigned char* receiver)
{
	return 0;
}

int GetMessageStatus(int id, int messageId) {
	return 0;
}

int ReceiveMyMessage(unsigned char usersList[100], unsigned char sender[100])
{
	return 0;
}