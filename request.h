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
	GET_ONLINE_USER_LIST_REQUEST,
	GET_ROOM_LIST_REQUEST
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
	char data[200];
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

typedef enum {
	CHAT_FRIEND_SEND
}ChatType;

typedef struct _ChatRequest {
	RequestType typeRequest;
	ChatType chatType;
	char userNameReceiver[50];
	char messenger[146];
}ChatRequest;

//------------ROOM REQUEST---------------

typedef enum {
	JOIN_ROOM,
	OUT_ROOM,
	CHAT_ROOM
}ChatRoomType;

typedef struct _ChatRoomRequest {
	RequestType typeRequest;
	ChatRoomType chatRoomType;
	char roomName[16];
	char messenger[180];
}ChatRoomRequest;

typedef struct _GetRoomListRequest {
	RequestType typeRequest;
	char data[200];
}GetRoomListRequest;



#endif