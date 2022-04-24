#undef UNICODE

#include <stdio.h>
#include <stdbool.h>
#include "../com/chat_rpc.h"

struct users {
	unsigned char *userName;
	int clientId;
	bool isOnline;
} userList[USERS_COUNT];

struct messages {
	int messageId;
	int senderId;
	unsigned char *text;
	unsigned char *receiverName;
	bool isMessageReceived;
} messageList[MESSAGES_COUNT];

static int userListIndex = -1;
static int messageListIndex = -1;

// ToDo - поправить глобально кодировку записи символов

int Login(unsigned char *userName)
{
	if (strlen(userName) <= STR_LEN) {
		userListIndex++;
		userList[userListIndex].userName = userName;
		userList[userListIndex].clientId = userListIndex; // FYI: clientId совпадет с индексом
		userList[userListIndex].isOnline = true;
		return userList[userListIndex].clientId;
	}
	return 0;
	// ToDo - Валидация если больше 50 символов на стороне клиента?
}

void Logout(int clientId)
{
	userList[clientId].isOnline = false;
}

int GetUsersList(unsigned char *usersList[USERS_COUNT])
{
	for (int i = 0; i < USERS_COUNT; i++) {
		usersList[i] = userList[i].userName;
	}
	return 0; // ToDo - может void раз мы список заполняем?
}

int SendMyMessage(int clientId, unsigned char *message, unsigned char *receiver)
{
	if (strlen(message) <= STR_LEN) {
		messageListIndex++;
		messageList[messageListIndex].messageId = messageListIndex; // FYI: messageId совпадет с индексом
		messageList[messageListIndex].senderId = clientId;
		messageList[messageListIndex].text = message;
		messageList[messageListIndex].receiverName = receiver;
		messageList[messageListIndex].isMessageReceived = false;
		return messageList[messageListIndex].messageId;
	}
	return -1;
	// ToDo - валидация на длину сообщения и несуществующего получателя на стороне клиента?
}

int GetMessageStatus(int clientId, int messageId) // похоже clientId тут и не нужен: нам есть разница какой пользователь его должен получить? (помоему важен только messageId)
{
	return messageList[messageId].isMessageReceived;
	// ToDo - статус на клиенте отображается в виде цифры (может текстом надо?)
}

int ReceiveMyMessage(int clientId, unsigned char *sender, unsigned char *message)
{
	unsigned char *clientUserName = userList[clientId].userName;
	for (int i = 0; i < MESSAGES_COUNT; i++) {
		if (messageList[i].receiverName == clientUserName) {
			for (int j = 0; j < USERS_COUNT; j++) {
				if (userList[j].clientId == messageList[i].senderId) {
					sender = userList[j].userName;
					break;
				}
			}
			message = messageList[i].text;
		}
	}
	return 0; // ToDo - может void раз мы переменные, подающиеся на вход, заполняем? 
}