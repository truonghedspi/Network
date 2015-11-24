#include <termios.h>
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


static struct termios old, new;
int currentSockFD = -1;
int numUsersOnline = 0;
char currenUserName[LEN];

typedef enum{
	USER,
	ROOM
}TypeChatList;

typedef struct{
	char userName[20];
	TypeChatList type;
}User_List;
User_List chatList[100];
User_List userList[100];

void initTermios(int echo) {
	tcgetattr(0, &old);                       
	new = old;                                
	new.c_lflag &= ~ICANON;                   
	new.c_lflag &= echo ? ECHO : ~ECHO;       
	tcsetattr(0, TCSANOW, &new);              
}

void resetTermios(void) {
  tcsetattr(0, TCSANOW, &old);
}

char getch_(int echo) {
	char ch;
	initTermios(echo);
	ch = getchar();
	resetTermios();
	return ch;
}

char getch(void) {
    return getch_(0);
}

int get_pass(char pass[]){
	int i=0;

	while(1){
		pass[i]=getch();
		if(pass[i]== '\n') break;
		i++;
	}
	pass[i]='\0';
	return 1;
}

int check_input(char buff[]){
	if(strlen(buff)<6 || strlen(buff)>=15) return 0;
	else return 1;
}

int check_input2(char buff[]){
	if(strlen(buff)>0 && strlen(buff)<=146) return 1;
	else if(strlen(buff) == 0){
		printf("\nNOT BE EMPTY!!");
		return 0;
	}else if(strlen(buff) >146){
		printf("\nCHAT CONTENT TOO LONG!!");
		return 0;
	}
}

int check_type(Respond respond){
	return respond.typeRespond;
}

int check_currUserName(){
	int i;
	for( i=0;i<numUsersOnline;i++){
		if(strcmp(userList[i].userName,currenUserName)==0)
			return 1;
	}
	return 0;
}

//phan biet loai chat voi user
void type_chat_respond(char buff[]){

	ChatRespond chatRespond;

	chatRespond=(*(ChatRespond*)buff);
	switch(chatRespond.chatType){
		case QUIT_CHAT:
			del_partner(chatRespond.userName);
			break;
		case CHAT:
			printf("\n@%s: %s",chatRespond.userName,chatRespond.messenger);
			break;		
	}
}

