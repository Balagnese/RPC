#undef UNICODE

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "../com/chat_rpc.h" // header file generated by the MIDL compiler

// TODO listen to server
#define RECEIVE_MODE "1"
#define SEND_MODE "2"
#define ERROR_STR "error"
#define MESSAGE_READ "read"
#define MESSAGE_UNREAD "unread"

void Usage(char *pszProgramName)
{
    fprintf(stderr, "Usage:  %s\n", pszProgramName);
    fprintf(stderr, " -p protocol_sequence\n");
    fprintf(stderr, " -n network_address\n");
    fprintf(stderr, " -e endpoint\n");
    fprintf(stderr, " -o options\n");
    fprintf(stderr, " -u user_name\n");
    fprintf(stderr, " -m mode: 1 - reeive, 2 - send\n");
    exit(1);
}

void main(int argc, char **argv)
{
    RPC_STATUS status;
    unsigned char *pszUuid = NULL;
    unsigned char *pszProtocolSequence = "ncacn_ip_tcp";
    unsigned char *pszNetworkAddress = "127.0.0.1";
    unsigned char *pszEndpoint = "33333";
    unsigned char *pszOptions = NULL;
    unsigned char *pszStringBinding = NULL;

    unsigned char *userName = "noname";
    unsigned char *mode = "1";

    /* Allow the user to override settings with command line switches */
    for (int i = 1; i < argc; i++)
    {
        if ((*argv[i] == '-') || (*argv[i] == '/'))
        {
            switch (tolower(*(argv[i] + 1)))
            {
            case 'p': // protocol sequence
                pszProtocolSequence = argv[++i];
                break;
            case 'n': // network address
                pszNetworkAddress = argv[++i];
                break;
            case 'e': // endpoint
                pszEndpoint = argv[++i];
                break;
            case 'o': // options
                pszOptions = argv[++i];
                break;
            case 'u': // user name
                userName = argv[++i];
                break;
            case 'm': // mode
                mode = argv[++i];
                break;
            case 'h':
            case '?':
            default:
                Usage(argv[0]);
            }
        }
        else
            Usage(argv[0]);
    }

    /* Use a convenience function to concatenate the elements of */
    /* the string binding into the proper sequence               */
    status = RpcStringBindingCompose(
        pszUuid,
        pszProtocolSequence,
        pszNetworkAddress,
        pszEndpoint,
        pszOptions,
        &pszStringBinding);

    printf("[INFO] RpcStringBindingCompose returned 0x%x\n", status);
    printf("[INFO] pszStringBinding = %s\n", pszStringBinding);
    if (status)
        exit(status);

    /* Set the binding handle that will */
    /* be used to bind to the server  */
    status = RpcBindingFromStringBinding(pszStringBinding, &chat_IfHandle);

    printf("[INFO] RpcBindingFromStringBinding returned 0x%x\n", status);
    if (status)
        exit(status);

    status = RpcBindingSetAuthInfo(
        chat_IfHandle,
        NULL,
        RPC_C_AUTHN_LEVEL_CONNECT,
        RPC_C_AUTHN_WINNT,
        NULL,
        0);

    printf("RpcBindingSetAuthInfo returned 0x%x\n", status);

    if (status)
        exit(status);

    /* Remote calls go here */
    int userId = Login_(userName);
    printf("\nSession started!\n");
    printf("Your ID is %d\n\n", userId);

    /* Main app loop */
    unsigned char cmd;
    int messageId;
    int loggedOut = 0;

    while (1)
    {
        printf("Enter command:\nu - user list\ns - send message\nr - receive message\nc - check message status\nq - quit\n");
        scanf("%c", &cmd);
        getchar(); // read extra newline char

        switch (cmd)
        {
        case 'u':
            Users_();
            break;
        case 's':
            // if (strcmp(mode, RECEIVE_MODE)) {
            //   printf("You need to use a different app mode. Restart your application with '-m 2'.");
            // } else if (strcmp(mode, SEND_MODE)) {
            //   printf("Not implemented. Live with it.");
            // }
            messageId = Send_(userId);
            printf("Message sent, ID: %d\n\n", messageId);
            break;
        case 'r':
            Receive_(userId);
            break;
        case 'c':
            printf("Enter message ID: ");
            scanf("%d", &messageId);
            getchar();
            Status_(userId, messageId);
            break;
        case 'q':
            Logout_(userId);
            loggedOut = 1;
            break;
        default:
            printf("Invalid command\n\n");
        }

        if (loggedOut)
            break;
    }

    printf("\nSession ended\n\n");

    /*  The calls to the remote procedures are complete. */
    /*  Free the string and the binding handle           */
    status = RpcStringFree(&pszStringBinding);
    printf("[INFO] RpcStringFree returned 0x%x\n", status);
    if (status)
        exit(status);

    status = RpcBindingFree(&chat_IfHandle);
    printf("[INFO] RpcBindingFree returned 0x%x\n", status);
    if (status)
        exit(status);

    exit(0);
}

