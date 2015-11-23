#ifndef _REQUEST_H_
#define _REQUEST_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef enum {
	LOGIN_REQUEST,
	LOGOUT_REQUEST,
	REGISTER_REQUEST,
	CHAT_REQUEST,
	CHAT_ROOM_REQUEST,
	GET_ONLINE_USER_LIST_REQUEST
}RequestType;

typedef struct _Request {
	RequestType typeRequest;
	char data[200];
}Request;

typedef struct _LoginRequest {
	RequestType typeRequest;
	char userName[100];
	char password[100];
}LoginRequest;

typedef struct _LogoutRequest {
	RequestType typeRequest;
	char userName[100];
	char password[100];
}LogoutRequest;

typedef struct _RegisterRequest {
	RequestType typeRequest;
	char userName[100];
	char password[50];
	char passwordConfirm[50];
}RegisterRequest;

typedef struct _GetOnlineUserListRequest {
	RequestType typeRequest;
	char data[200];
}GetOnlineUserListRequest;

typedef enum{
	QUIT_CHAT,
	CHAT,
	CHAT_ROOM,
	INVITE_ROOM,
	IN_ROOM,
	OUT_ROOM
}ChatType;

typedef struct _ChatRequest {
	RequestType typeRequest;
	ChatType chatType;
	char userName[50];
	char messenger[146];
}ChatRequest;


#endif