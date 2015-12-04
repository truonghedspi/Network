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
	GET_ONLINE_USER_LIST_REQUEST,
	ROOM_REQUEST,
	GET_ROOM_LIST_REQUEST,
	BLOCK_REQUEST
	GET_BLOCK_LIST_REQUEST
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
	CHAT_FRIEND_SEND,
	CHAT_LOG_REQUEST
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
	CHAT_ROOM_REQUEST
}RoomType;

typedef struct _RoomRequest {
	RequestType typeRequest;
	RoomType roomType;
	char roomName[16];
	char messenger[180];
}RoomRequest;

typedef struct _GetRoomListRequest {
	RequestType typeRequest;
	char data[200];
}GetRoomListRequest;


//--------BLock Use-----------
typedef enum{
	BLOCK,
	UNBLOCK
}BlockType;

typedef struct _BlockUserRequest {
	RequestType  typeRequest;
	BlockType blockType;
	char blockUserName[50];
	char data[146];
}BlockUserRequest;

typedef struct _GetBlockListRequest {
	RequestType typeRequest;
	char data[200];
}GetBlockListRequest;

#endif