void __RPC_FAR *__RPC_USER midl_user_allocate(size_t len)
{
    return (malloc(len));
}

void __RPC_USER midl_user_free(void __RPC_FAR *ptr)
{
    free(ptr);
}

int Login_(unsigned char *userName)
{
    unsigned long ulCode;
    int userId = 0;

    RpcTryExcept
    {
        printf("\n[DEBUG] Calling remote procedure Login(userName='%s')\n", userName);
        userId = Login(userName);
    }

    RpcExcept(1)
    {
        ulCode = RpcExceptionCode();
        printf("[ERROR] Runtime reported exception 0x%lx \n\n", ulCode);
    }

    RpcEndExcept

        return userId;
}

int Logout_(int userId)
{
    unsigned long ulCode;

    RpcTryExcept
    {
        printf("\n[DEBUG] Calling remote procedure Logout(userId=%d)\n", userId);
        Logout(userId);
    }

    RpcExcept(1)
    {
        ulCode = RpcExceptionCode();
        printf("[ERROR] Runtime reported exception 0x%lx\n\n", ulCode);
    }

    RpcEndExcept

        return 0;
}

int Users_()
{
    unsigned long ulCode;
    unsigned char usersList[USERS_COUNT][STR_LEN];

    RpcTryExcept
    {
        printf("\n[DEBUG] Calling remote procedure GetUsersList()\n");
        int userCount = GetUsersList(usersList);

        for (int i = 0; i < userCount; i++)
        {
            if (usersList[i] == NULL)
            {
                break;
            }
            printf("[%2d]: %s\n", i + 1, usersList[i]);
        }
    }

    RpcExcept(1)
    {
        ulCode = RpcExceptionCode();
        printf("[ERROR] Runtime reported exception 0x%lx\n\n", ulCode);
    }

    RpcEndExcept;

    return 0;
}

int Send_(int userId)
{
    unsigned long ulCode;
    char receiver[STR_LEN] = "";
    char text[STR_LEN] = "";
    int messageId;

    printf("To: ");
    fgets(receiver, STR_LEN, stdin);
    printf("Message text (limit - %d symbols):\n", STR_LEN);
    fgets(text, STR_LEN, stdin);

    RpcTryExcept
    {
        printf("\n[DEBUG] Calling remote procedure SendMyMessage(userId=%d, receiver=%s)\n", userId, receiver);
        messageId = SendMyMessage(userId, text, receiver);
    }

    RpcExcept(1)
    {
        ulCode = RpcExceptionCode();
        printf("[ERROR] Runtime reported exception 0x%lx\n\n", ulCode);
    }

    RpcEndExcept;

    return messageId;
}

int Status_(int userId, int messageId)
{
    unsigned long ulCode;
    int status = -1;

    RpcTryExcept
    {
        printf("\n[DEBUG] Calling remote procedure GetMessageStatus(userId=%d, messageId=%d)\n", userId, messageId);
        status = GetMessageStatus(userId, messageId);
    }

    RpcExcept(1)
    {
        ulCode = RpcExceptionCode();
        printf("[ERROR] Runtime reported exception 0x%lx\n\n", ulCode);
    }

    RpcEndExcept;

    switch (status)
    {
    case 0:
        printf("Message status: %s\n\n", MESSAGE_UNREAD);
        break;
    case 1:
        printf("Message status: %s\n\n", MESSAGE_READ);
        break;
    default:
        printf("Message status: %s\n\n", ERROR_STR);
    }

    return status;
}

int Receive_(int userId)
{
    unsigned long ulCode;
    unsigned char sender[STR_LEN];
    unsigned char message[STR_LEN];

    RpcTryExcept
    {
        printf("\n[DEBUG] Calling remote procedure ReceiveMyMessage(userId=%d)\n", userId);
        int found = ReceiveMyMessage(userId, sender, message);
        if (found)
        {
            printf("From: %s\n%s\n\n", sender, message);
        }
        else
        {
            printf("There are no new messages\n\n");
        }
    }

    RpcExcept(1)
    {
        ulCode = RpcExceptionCode();
        printf("[ERROR] Runtime reported exception 0x%lx\n\n", ulCode);
    }

    RpcEndExcept;

    return 0;
}
