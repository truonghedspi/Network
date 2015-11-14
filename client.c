#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <arpa/inet.h>
#include <errno.h>
#include <poll.h>
#include <sys/select.h>
#include "request.h"
#include "respond.h"
#define LEN 204



int currentSockFD = -1;

typedef enum{
	USER,
	ROOM
}TypeChatList;

typedef struct{
	char userName[20];
	TypeChatList type;
}User_List;



int check_input(char buff[]){
	if(strlen(buff)<6 || strlen(buff)>=15) return 0;
	else return 1;
}

int check_input2(char buff[]){
	if(strlen(buff)>0 && strlen(buff)<=146) return 1;
	else if(strlen(buff) == 0){
		printf("\nKhong duoc de trong!!");
		return 0;
	}else if(strlen(buff) >146){
		printf("\nNoi dung chat qua dai!!");
		return 0;
	}
}

int check_type(Respond respond){
	return respond.typeRespond;
}

void type_chat_respond(char buff[],User_List chatList[]){

	ChatRespond chatRespond;

	chatRespond=(*(ChatRespond*)buff);
	switch(chatRespond.chatType){
		case ASK_CHAT:
			answer_ask_chat(chatRespond,chatList);
			break;
		case QUIT_CHAT:
			del_partner(chatRespond.userName,chatList);
			break;
		case CHAT:
			printf("\n@%s: %s",chatRespond.userName,chatRespond.messenger);
			break;
		case YES:
			printf("\n@%s dong y chat!!",chatRespond.userName);
			add_partner(chatRespond.userName,chatList,USER);
			break;
		case NO:
			printf("\n@%s khong dong y chat!!",chatRespond.userName);
			break;
		
	}
}

