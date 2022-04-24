#undef UNICODE

#include <stdio.h>
#include <stdbool.h>
#include "../com/chat_rpc.h"

struct users
{
	unsigned char userName[STR_LEN];
	int userId;
	bool isOnline;
} users[USERS_COUNT];

struct messages
{
	int messageId;
	int senderId;
	unsigned char text[STR_LEN];
	unsigned char receiverName[STR_LEN];
	bool isMessageReceived;
} messagesList[MESSAGES_COUNT];

static int nextUserIndex = 0;
static int nextMessageIndex = 0;

static int _FindUser(unsigned char *userName)
{
	for (int i = 0; i < nextUserIndex; i++)
	{
		if (!strcmp(users[i].userName, userName))
			return users[i].userId;
	}
	return -1;
}

int Login(unsigned char *userName)
{
	int userId = _FindUser(userName);

	if (userId >= 0)
	{
		return userId;
	}
	else if (strlen(userName) <= STR_LEN)
	{
		strcpy(users[nextUserIndex].userName, userName);
		users[nextUserIndex].userId = nextUserIndex; // FYI: userId совпадет с индексом
		users[nextUserIndex].isOnline = true;
		printf("[DEBUG] User Login [%d] %s\n", users[nextUserIndex].userId, users[nextUserIndex].userName);
		return users[nextUserIndex++].userId;
	}
	else
	{
		return -1;
	}
}

void Logout(int userId)
{
	users[userId].isOnline = false;
}

int GetUsersList(unsigned char usersList[USERS_COUNT][STR_LEN])
{
	for (int i = 0; i < nextUserIndex; i++)
	{
		strcpy(usersList[i], users[i].userName);
		printf("%d", users[i].userId);
	}
	return nextUserIndex;
}

int SendMyMessage(int userId, unsigned char *message, unsigned char *receiver)
{
	if (strlen(message) <= STR_LEN)
	{
		messagesList[nextMessageIndex].messageId = nextMessageIndex++; // FYI: messageId совпадет с индексом
		messagesList[nextMessageIndex].senderId = userId;
		strcpy(messagesList[nextMessageIndex].text, message);
		strcpy(messagesList[nextMessageIndex].receiverName, receiver);
		messagesList[nextMessageIndex].isMessageReceived = false;
		return messagesList[nextMessageIndex].messageId;
	}
	return -1;
}

int GetMessageStatus(int userId, int messageId)
{
	return messagesList[messageId].isMessageReceived;
}

int ReceiveMyMessage(int userId, unsigned char *sender, unsigned char *message)
{
	int found = 0;
	unsigned char *clientUserName = users[userId].userName;
	for (int i = 0; i < MESSAGES_COUNT; i++)
	{
		printf("compare %s, %s", messagesList[i].receiverName, clientUserName);
		if (!strcmp(messagesList[i].receiverName, clientUserName))
		{
			for (int j = 0; j < USERS_COUNT; j++)
			{
				if (users[j].userId == messagesList[i].senderId)
				{
					strcpy(sender, users[j].userName);
					break;
				}
			}
			strcpy(message, messagesList[i].text);
			messagesList[i].isMessageReceived = true;
			found = 1;
		}
	}
	return found;
}