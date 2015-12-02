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
#define MAX_LENGTH 204

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
int getInt() {
	int num;

	while (scanf("%d", &num) != 1) {
		while(getchar() != '\n');
	}
	while(getchar() != '\n');

	return num;
}
void getString(char * str) {
	
	while (1) {
		fgets(str,MAX_LENGTH + 1, stdin);
		str[strlen(str) - 1] = '\0';
		if (str[0] == '\n' || str[0] == '\0') {
			printf("Can't be Empty!\nTry again\n");
			continue;
		}
		return;
	}
}
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
int mygetch( ) {		//ham tao dau * khi nhap password
  struct termios oldt,newt;
  int ch;
  tcgetattr( STDIN_FILENO, &oldt );
  newt = oldt;
  newt.c_lflag &= ~( ICANON | ECHO );
  tcsetattr( STDIN_FILENO, TCSANOW, &newt );
  ch = getchar();
  tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
  return ch;
}
int get_pass(char pass[]){
	int i=0;

	while(1){
		pass[i]=getch();
		if(pass[i]== '\n') break;
		printf("*");
		i++;
	}
	pass[i]='\0';
	return 1;
}

void format_string(char* string){
	int i = 0;
	while(string[i] != '\0'){
		string[i] = '\0';
		i++;
	}
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
int show_user_list(){
	int i;
	fflush(stdout);
	printf("\nHave %d User Is Online",numUsersOnline);
	for ( i = 0; i < numUsersOnline; i++){
		fflush(stdout);
		printf("\n%d.%s",i+1,userList[i].userName);
	}
	fflush(stdout);
	printf("\n");
	return 1;
}
//phan biet loai chat voi user
void type_chat_respond(char buff[]){

	ChatRespond chatRespond;

	chatRespond=(*(ChatRespond*)buff);
	switch(chatRespond.chatResult){
		case CHAT_SUCCESS:
			//printf("\nDa gui cho client");
			break;
		case CHAT_USER_OFFLINE:
			del_partner(chatRespond.userNameSender);
			break;
		case CHAT_FRIEND_RECV:
			fflush(stdout);
			printf("\n@%s: %s\n",chatRespond.userNameSender,chatRespond.messenger);
			break;		
	}
}

//phan biet loai chat voi room
void type_chat_room_respond(char buff[]){
	/*ChatRoomRespond chatRoomRespond;

	chatRoomRespond=(*(ChatRoomRespond*)buff);
	switch(chatRoomRespond.chatResult){
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
	}*/
}

void notification(char buff[]){
	UserChangeStatusRespond statusRespond;

	statusRespond=(*(UserChangeStatusRespond*)buff);
	if(statusRespond.userStatus==ONLINE){
		fflush(stdout);
		printf("\nSERV: @%s vua online",statusRespond.userName);
		add_partner(statusRespond.userName);
		fflush(stdout);
		printf("\n");
	}else if(statusRespond.userStatus==OFFLINE){
		fflush(stdout);
		printf("\nSERV: @%s vua offline",statusRespond.userName);
		del_partner(statusRespond.userName);
		if(strcmp(statusRespond.userName,currenUserName)==0){
			format_string(currenUserName);
		}
		fflush(stdout);
		printf("\n");
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
			t=take_user_list(mesg);
			if(t == 1) {
				show_user_list();
			}
			else if(t == 0){
				fflush(stdout);
			 	printf("\nRECEIVING LIST...");
			 }
			else if(t == 2){
				fflush(stdout);
			 	printf("\nNO OTHER USER IS ONLINE!");
			 }
			fflush(stdout);
			printf("\n");
			break;
		case USER_CHANGE_STATUS_RESPOND:
			notification(mesg);
			break;

	}
}
int wait_char(char buff[LEN]){
	int rv;
	int size_recv;
	char mesg[LEN];
	fd_set readSet;
	struct timeval tv;
	Respond respond;

	format_string(buff);
	//fflush(stdout);
	while(1){
		tv.tv_sec = 0;
		tv.tv_usec =0;
		FD_SET(currentSockFD, &readSet);
		FD_SET(fileno(stdin), &readSet);
		
		select(currentSockFD +1, &readSet, NULL, NULL, &tv);
		if (FD_ISSET(fileno(stdin), &readSet)){
			getString(buff);
			return 0;
		}
		if(FD_ISSET(currentSockFD, &readSet)){
			strcpy(mesg,"");
			size_recv = recv(currentSockFD,mesg,LEN,0);
			if(size_recv == 0){
				fflush(stdout);
				printf("\nServer disconnect\n");
				close(currentSockFD);
				exit(0);
			}
			if (size_recv < 0) {
				fflush(stdout);
				printf("Error\n");
				close(currentSockFD);
				exit(0);
			}
			if(size_recv > 0){
				check_respond(mesg);
			}
			format_string(buff);
			return 1;
			/*if(--rv <= 0){
                //continue;
                return 1;
            }*/
		}
	}
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

	format_string(buff);
	//fflush(stdout);
	while(1){
		tv.tv_sec = 0;
		tv.tv_usec =0;
		FD_SET(currentSockFD, &readSet);
		FD_SET(fileno(stdin), &readSet);
		fflush(stdin);	
		select(currentSockFD +1, &readSet, NULL, NULL, &tv);
		if (FD_ISSET(fileno(stdin), &readSet)){	
			return getInt();
		}
		if(FD_ISSET(currentSockFD, &readSet)){
			strcpy(mesg,"");
			size_recv = recv(currentSockFD,mesg,LEN,0);
			if(size_recv == 0){
				fflush(stdout);
				printf("\nServer disconnect\n");
				close(currentSockFD);
				exit(0);
			}
			if (size_recv < 0) {
				fflush(stdout);
				printf("Error\n");
				close(currentSockFD);
				exit(0);
			}
			if(size_recv > 0){
				check_respond(mesg);
			}
			return -1;
			/*if(--rv <= 0){
				//n=-1;
                //continue;
                return n=-1;
            }*/
		}
		 
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
	}else if(i==numUsersOnline){
		fflush(stdout);
		printf("\ndel_partner ERROR" );
		return 0;
	}
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
		printf("Password: ");
		get_pass(pass);
		if(check_input(pass) == 0){
			printf("\nTry Again!!\n");
		}else break;
	}
	strcpy(user.password,pass);
	user.typeRequest=LOGIN_REQUEST;
	memcpy(mesg,&user,LEN);
	send(currentSockFD, mesg,LEN, 0);
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
		printf("Password: ");
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
	send(currentSockFD, mesg,LEN, 0);
	strcpy(mesg,"");
	recv(currentSockFD, mesg, LEN, 0);
	respond=(*(Respond*)mesg);
	if(respond.typeRespond == REGISTER_RESPOND){
		registerRespond=(*(RegisterRespond*)mesg);
		if(registerRespond.registerResult == REGISTER_SUCCESS){
			printf("\n%s",registerRespond.messenger);
			memcpy(mesg,&loginRequest,LEN);
			send(currentSockFD, mesg,LEN, 0);
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
	char mesg[LEN];

	request.typeRequest=LOGOUT_REQUEST;
	memcpy(mesg,&request,LEN);
	send(currentSockFD,mesg,LEN,0);
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
		for (y = 0; y < numUsersOnline; y++) {
			if (strcmp(userListRespond.onlineUserList[i], userList[y].userName) == 0) {
				break;
			}
		}


		//khong co thang nao ton tai

		if(y == numUsersOnline) {
			strcpy(userList[y].userName, userListRespond.onlineUserList[i]);
			numUsersOnline++;
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
	memcpy(mesg,&listRequest,LEN);
	send(currentSockFD,mesg,LEN,0);
	return;
}

//chon user de chat
int choose_user(){
	char userName[LEN];
	
	printf("\nEnter User Name Want Chat With:");
	fflush(stdin);
	fflush(stdout);
	wait_char(userName);
	if(strcmp(userName,"")!=0 ){
		strcpy(currenUserName,userName);
	}
	fflush(stdout);
	printf("\n");	
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
		strcpy(chatRequest.userNameReceiver,currenUserName);
		strcpy(chatRequest.messenger,buff);
		memcpy(mesg,&chatRequest,LEN);
		send(currentSockFD,mesg,LEN,0);
		return 1;
	}else if(t==0){
		fflush(stdout);
		printf("\nCurrenUserName NULL");
		return 0;
	}
}

int chatting(){
	char buff[LEN];

						
	fflush(stdout);
	printf("\nYou are chat with @%s",currenUserName);
	do{
		fflush(stdout);
		wait_char(buff);
		if(strcmp(buff,"")!=0){
				if(strcmp(buff,"\\q")==0 || strcmp(buff,"\\Q")==0){
		
				}else
				if(strcmp(buff,"\\r")==0 || strcmp(buff,"\\R")==0){
		
				}else
				if(strcmp(buff,"\\l")==0 || strcmp(buff,"\\L")==0){
		
				}else
				if(strcmp(buff,"\\h")==0 || strcmp(buff,"\\H")==0){
					return 1;
				}else{
					send_chat(buff,CHAT_FRIEND_SEND);
				}
			}
	}while(1);
}



void menu(){

	int choose;
	
	do{	
		
		printf("\n***WAIT USER OR CHOOSE***\n");
		printf("\n1.SHOW USER LIST");
		printf("\n2.CHAT WITH");
		printf("\n3.INVITE USER INTO ROOM");
		printf("\n4.EXIT\n");
		fflush(stdout);
		choose=wait_int();
		if(choose>0){
				switch(choose){
					case 1 :
						fflush(stdout);
						printf("\nYou choose 1");
						online_user_list_request();
						break;
					case 2 :
						
						printf("\nYou choose 2");
						fflush(stdout);
						choose_user();
						if(check_currUserName()==1){
							chatting();
						}
						break;
					case 3 :
						fflush(stdout);
						printf("\nYou choose 3");
						//invite_user_into_room(chatList) 
						break ;
					case 4 :
						fflush(stdout);
						printf("\nYou choose 4");
						log_out();
						return;
					default :
						break;
				}
			}
	}while(choose!=4);

}

void main(){
	int sockfd;
	struct sockaddr_in serv_addr;
	int choose;
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
    }
    currentSockFD = sockfd;
    printf("\n*****MENU*****");
	printf("\n1.LOG IN" );
	printf("\n2.SIGN UP");
	printf("\n3.EXIT");
	printf("\nChoose: ");
    do{
    	fflush(stdout);
    	choose=wait_int();
    	if(choose>0){
    	    	switch(choose){
    	    		case 1: 
    	    				printf("\nLog In");
    	    				t = login();
    	    			    if(t == 1) {
    	    			    	menu();
    	        			    /*do{
    	    						wait_char(buff);	
    	    					} while (1);*/
    	    				}
    	   					break;
    	    		case 2: 	
    	    				printf("\nSign Up");
    	    				t = sign_up();
    	    			    if(t == 1) {
    	    			    	menu();
    	        			    /*do{
    	    						wait_char(buff);	
    	    					} while (1);*/
    	    				}
    	    			    
    	    			    break;
    	    		case 3:
    	    			close(currentSockFD);
    	    			return;
    	    		default : 
    	    			printf("\n*****MENU*****\n");
    			    	printf("\n1.LOG IN" );
    			    	printf("\n2.SIGN UP");
    			    	printf("\n3.EXIT");
    			    	printf("\nChoose: ");
    	    			break;
    	    	}
    	    }
    }while(1);
}