//phan biet loai chat voi room
void type_chat_room_respond(char buff[]){
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

void notification(char buff[]){
	UserChangeStatusRespond statusRespond;

	statusRespond=(*(UserChangeStatusRespond*)buff);
	if(statusRespond.userStatus==ONLINE){
		printf("\n@%s vua online",statusRespond.userName);
		add_partner(statusRespond.userName);
	}else if(statusRespond.userStatus==OFFLINE){
		printf("\n@%s vua offline",statusRespond.userName);
		del_partner(statusRespond.userName);
	}
}
void check_respond(char mesg[]){
	Respond respond;
	int t;

	respond=(*(Respond*)mesg);
	switch(respond.typeRespond){
		case LOGIN_RESPOND:
			break;
		case LOGOUT_RESPOND:
			break;
		case REGISTER_RESPOND:
			break;
		case CHAT_RESPOND:
			type_chat_respond(mesg);
			break;
		case CHAT_ROOM_RESPOND:
			
			
			break;
		case GET_ONLINE_USER_LIST_RESPOND:
			printf("\nCHAT_RESPOND");
			t=take_user_list(mesg);
			if(t == 1) {
				choose_user();
				chatting();
			}
			else if(t == 0) printf("\nRECEIVING LIST...");
			else if(t == 2) printf("\nNO OTHER USER IS ONLINE!");
			printf("\n");
			break;
		case USER_CHANGE_STATUS_RESPOND:
			notification(mesg);
			break;

	}
}

//them doi tuong vao danh sach chatList
int add_partner(char userName[]){
	int i=0;

	strcpy(userList[numUsersOnline].userName,userName);
	numUsersOnline++;
	//chatList[i].type=type;
	return 1;
}

//xoa doi tuong khoi danh sach chatList
int del_partner(char userName[]){
	int i=0;

	for(i=0;i<numUsersOnline;i++){
		if(strcmp(userList[i].userName,userName)==0){
			break;
		}
	}
	if(i<numUsersOnline){
		while(1){
			if(i == numUsersOnline)
				break;
			strcpy(chatList[i].userName,chatList[i+1].userName);
			chatList[i].type = chatList[i+1].type;
			i++;
		}
		strcpy(chatList[i].userName,"");
		numUsersOnline--;
		return 1;
	}else if(i==numUsersOnline)
		printf("\ndel_partner ERROR" );
		return 0;
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
		printf("\nUserName: ");
		strcpy(tk,"");
		fgets(tk,LEN,stdin);
		tk[strlen(tk)-1]='\0';
		if(check_input(tk) == 0){
			printf("\nTry Again!!");
		}else break;
	}
	strcpy(user.userName,tk);
	while(1){
		printf("Password(Not Diplay): ");
		get_pass(pass);
		if(check_input(pass) == 0){
			printf("\nTry Again!!\n");
		}else break;
	}
	strcpy(user.password,pass);
	user.typeRequest=LOGIN_REQUEST;
	memcpy(mesg,&user,204);
	send(currentSockFD, mesg,204, 0);
	strcpy(mesg,"");
	recv(currentSockFD, mesg, 204, 0);
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
	RegisterRequest user;
	Respond respond;
	RegisterRespond registerRespond;
	LoginRequest loginRequest;
	LoginRespond loginRespond;
	
	while(1){
		printf("\nUserName: ");
		strcpy(tk,"");
		fgets(tk,LEN,stdin);
		tk[strlen(tk)-1]='\0';
		if(check_input(tk) == 0){
			printf("\nTry Again!!");
		}else break;
	}
	strcpy(user.userName,tk);
	strcpy(loginRequest.userName,tk);
	while(1){
		printf("Password(Not Display): ");
		get_pass(pass);
		if(check_input(pass) == 0){
			printf("\nTry Again!!\n");
		}else break;
	}
	while(1){
		printf("Re-Enter Password: ");
		get_pass(comf_pass);
		if(strcmp(pass,comf_pass)==0) break;
	}
	strcpy(user.password,pass);
	strcpy(loginRequest.password,pass);
	strcpy(user.passwordConfirm,comf_pass);
	user.typeRequest=REGISTER_REQUEST;
	loginRequest.typeRequest=LOGIN_REQUEST;
	memcpy(mesg,&user,LEN);
	send(currentSockFD, mesg,204, 0);
	strcpy(mesg,"");
	recv(currentSockFD, mesg, LEN, 0);
	respond=(*(Respond*)mesg);
	if(respond.typeRespond == REGISTER_RESPOND){
		registerRespond=(*(RegisterRespond*)mesg);
		if(registerRespond.registerResult == REGISTER_SUCCESS){
			printf("\n%s",registerRespond.messenger);
			memcpy(mesg,&loginRequest,LEN);
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

int log_out(){
	Request request;
	char mesg[204];

	request.typeRequest=LOGOUT_REQUEST;
	memcpy(mesg,&request,204);
	send(currentSockFD,mesg,204,0);
	return 1;
}

int creat_room(){
}

//tra loi yeu cau moi vao nhom Chat
int answer_invite_room(){
}

int clear_user_list(){
	int i;

	for(i=0;i<100;i++){
		strcpy(userList[i].userName,"");
	}
}

int take_user_list(char mesg[]){
	int i=0,y=0;
	GetOnlineUserListRespond userListRespond;
	
	userListRespond=(*(GetOnlineUserListRespond*)mesg);
	for(i=0;i<userListRespond.numUsersOnline;i++){
		for (y = 0; y < numUsersOnline; ++y) {
			if (strcmp(userListRespond.onlineUserList[i], userList[y].userName) == 0) {
				break;
			}
		}
		if(y == numUsersOnline) {
			strcpy(userList[y].userName, userListRespond.onlineUserList[i]);
			++numUsersOnline;
		}
	}
	if(numUsersOnline == 0) return 2;
	if(userListRespond.getListRespondResult == END) return 1;
	if(userListRespond.getListRespondResult == CONT) return 0;
}

//gui yeu cau lay danh sach user online
void online_user_list_request(){
	GetOnlineUserListRequest	listRequest;
	char mesg[LEN];

	listRequest.typeRequest=GET_ONLINE_USER_LIST_REQUEST;
	memcpy(mesg,&listRequest,204);
	send(currentSockFD,mesg,204,0);
	return;
}

//chon user de chat
int choose_user(){
	int i,y;
	
	printf("\nHave %d User Is Online",numUsersOnline);
	for ( i = 0; i < numUsersOnline; i++){
		printf("\n%d.%s",i+1,userList[i].userName);
	}
	printf("\n0.Exit");
	printf("");
	while(1){
		y=wait_int();
		if(0<=y&&y<=numUsersOnline) break;
	}
	if(y==0){
		return 0;
	}else{
		strcpy(currenUserName,userList[y-1].userName);
		printf("\nYou are chat with @%s",userList[y-1].userName);
	}	
}

//gui noi dung Chat
int send_chat(char buff[],ChatType type){
	ChatRequest chatRequest;
	int t;
	char mesg[LEN];

	t=check_currUserName();
	if(t==1){
		chatRequest.typeRequest=CHAT_REQUEST;
		chatRequest.chatType=type;
		strcpy(chatRequest.userName,currenUserName);
		strcpy(chatRequest.messenger,buff);
		memcpy(mesg,&chatRequest,204);
		send(currentSockFD,mesg,204,0);
		return 1;
	}else if(t==0){
		printf("\nCurrenUserName NULL");
		return 0;
	}
}

int chatting(){
	char buff[LEN];

	printf("\nYou are chat with @%s",currenUserName);
	do{
		wait_char(buff);
		if(strcmp(buff,"\\q")==0 || strcmp(buff,"\\Q")==0){

		}else
		if(strcmp(buff,"\\r")==0 || strcmp(buff,"\\R")==0){

		}else
		if(strcpy(buff,"\\l")==0 || strcpy(buff,"\\L")==0){

		}else
		if(strcpy(buff,"\\h")==0 || strcpy(buff,"\\H")==0){

		}else{
			send_chat(buff,CHAT);
		}
	}while(1);
}

int wait_char(char buff[LEN]){
	
	int rv;
	int size_recv;
	char mesg[LEN];
	fd_set readSet;
	struct timeval tv;
	Respond respond;

	strcpy(buff,"");
	while(1){
		tv.tv_sec = 0;
		tv.tv_usec =0;
		FD_SET(currentSockFD, &readSet);
		FD_SET(fileno(stdin), &readSet);
		
		select(currentSockFD +1, &readSet, NULL, NULL, &tv);
		if(FD_ISSET(currentSockFD, &readSet)){
			strcpy(mesg,"");
			size_recv = recv(currentSockFD,mesg,LEN,0);
			if(size_recv == 0){
				printf("Server disconnect\n");
				close(currentSockFD);
				exit(0);
			}
			if (size_recv < 0) {
				printf("Error\n");
				close(currentSockFD);
				exit(0);
			}
			if(size_recv > 0){
				check_respond(mesg);
			}
			if(--rv <= 0){
				//strcpy(buff,"");
                continue;
            }
		}
		if (FD_ISSET(fileno(stdin), &readSet)){	
			fgets(buff,LEN,stdin);
			buff[strlen(buff)-1]='\0';
			return;
		} 
	}
	//return buff;
}

int wait_int(){
	
	char buff[LEN];
	int n;
	int rv;
	int size_recv;
	char mesg[LEN];
	fd_set readSet;
	struct timeval tv;
	Respond respond;

	strcpy(buff,"");
	while(1){
		tv.tv_sec = 0;
		tv.tv_usec =0;
		FD_SET(currentSockFD, &readSet);
		FD_SET(fileno(stdin), &readSet);
		
		select(currentSockFD +1, &readSet, NULL, NULL, &tv);
		if(FD_ISSET(currentSockFD, &readSet)){
			strcpy(mesg,"");
			size_recv = recv(currentSockFD,mesg,LEN,0);
			if(size_recv == 0){
				printf("Server disconnect\n");
				close(currentSockFD);
				exit(0);
			}
			if (size_recv < 0) {
				printf("Error\n");
				close(currentSockFD);
				exit(0);
			}
			if(size_recv > 0){
				check_respond(mesg);
			}
			if(--rv <= 0){
				//n=-1;
                continue;
            }
		}
		if (FD_ISSET(fileno(stdin), &readSet)){	
			scanf("%d",&n);
			fgets(buff,LEN,stdin);
			return n;
		} 
	}	
}

void main_menu() {
			printf("\n***WAIT USER OR CHOOSE***\n");
			printf("\n1.CHOOSE A USER TO CHAT");
			printf("\n2.CREAT ROOM");
			printf("\n3.INVITE USER INTO ROOM");
			printf("\n4.EXIT");
			printf("\nChoose: ");
			printf("\n");
}

void menu(){
	//char buff[LEN];
	int choose;

	main_menu();
	choose=wait_int();

	switch(choose){
		case 1 :
			printf("\nYou choose 1");
			clear_user_list();
			online_user_list_request();
			break;
		case 2 :
			printf("\nYou choose 2");
			//creat_room(chatList);
			//chat
			break;
		case 3 :
			printf("\nYou choose 3");
			//invite_user_into_room(chatList) 
			break ;
		case 4 :
			printf("\nYou choose 4");
			log_out();
			return;
		default :
			break;
	}
}

void main(){
	int sockfd;
	struct sockaddr_in serv_addr;
	char choose[2];
	int t;
	char buff[LEN];

    serv_addr.sin_family = AF_INET;       
    serv_addr.sin_port = htons(5500);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
    size_t size =100;

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    	printf("\nSocket ERROR!!");
    }
    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\nConnect ERROR!!\n");
       return ;
    }else
    while(1){
    	currentSockFD = sockfd;
    	printf("\n*****MENU*****\n");
    	printf("\n1.LOG IN\n2.SIGN UP\n3.EXIT" );
    	printf("\nChoose: ");
    	fgets(choose,3,stdin);
    	switch(choose[0]){

    		case '1': 
    				printf("\nLog In");
    				t = login();
    			    if(t == 1) {
    			    	menu();
        			    do{
    						wait_char(buff);	
    					} while (1);
    				}
   					break;
    		case '2': 	
    				printf("\nSign Up");
    				t = sign_up();
    			    //if(t == 1) menu(sockfd);
    			    
    			    break;
    		case '3':
    			close(currentSockFD);
    			return;
    		default : break;
    			

    	}
    }
}