void type_chat_room_respond(char buff[],User_List chatList[]){
	ChatRoomRespond chatRoomRespond;

	chatRoomRespond=(*(ChatRoomRespond*)buff);
	switch(chatRoomRespond.chatType){
		case CHAT_ROOM:
			printf("\n#%s_@%s: %s",chatRoomRespond.roomName,chatRoomRespond.userName,chatRoomRespond.messenger);
			break;
		case INVITE_ROOM:
			//answer_invite_room
			break;
		case IN_ROOM:
			break;
		case OUT_ROOM:
			break;
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

int login(){
	char mesg[LEN];
	char tk[100];
	char pass[100];
	LoginRequest user;
	Respond respond;
	LoginRespond loginRespond;
	int t;

	while(1){
		printf("\nNhap ten tai khoan: ");
		strcpy(tk,"");
		fgets(tk,LEN,stdin);
		tk[strlen(tk)-1]='\0';
		if(check_input(tk) == 0){
			printf("\nMoi nhap lai!!");
		}else break;
	}
	strcpy(user.userName,tk);
	while(1){
		printf("Nhap mat khau: ");
		strcpy(pass,"");
		fgets(pass,LEN,stdin);
		pass[strlen(pass)-1]='\0';
		if(check_input(pass) == 0){
			printf("\nMoi nhap lai!!\n");
		}else break;
	}
	strcpy(user.password,pass);
	user.typeRequest=LOGIN_REQUEST;
	memcpy(mesg,&user,204);
	send(currentSockFD, mesg,204, 0);
	strcpy(mesg,"");
	recv(currentSockFD, mesg, LEN, 0);
	respond=(*(Respond*)mesg);
	
	if(check_type(respond) == LOGIN_RESPOND){
		loginRespond=(*(LoginRespond*)mesg);
		if(loginRespond.loginResult == LOGIN_SUCCESS){
			printf("\n%s",loginRespond.messenger);
			return 1;
		}else if(loginRespond.loginResult == LOGIN_INVALID_USERNAME){
			printf("\n%s",loginRespond.messenger);
			return 0;
		}else if(loginRespond.loginResult == LOGIN_INVALID_PASSWORD){
			printf("\n%s",loginRespond.messenger);
			return 0;
		}else if(loginRespond.loginResult == LOGIN_ONLINING){
			printf("\n%s",loginRespond.messenger);
			return 0;
		}
	} 	
}

int sign_up(){
	char mesg[LEN];
	char tk[20];
	char pass[20];
	char comf_pass[20];
	int t;
	RegisterRequest user;
	Respond respond;
	RegisterRespond registerRespond;

	while(1){
		printf("\nNhap ten tai khoan: ");
		strcpy(tk,"");
		fgets(tk,LEN,stdin);
		tk[strlen(tk)-1]='\0';
		if(check_input(tk) == 0){
			printf("\nMoi nhap lai!!");
		}else break;
	}
	strcpy(user.userName,tk);
	while(1){
		printf("Nhap mat khau: ");
		strcpy(pass,"");
		fgets(pass,LEN,stdin);
		pass[strlen(pass)-1]='\0';
		if(check_input(pass) == 0){
			printf("\nMoi nhap lai!!\n");
		}else break;
	}
	while(1){
		printf("Xac nhan mat khau: ");
		strcpy(comf_pass,"");
		fgets(comf_pass,LEN,stdin);
		comf_pass[strlen(comf_pass)-1]='\0';
		if(strcmp(pass,comf_pass)==0) break;
	}
	strcpy(user.password,pass);
	strcpy(user.passwordConfirm,comf_pass);
	user.typeRequest=REGISTER_REQUEST;
	memcpy(mesg,&user,LEN);
	send(currentSockFD, mesg,204, 0);
	strcpy(mesg,"");
	recv(currentSockFD, mesg, LEN, 0);
	respond=(*(Respond*)mesg);
	if(respond.typeRespond == REGISTER_RESPOND){
		registerRespond=(*(RegisterRespond*)mesg);
		if(registerRespond.registerResult == REGISTER_SUCCESS){
			printf("\n%s",registerRespond.messenger);
			return 1;
		}else if(registerRespond.registerResult == REGISTER_INVALID_USERNAME){
			printf("\n%s",registerRespond.messenger);
			return 0;
		}else if(registerRespond.registerResult == REGISTER_INVALID_PASSWORD){
			printf("\n%s",registerRespond.messenger);
			return 0;
		}else if(registerRespond.registerResult == REGISTER_USER_EXISTED){
			printf("\n%s",registerRespond.messenger);
			return 0;
		}
	}
}

int answer_invite_room(){

}
int answer_ask_chat(ChatRespond chatRespond,User_List chatList[]){
	int i;
	char mesg[LEN];
	char yes[2];
	ChatRequest chatRequest;
	
	printf("\n@%s muon chat voi ban!!",chatRespond.userName);
	printf("\nBan co dong y(y/n): ");
	fgets(yes,3,stdin);
	if(yes[0] == 'y' || yes[0] == 'Y') {
		add_partner(chatRespond.userName,chatList,USER);
		chatRequest.typeRequest=CHAT_REQUEST;
		chatRequest.chatType=YES;
		strcpy(chatRequest.userName,chatRespond.userName);
		memcpy(mesg,&chatRequest,204);
		send(currentSockFD,mesg,204,0);
	}else {
		chatRequest.typeRequest=CHAT_REQUEST;
		chatRequest.chatType=NO;
		strcpy(chatRequest.userName,chatRespond.userName);
		memcpy(mesg,&chatRequest,204);
		send(currentSockFD,mesg,204,0);
	}
}

int take_user_list(GetOnlineUserListRespond userListRespond,User_List userList[]){
	int i=0;

	while(strcmp(userListRespond.onlineUserList[i],"") != 0){
		strcpy(userList[i].userName,userListRespond.onlineUserList[i]);
		i++;
	}
	return i;
}

void online_user_list_request(){
	GetOnlineUserListRequest	listRequest;
	char mesg[LEN];

	listRequest.typeRequest=GET_ONLINE_USER_LIST_REQUEST;
	memcpy(mesg,&listRequest,204);
	send(currentSockFD,mesg,204,0);
	return;
}

int choose_user(char mesg[],User_List chatList[]){
	int i,t,y,TOTAL_USER;
	User_List userList[100];
	char buff[LEN],buff3[LEN];
	GetOnlineUserListRespond 	userListRespond;
	ChatRequest chatRequest;


	userListRespond=(*(GetOnlineUserListRespond*)mesg);
	if(userListRespond.listResult == LIST_EMPTY){
		printf("\nKhong co user nao online!!");
		return 0;
	}else if(userListRespond.listResult == LIST_NON_EMPTY){
		TOTAL_USER=take_user_list(userListRespond,userList);
		for ( i = 0; i < TOTAL_USER; i++){
			printf("\n%d.@%s",i+1,userList[i].userName);
		}
		while(1){
			printf("\nChon user de chat: ");
			scanf("%d",&y);
			fgets(buff3,LEN,stdin);
			if(0<y&&y<=TOTAL_USER) break;
		}
		chatRequest.typeRequest=CHAT_REQUEST;
		chatRequest.chatType=ASK_CHAT;
		strcpy(chatRequest.userName,userList[y-1].userName);
		memcpy(mesg,&chatRequest,204);
		send(currentSockFD,mesg,204,0);
	}	
}

int send_chat(char userName[],char buff[],ChatType type){
	ChatRequest chatRequest;
	char mesg[LEN];

	chatRequest.typeRequest=CHAT_REQUEST;
	chatRequest.chatType=type;
	strcpy(chatRequest.userName,userName);
	strcpy(chatRequest.messenger,buff);
	memcpy(mesg,&chatRequest,204);
	send(currentSockFD,mesg,204,0);
}

/*int chatting(User_List chatList[]){
	int TOTAL_USER=0,i,y;
	char buff[LEN],mesg[LEN];
	Respond respond;
	ChatRequest chatRequest;
	fd_set readSet;
	struct timeval tv;


	while(strcmp(chatList[TOTAL_USER].userName,"") != 0){
		TOTAL_USER++;
	}

	while(1){
		tv.tv_sec = 0;
		tv.tv_usec =10000;
		FD_SET(currentSockFD, &readSet);
		FD_SET(fileno(stdin), &readSet);
		select(currentSockFD +1, &readSet, NULL, NULL, &tv);

		if(TOTAL_USER > 0){
			if (FD_ISSET(fileno(stdin), &readSet)){
				while(1){
							fgets(buff,LEN,stdin);
							buff[strlen(buff)-1]='\0';
							if(check_input2(buff) == 1) break;
				}
				if(TOTAL_USER == 1){
					if(chatList[0].type == USER){
						if(strcmp(buff,"\\m") == 0){
							return 1;
						}else if(strlen(buff)==1 && ((buff[0]=='q') || (buff[0]=='Q'))) {
							send_chat(chatList[0].userName,"",QUIT_CHAT);
							TOTAL_USER = del_partner(chatList[0].userName,chatList);
						}else{
							send_chat(chatList[0].userName,buff,CHAT);
						}
					}else if(chatList[0].type == ROOM){
						if(strlen(buff)==1 && ((buff[0]=='q') || (buff[0]=='Q'))) {
							send_chat(chatList[0].userName,"",OUT_ROOM);
							TOTAL_USER = del_partner(chatList[0].userName,chatList);
						}else{
							send_chat(chatList[0].userName,buff,CHAT_ROOM);
						}
					}
				}else if(TOTAL_USER > 1){
					for ( i = 0; i < TOTAL_USER; i++){
						if(chatList[i].type==1){
							printf("\n%d.#%s",i+1,chatList[i].nich_name);
						}else{
							printf("\n%d.@%s",i+1,chatLlist[i].nich_name);
						}
					}
					while(1){
						printf("\nChon user de tra loi: ");
						scanf("%d",&y);
						fgets(buff3,LEN,stdin);
						if(0<y&&y<=TOTAL_USER) break;
					}
					if(chatList[y].type == USER){
						if((strcmp(buff,"\\m") == 0){
							return 1;
						}else if(strlen(buff)==1 && ((buff[0]=='q') || (buff[0]=='Q'))) {
							send_chat(chatList[y].userName,"",QUIT_CHAT);
							TOTAL_USER = del_partner(chatList[y].userName,chatList);
						}else{
							send_chat(chatList[y].userName,buff,CHAT);
						}
					}else if(chatList[y].type == ROOM){
						if(strlen(buff)==1 && ((buff[0]=='q') || (buff[0]=='Q'))) {
							send_chat(chatList[y].userName,"",OUT_ROOM);
							TOTAL_USER = del_partner(chatList[y].userName,chatList);
						}else{
							send_chat(chatList[y].userName,buff,CHAT_ROOM);
						}
					}

				} 
			}
			if (FD_ISSET(currentSockFD, &readSet)){
				strcpy(mesg,"");
				recv(currentSockFD,mesg,LEN,0);
				respond=(*(Respond*)mesg);
				if(check_type(respond) == CHAT_ROOM_RESPOND){
					type_chat_room_respond(mesg,chatList);
				}else if(check_type(respond) == CHAT_RESPOND) {
					type_chat_respond(mesg,chatList);
					//chat
				}else if (check_type(respond) == GET_ONLINE_USER_LIST_RESPOND){
					choose_user(mesg,chatList);
					//chat
				}

			}
		}else {
			printf("\nChat list rong");
			return 0;
		}

	}
}*/
void menu(){

	char choose[2];
	int max_user_list;
	int rv,t,x;
	char mesg[LEN];
	User_List userList[100];
	User_List chatList[10];
	fd_set readSet;
	struct timeval tv;
	Respond respond;
	ChatRespond chatRespond;

	while(1){
		printf("\n***DOI 1 USER KHAC HOAC CHON CAC MUC DUOI DAY***\n");
		printf("\n1.Chon 1 user dang online de chat");
		printf("\n2.Chon 1 phong de chat");
		printf("\n3.Thoat");
		printf("\nBan chon: ");
		printf("\n");
		while(1){
			tv.tv_sec = 5;
			tv.tv_usec =0;
			FD_SET(currentSockFD, &readSet);
			FD_SET(fileno(stdin), &readSet);
			select(currentSockFD +1, &readSet, NULL, NULL, &tv);
	
			if(FD_ISSET(currentSockFD, &readSet)){
				strcpy(mesg,"");
				recv(currentSockFD,mesg,LEN,0);
				respond=(*(Respond*)mesg);
				if(check_type(respond) == CHAT_ROOM_RESPOND){
					type_chat_room_respond(mesg,chatList);
				}else if(check_type(respond) == CHAT_RESPOND) {
					type_chat_respond(mesg,chatList);
					//chat
				}else if (check_type(respond) == GET_ONLINE_USER_LIST_RESPOND){
					choose_user(mesg,chatList);
					//chat
				}
			}else if (FD_ISSET(fileno(stdin), &readSet)){
				fgets(choose,3,stdin);
				switch(choose[0]){
		    		case '1' :
		    			online_user_list_request();
		    			break;
		    		case '2' :
		    			//choose_room(sockfd,chatList);
		    			//chat
		    			break;
		    		case '3' :
		    			//log_out(sockfd);
		    			return ;
		    		default :
		    			break;
		    	}
			} 
		}
	}

}

void main(){
	int sockfd;
	struct sockaddr_in serv_addr;
	char choose[2];
	int t;

    serv_addr.sin_family = AF_INET;       
    serv_addr.sin_port = htons(5500);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
    size_t size =100;

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    	printf("\nLoi tao socket!!");
    }
    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\nConnect loi!!\n");
       return ;
    }else
    while(1){
    	currentSockFD = sockfd;
    	printf("\n*****MENU*****\n");
    	printf("\n1.Dang nhap\n2.Dang ki\n3.Thoat" );
    	printf("\nBan chon: ");
    	fgets(choose,3,stdin);
    	switch(choose[0]){

    		case '1': 
    				printf("\nBan chon Dang Nhap");
    				t = login();
    			    //if(t == 1) menu(sockfd);
    			    
    			    break;
    		case '2': 	
    				printf("\nBan chon Dang Ki");
    				t = sign_up();
    			    //if(t == 1) menu(sockfd);
    			    
    			    break;
    		case '3':
    			return;
    		default : break;
    			

    	}
    }



}