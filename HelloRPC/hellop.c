#undef UNICODE

#include <stdio.h>  
#include "hello.h"


void HelloProc(unsigned char * pszString)
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

