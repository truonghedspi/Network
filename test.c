#include <stdio.h>
#include "respond.h"
#include "request.h"
#define LEN 204 

typedef enum{
	USER,
	ROOM
}TypeChatList;

typedef struct{
	char userName[LEN];
	int type;
}User_List;

int check_input(char buff[]){
	if(strlen(buff)<6 || strlen(buff)>=10) return 0;
	else return 1;
}
int check_input2(char buff[]){
	if(strlen(buff)>0 && strlen(buff)<=10) return 1;
	else if(strlen(buff) == 0){
		printf("\nKhong duoc de trong!!");
		return 0;
	}else if(strlen(buff) >10){
		printf("\nNoi dung chat qua dai!!");
		return 0;
	}
}

int add_partner(char userName[],User_List chatList[],TypeChatList type){
	int i=0;
	while(strcmp(chatList[i].userName,"") != 0 ){
		i++;
	}
	strcpy(chatList[i].userName,userName);
	chatList[i].type=type;
	return i+1;
}

int del_partner(char userName[],User_List chatList[]){
	int i=0;
	while(1){
		if(strcmp(userName,chatList[i].userName)==0)
			break;
		i++;
	}
	while(1){
		if(strcmp(chatList[i+1].userName,"")==0)
			break;
		strcpy(chatList[i].userName,chatList[i+1].userName);
		chatList[i].type = chatList[i+1].type;
		i++;
	}
	strcpy(chatList[i].userName,"");
	return i;
}

int take_user_list(GetOnlineUserListRespond userListRespond,User_List userList[]){
	int i=0;

	while(strcmp(userListRespond.onlineUserList[i],"") != 0){
		strcpy(userList[i].userName,userListRespond.onlineUserList[i]);
		i++;
	}
	return i;
}

int chat(char userName[],char buff[],int type){
	ChatRequest chatRequest,chatRequest2;
	char mesg[LEN];

	chatRequest.typeRequest=CHAT_REQUEST;
	chatRequest.chatType=type;
	strcpy(chatRequest.userName,userName);
	strcpy(chatRequest.messenger,buff);
	memcpy(mesg,&chatRequest,204);
	chatRequest2=(*(ChatRequest*)mesg);
	printf("%d\n",chatRequest2.typeRequest );
	printf("%d\n",chatRequest2.chatType );
	printf("%s\n", chatRequest2.userName);
	printf("%s\n", chatRequest2.messenger);

}

int main() {
	GetOnlineUserListRespond listRespond,listRespond1;
	char buff[204],buff1[204];
	char onlineUserList[10][20];
	User_List userList[100];
	int i=0,y,a;
	User_List chatList[4];
	strcpy(chatList[0].userName,"tranha1");
	chatList[0].type=USER;
	strcpy(chatList[1].userName,"TRANHA");
	chatList[1].type=ROOM;
	strcpy(chatList[2].userName,"tranha2");
	chatList[2].type=USER;
	strcpy(chatList[3].userName,"tranha3");
	chatList[3].type=USER;

	a = del_partner("TRANHA",chatList);
	for(i=0;i<a;i++){
		printf("%s\n",chatList[i].userName );
		printf("%d\n",chatList[i].type );
	}
	a=add_partner("TRANHA2",chatList,ROOM);
	for(i=0;i<a;i++){
		printf("%s\n",chatList[i].userName );
		printf("%d\n",chatList[i].type );
	}
	return 0;
}