#ifndef _RESPOND_H_
#define _RESPOND_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "request.h"

typedef enum {
	LOGIN_RESPOND,
	LOGOUT_RESPOND,
	REGISTER_RESPOND,
	CHAT_RESPOND,
	GET_LIST_USER_ONLINE_RESPOND
}RespondType;

typedef struct _Respond{
	RespondType typeRespond;
	char data[200];
}Respond;

typedef enum {
	LOGIN_SUCCESS,
	LOGIN_INVALID_USERNAME,
	LOGIN_INVALID_PASSWORD,
	LOGIN_ONLINE_EXISTED //da co thang online tai khoan do
}LoginResult;

typedef struct _LoginRespond {
	RespondType typeRespond;
	LoginResult loginResult;
	char messenger[196];
}LoginRespond;

typedef enum {
	LOGOUT_SUCCESS,
	LOGOUT_NOT_ONLINE //thang do chua online
}LogoutResult;

typedef struct _LogoutRespond {
	RespondType typeRespond;
	LogoutResult logoutResult;
	char messenger[196];
}LogoutRespond;

typedef enum {
	REGISTER_SUCCESS,
	REGISTER_INVALID_USERNAME,
	REGISTER_INVALID_PASSWORD,
	REGISTER_USER_EXISTED
}RegisterResult;

typedef struct _RegisterRespond {
	RespondType typeRespond;
	RegisterResult registerResult;
	char messenger[196];
}RegisterRespond;
typedef enum {
	LIST_EMPTY,
	LIST_NON_EMPTY
}ListResult;
typedef struct _GetOnlineUserListRespond {
	RespondType typeRespond;
	ListResult listResult;
	char listOnlineUser[10][20];
}GetOnlineUserListRespond;

/*typedef enum {
	CHAT,QUIT_CHAT,
	ASK_CHAT,YES,NO
}ChatTypeRespond;
*/
typedef struct _ChatRespond {
	RespondType typeRespond;
	ChatType chatType;
	char userName[50];
	char messenger[146];
}ChatRespond;

#endif