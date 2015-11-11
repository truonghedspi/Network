#ifndef _RESPOND_H_
#define _RESPOND_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef enum {
	LOGIN_RESPOND,
	LOGOUT_RESPOND,
	REGISTER_RESPOND,
	CHAT_WITH_USER_RESPOND,
	GET_ONLINE_USER_LIST_RESPOND
}RespondType;



typedef enum {
	LOGOUT_SUCCESS,
	LOGOUT_NOT_ONLINE //thang do chua online
}LogoutResult;

typedef enum {
	REGISTER_SUCCESS,
	REGISTER_INVALID_USERNAME,
	REGISTER_INVALID_PASSWORD,
	REGISTER_USER_EXISTED
}RegisterResult;

typedef enum {
	CHAT_SUCCESS,
	CHAT_USER_NOT_ONLINE,
	CHAT_INVALID_CONTENT
}ChatResult;

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

typedef struct _LogoutRespond {
	RespondType typeRespond;
	LogoutResult logoutResult;
	char messenger[196];
}LogoutRespond;

typedef struct _RegisterRespond {
	RespondType typeRespond;
	RegisterResult registerResult;
	char messenger[196];
}RegisterRespond;

typedef struct _GetOnlineUserListRespond {
	RespondType typeRespond;
	char listOnlineUser[10][20];
}GetOnlineUserListRespond;

typedef struct _ChatUserRespond {
	RespondType typeRespond;
	ChatResult chatResult;
	char messenger[196];
}ChatUserRespond;

#endif