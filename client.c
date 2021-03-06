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
int numBlockList = 0;
char currenUserName[LEN];
char currenRoom[LEN];


typedef enum{
	USER,
	ROOM
}TypeChatList;

typedef struct{
	char userName[20];
}User_List;
User_List userList[100];
User_List blockList[100];

typedef struct{
	char roomName[LEN];
	int numberUser;
}Room_List;
Room_List roomList[10];

int getInt(); 
void getString(char * str);
void initTermios(int echo);
void resetTermios(void);
char getch_(int echo);
char getch(void);
int mygetch();
int get_pass(char pass[]);
void format_string(char* string);
int check_input(char buff[]);
int check_input2(char buff[]);
int check_type(Respond respond);
int check_currRoom(char roomName[]);
int check_currUserName(char userName[]);
int check_blockUserName(char userName[]);
int send_request(void *request);
int add_partner(char userName[]);
int del_partner(char userName[]);
int add_block(char userName[]);
int del_block(char userName[]);
int show_room_list();
int show_user_list();
int show_block_list();
void type_chat_respond(char buff[]);
void type_room_respond(char buff[]);
void type_block_respond(char buff[]);
void notification(char buff[]);
int wait_char(char buff[LEN]);
int wait_int();
int login();
int sign_up();
int log_out();
int take_room_list(char mesg[]);
int take_user_list(char mesg[]);
int take_block_list(char mesg[]);
void online_user_list_request();
void room_list_request();
void block_list_request();
int out_room();
int choose_room();
int choose_user();
int send_chat_room(char buff[],RoomType type);
int send_chat(char buff[],ChatType type);
void reply();
int block_user();
int un_block_user();
void check_respond(char mesg[]);
int chatting_room();
int chatting();
void menu();

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
    do{
    	printf("\e[0;34m");
    	printf("\n*****MENU*****");
		printf("\n1.LOG IN" );
		printf("\n2.SIGN UP");
		printf("\n3.EXIT");
		printf("\nChoose: ");
		printf("\e[0m");
    	fflush(stdout);
    	fflush(stdin);
    	choose=wait_int();
    	fflush(stdin);
    	if(choose>0){
    	    	switch(choose){
    	    		case 1: 
    	    				printf("\nLog In");
    	    				t = login();
    	    			    if(t == 1) {
    	    			    	printf("\n\e[0;34m(Type \"\e[0mmenu\e[0;34m\" to go MENU)\e[0m");
								online_user_list_request();
								room_list_request();
								block_list_request();
								while(1){
									fflush(stdout);
									wait_char(buff);
									if(strcmp(buff,"") != 0)
										break;
								}
    	    			    	menu();
    	    				}
    	   					break;
    	    		case 2: 	
    	    				printf("\nSign Up");
    	    				t = sign_up();
    	    			    if(t == 1) {
    	    			    	printf("(\nType \"menu\" to go MENU)");
								online_user_list_request();
								room_list_request();
								block_list_request();
								while(1){
									fflush(stdout);
									wait_char(buff);
									if(strcmp(buff,"") != 0)
										break;
								}
    	    			    	menu();
    	    				}
    	    			    break;
    	    		case 3:
    	    			close(currentSockFD);
    	    			return;
    	    		default : 
    	    			printf("\nCHOOSE FALSE! AGAIN\n" );
    	    			break;
    	    	}
    	    }
    }while(1);
}

//----------ham phuc vu cho viec nhap password
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
int mygetch( ) {		
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
		if(pass[i] == '\n') break;
		if(pass[i] == 127){
			if(i==0) continue;
			putchar(8);
			i--;
			continue;
		}
		printf("*");
		i++;
	}
	pass[i]='\0';
	return 1;
}
//----------------------------------------------

//-------------cac ham phu
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

int check_currRoom(char roomName[]){
	int i;

	//if(strcmp(roomName,"")==0) return 0;
	for( i=0;i<10;i++){
		if(strcmp(roomList[i].roomName,roomName)==0)
			return 1;
	}
	return 0;
}

int check_blockUserName(char userName[]){
	int i;

	if(numBlockList==0) return 0;
	//if(strcmp(userName,""))return 0;
	for( i=0;i<numBlockList;i++){
		if(strcmp(blockList[i].userName,userName)==0)
			return 1;
	}
	format_string(userName);
	return 0;
}

int check_currUserName(char userName[]){
	int i;

	if(numUsersOnline==0) return 0;
	//if(strcmp(userName,"")==0) return 0;
	for( i=0;i<numUsersOnline;i++){
		if(strcmp(userList[i].userName,userName)==0)
			return 1;
	}
	format_string(userName);
	return 0;
}

int add_partner(char userName[]){
	int i=0;

	strcpy(userList[numUsersOnline].userName,userName);
	numUsersOnline++;
	return 1;
}

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
			strcpy(userList[i].userName,userList[i+1].userName);
			i++;
		}
		strcpy(userList[i].userName,"");
		numUsersOnline--;
		return 1;
	}else if(i==numUsersOnline){
		printf("\ndel_partner ERROR" );
		return 0;
	}
}

int add_block(char userName[]){
	int i=0;

	strcpy(blockList[numBlockList].userName,userName);
	numBlockList++;
	return 1;
}

int del_block(char userName[]){
	int i;
	
	i=0;
	while(1){
		if(strcmp(blockList[i].userName,userName)==0){
			break;
		}
		i++;

	}
	if(i<numBlockList){
		while(1){
			if(i == numBlockList)
				break;
			strcpy(blockList[i].userName,blockList[i+1].userName);
			i++;
		}
		strcpy(blockList[i].userName,"");
		numBlockList--;
		return 1;
	}
	if(i==numBlockList){
		printf("\ndel_block ERROR" );
		return 0;
	}
}

void reply(){
	char tempUserName[LEN];
	char buff[LEN];

	strcpy(tempUserName,currenUserName);
	choose_user();
	if(check_currUserName(currenUserName)==1){
		printf("\n\e[0;34mReply \e[1;33m%s:\e[0m ",currenUserName);
		fflush(stdout);
		wait_char(buff);
		if(strcmp(buff,"")!=0){
			send_chat(buff,CHAT_FRIEND_SEND);
		}
	}
	strcpy(currenUserName,tempUserName);
}

//---------------------------------------------

//---------------gui cac yeu cau len server
int send_request(void *request){
	return send(currentSockFD,(char* )request,LEN,0);
}

int send_chat(char buff[],ChatType type){
	ChatRequest chatRequest;
	int t;
	char mesg[LEN];

	t=check_currUserName(currenUserName);
	if(t==1){
		chatRequest.typeRequest=CHAT_REQUEST;
		chatRequest.chatType=type;
		strcpy(chatRequest.userNameReceiver,currenUserName);
		strcpy(chatRequest.messenger,buff);
		send_request(&chatRequest);
		return 1;
	}else if(t==0){
		printf("\e[1;31m");
		printf("\nYou No Chat With Someone\n");
		printf("\e[0m");
		format_string(currenUserName);
		return 0;
	}
}

int send_chat_room(char buff[],RoomType type){
	RoomRequest roomRequest;
	int t;
	char mesg[LEN];

	t=check_currRoom(currenRoom);
	if(t==1){
		roomRequest.typeRequest=ROOM_REQUEST;
		roomRequest.roomType=type;
		strcpy(roomRequest.roomName,currenRoom);
		strcpy(roomRequest.messenger,buff);
		send_request(&roomRequest);
		return 1;
	}else if(t==0){
		printf("\n\e[1;31mYou Not In Room\n\e[0m");
		format_string(currenRoom);
		return 0;
	}
}
//------------------------------------------

//----------------yeu cau server tra ve danh sach

void online_user_list_request(){
	GetOnlineUserListRequest	request;
	char mesg[LEN];

	request.typeRequest=GET_ONLINE_USER_LIST_REQUEST;
	send_request(&request);
	return;
}

void room_list_request(){
	GetRoomListRequest request;
	char mesg[LEN];

	request.typeRequest=GET_ROOM_LIST_REQUEST;
	send_request(&request);
	return;
}

void block_list_request(){
	GetBlockListRequest request;

	request.typeRequest=GET_BLOCK_LIST_REQUEST;
	send_request(&request);
	return ;
}

//-----------------nhan danh sach tra ve

int take_room_list(char mesg[]){
	GetRoomListRespond roomListRespond;
	int i;

	roomListRespond=(*(GetRoomListRespond*)mesg);
	for( i=0;i<10;i++){
		strcpy(roomList[i].roomName,roomListRespond.roomList[i]);
		roomList[i].numberUser=roomListRespond.numberUser[i];
	}
	return 1;
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
		if(y == numUsersOnline) {
			strcpy(userList[y].userName, userListRespond.onlineUserList[i]);
			numUsersOnline++;
		}
	}
	if(numUsersOnline == 0) return 2;
	if(userListRespond.getListRespondResult == END) return 1;
	if(userListRespond.getListRespondResult == CONT) return 0;
}

int take_block_list(char mesg[]){
	int i=0,y=0;
	GetBlockListRespond blockListRespond;
	
	blockListRespond=(*(GetBlockListRespond*)mesg);
	for(i=0;i<blockListRespond.numBlockList;i++){
		for (y = 0; y < numBlockList; y++) {
			if (strcmp(blockListRespond.userNameBlock[i], blockList[y].userName) == 0) {
				break;
			}
		}
		if(y == numBlockList) {
			strcpy(blockList[y].userName, blockListRespond.userNameBlock[i]);
			numBlockList++;
		}
	}
	if(numBlockList == 0) return 2;
	if(blockListRespond.getBlockRespondResult == END) return 1;
	if(blockListRespond.getBlockRespondResult == CONT) return 0;
}

//-------------hien thi danh sach

int show_room_list(){
	int i;

	for(i=0;i<10;i++){
		printf("\n\e[1;36m%s\e[0m : %d member",roomList[i].roomName,roomList[i].numberUser);
	}
	printf("\n");
	return 1;
}

int show_user_list(){
	int i;
	
	if(numUsersOnline == 0) {
		printf("\n\e[1;31mNO OTHER USER ONLINE!\e[0m\n");
		return 0;
	}
	printf("\n\e[0;34mHave %d User Is Online\e[0m",numUsersOnline);
	if(numUsersOnline==0) return 0;
	for ( i = 0; i < numUsersOnline; i++){
		printf("\n%d.\e[1;33m%s\e[0m",i+1,userList[i].userName);
	}
	printf("\n");
	return 1;
}

int show_block_list(){
	int i;

	if(numBlockList==0) {
		printf("\n\e[1;31mBlock List Empty!\e[0m\n");
		return 0;
	}
	printf("\n\e[0;34mYou Block %d User\e[0m",numBlockList);
	for ( i = 0; i < numBlockList; i++){
		printf("\n%d.\e[1;33m%s\e[0m",i+1,blockList[i].userName);
	}
	printf("\n");
	return 1;
}

//--------------------------------------------	

//---------------check kieu cho respond tra ve

void check_respond(char mesg[]){
	Respond respond;
	BlockUserRespond blockRespond;
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
		case ROOM_RESPOND:
			type_room_respond(mesg);
			break;
		case BLOCK_RESPOND:
			type_block_respond(mesg);
			break;
		case GET_ONLINE_USER_LIST_RESPOND:
			t=take_user_list(mesg);
			if(t == 1) {
			}
			else if(t == 0){
			 	//printf("\n\e[1;31mRECEIVING LIST...\e[0m");
			}
			else if(t == 2){
			 	printf("\n\e[1;31mNO OTHER USER IS ONLINE!\e[0m");
			}
			printf("\n");
			break;
		case GET_ROOM_LIST_RESPOND:
			take_room_list(mesg);
			break;
		case GET_BLOCK_LIST_RESPOND:
			take_block_list(mesg);
			break;
		case USER_CHANGE_STATUS_RESPOND:
			notification(mesg);
			break;
		default :
			break;

	}
}

void type_chat_respond(char buff[]){

	ChatRespond chatRespond;

	chatRespond=(*(ChatRespond*)buff);
	switch(chatRespond.chatResult){
		case CHAT_SUCCESS:
			break;
		case CHAT_USER_OFFLINE:
			del_partner(chatRespond.userNameSender);
			break;
		case CHAT_FRIEND_RECV:
			printf("\n\e[1;33m%s: \e[0;33m%s\n\e[0m",chatRespond.userNameSender,chatRespond.messenger);
			break;
		case CHAT_LOG_RESPOND:
			printf("\n%s",chatRespond.messenger);
			break;
		case CHAT_USER_BLOCK:
			printf("\n\e[1;31mCHAT_USER_BLOCK\n\e[0m");
			break;
		default :
			break;		
	}
}

void type_room_respond(char buff[]){
	RoomRespond roomRespond;
	int i;

	i=0;
	roomRespond=(*(RoomRespond*)buff);
	switch(roomRespond.roomResult){
		case CHAT_ROOM:
			printf("\n\e[1;36m%s:\e[1;33m%s: \e[0;33m%s\n\e[0m",roomRespond.roomName,roomRespond.userName,roomRespond.messenger);
			return;
		case JOIN_SUCCESS:
			printf("\n\e[0;34mJoin \e[1;36m%s \e[0;34mSuccess\n\e[0m",roomRespond.roomName);
			strcpy(currenRoom,roomRespond.roomName);
			chatting_room();
			return;
		case USER_JOIN_ROOM :
			printf("\n\e[1;36m%s:\e[0;34m User \e[1;33m%s \e[0;34mJoin Room!\n\e[0m",roomRespond.roomName,roomRespond.userName);
			return;
		case USER_OUT_ROOM:
			printf("\n\e[1;36m%s:\e[0;34m User \e[1;33m%s \e[0;34mOut Room!\n\e[0m",roomRespond.roomName,roomRespond.userName);
			return;
		case JOIN_FALSE:
			printf("\n\e[1;37mSERV(JOIN_FALSE):\e[0m %s\n",roomRespond.messenger);
			strcpy(currenRoom,roomRespond.roomName);
			chatting_room();
			return;
		case OUT_SUCCESS:
			if(strcmp(roomRespond.roomName,currenRoom)==0){
				format_string(currenRoom);
			}
			return;
		case OUT_FALSE:
			printf("\n\e[1;37mSERV(OUT_FALSE)\n\e[0m");
			return;
		case CHAT_ROOM_SUCCESS:
			return;
		case CHAT_ROOM_FALSE:
			printf("\n\e[1;37mSERV(CHAT_ROOM_FALSE)\n\e[0m");
			return;
		default :
			return;
	}
}

void type_block_respond(char buff[]){
	BlockUserRespond blockRespond;

	blockRespond=(*(BlockUserRespond*)buff);
	switch(blockRespond.blockResult){
		case BLOCK_SUCCESS:
			add_block(blockRespond.userNameBlock);
			printf("\n\e[1;37mSERV(BLOCK_SUCCESS)\n\e[0m");
			break;
		case BLOCK_USER_NOT_EXISTED:
			printf("\n\e[1;37mSERV(BLOCK_USER_NOT_EXISTED)\n\e[0m");
			break;
		case BLOCK_USER_BLOCKING:
			printf("\n\e[1;37mSERV(BLOCK_USER_BLOCKING)\n\e[0m");
			break;
		case BLOCK_YOU:
			printf("\n\e[1;37mSERV(BLOCK_YOU)\n\e[0m");
			break;
		case UNBLOCK_SUCCESS:
			printf("\n\e[1;37mSERV(UNBLOCK_SUCCESS)\n\e[0m");
			del_block(blockRespond.userNameBlock);
			break;
		case UNBLOCK_USER_NOT_BLOCKED:
			printf("\n\e[1;37mSERV(UNBLOCK_USER_NOT_BLOCKED)\n\e[0m");
			break;
		case UNBLOCK_USER_NOT_EXISTED:
			printf("\n\e[1;37mSERV(UNBLOCK_USER_NOT_EXISTED)\n\e[0m");
			break;
		case UNBLOCK_USER_UNAVAILABLE:
			printf("\n\e[1;37mSER(UNBLOCK_USER_UNAVAILABLE)\n\e[0m");
			break;
		case UNBLOCK_YOU:
			printf("\n\e[1;37mSER(UNBLOCK_YOU)\n\e[0m");
			break;
		default :
			break;
	}
}

void notification(char buff[]){
	UserChangeStatusRespond statusRespond;
	int i=0;

	statusRespond=(*(UserChangeStatusRespond*)buff);
	switch(statusRespond.userStatus){
		case ONLINE:
			printf("\n\e[1;37mSERV: \e[1;33m%s  \e[1;37monline\n\e[0m",statusRespond.userName);
			add_partner(statusRespond.userName);
			break;
		case OFFLINE :
			printf("\n\e[1;37mSERV: \e[1;33m%s \e[1;37moffline\n\e[0m",statusRespond.userName);
			del_partner(statusRespond.userName);
			if(strcmp(statusRespond.userName,currenUserName)==0){
				format_string(currenUserName);
			}
			break;
		case ADD:
			while(1){
				if(strcmp(statusRespond.userName,roomList[i].roomName)==0){
					roomList[i].numberUser++;
					break;
				}
				i++;
			}
			break;
		case SUB:
			while(1){
				if(strcmp(statusRespond.userName,roomList[i].roomName)==0){
					roomList[i].numberUser--;
					break;
				}
				i++;
			}
			break;
		default:
			break;
	}
}

//----------dieu huong khi xac dinh dc kieu cua respond
int choose_room(){
	char roomName[LEN];
	RoomRequest roomRequest;
	
	roomRequest.typeRequest=ROOM_REQUEST;
	roomRequest.roomType=JOIN_ROOM;
	while(1){
		show_room_list();
		printf("\n\e[0;34mEnter Room Name Want Join:\e[0m");
		fflush(stdout);
		wait_char(roomName);
		if(strcmp(roomName,"q")==0) return 0;
		if(strcmp(roomName,"")!=0 ){
			if(check_currRoom(roomName)==1) break;
		}
	}
	strcpy(roomRequest.roomName,roomName);
	send_request(&roomRequest);
	return 1;
}

int choose_user(){
	char userName[LEN];
	int t;
	
	while(1){
		t=show_user_list();
		if(t==1){
			printf("\n\e[0;34mEnter User Name Want Chat With: \e[0m");
			fflush(stdout);
			wait_char(userName);
			if(strcmp(userName,"q")==0) return 0;
			if(strcmp(userName,"")!=0 ){
				strcpy(currenUserName,userName);
				if(check_currUserName(currenUserName)==1) break;
			}
		}else return 0;
	}
	return 1;	
}

int chatting_room(){
	char buff[LEN];

	if(check_currRoom(currenRoom)==0) {
		printf("\n\e[1;31mYou Not In Room\n\e[0m");
		return 0;
	}
	printf("\n\e[0;34mYou are chat in \e[1;36m%s\n\e[0m",currenRoom);
	do{
		fflush(stdout);
		wait_char(buff);
		if(strcmp(buff,"")!=0){
				if(strcmp(buff,"\\q")==0 || strcmp(buff,"\\Q")==0){
					out_room();
					return 1;
				}else
				if(strcmp(buff,"\\r")==0 || strcmp(buff,"\\R")==0){
					reply();
				}else
				if(strcmp(buff,"\\l")==0 || strcmp(buff,"\\L")==0){
		
				}else
				if(strcmp(buff,"\\m")==0 || strcmp(buff,"\\M")==0){
					return 1;
				}else{
					if(send_chat_room(buff,CHAT_ROOM_REQUEST)==0) return 0;
				}
			}
	}while(1);
}

int chatting(){
	char buff[LEN];

						
	if(check_currUserName(currenUserName)==0){
		printf("\n\e[1;31mYou No Chat With Someone\e[0m");
		return 0;
	}
	printf("\n\e[0;34mYou are chat with \e[1;33m%s\n\e[0m",currenUserName);
	do{
		fflush(stdout);
		wait_char(buff);
		if(strcmp(buff,"")!=0){
				if(strcmp(buff,"\\m")==0 || strcmp(buff,"\\m")==0){
					return 1;
				}else
				if(strcmp(buff,"\\r")==0 || strcmp(buff,"\\R")==0){
					reply();
				}else
				if(strcmp(buff,"\\h")==0 || strcmp(buff,"\\H")==0){
					send_chat(buff,CHAT_LOG_REQUEST);
				}else{
					if(send_chat(buff,CHAT_FRIEND_SEND)==0) return 0;
				}
			}
	}while(1);
}

int out_room(){
	RoomRequest roomRequest;

	roomRequest.typeRequest=ROOM_REQUEST;
	roomRequest.roomType=OUT_ROOM;
	strcpy(roomRequest.roomName,currenRoom);
	send_request(&roomRequest);
	format_string(currenRoom);
}

int block_user(){
	char userName[LEN];
	BlockUserRequest request;
	int t;

	request.typeRequest=BLOCK_REQUEST;
	request.blockType=BLOCK;
	while(1){
		t=show_user_list();
		if(t==1){
			printf("\n\e[1;33mEnter UserName Want Block: \e[0m");
			fflush(stdout);
			wait_char(userName);
			if(strcmp(userName,"q")==0) return 0;
			if(check_currUserName(userName)==1){
				strcpy(request.blockUserName,userName);
				send_request(&request);
				return 1;
			}
		}else return 0;
	}
	return 1;
}

int un_block_user(){
	char userName[LEN];
	BlockUserRequest request;

	request.typeRequest=BLOCK_REQUEST;
	request.blockType=UNBLOCK;
	while(1){
		if(show_block_list()==1){
			printf("\n\e[0;34mEnter UserName Want UnBlock: \e[0m");
			fflush(stdout);
			wait_char(userName);
			if(strcmp(userName,"q")==0) return 0;
			if(check_blockUserName(userName)==1){
				strcpy(request.blockUserName,userName);
				send_request(&request);
				return 1;
			}
		}else return 0;
	}
}

//--------------------------------------

int wait_char(char buff[LEN]){
	int rv;
	int size_recv;
	char mesg[LEN];
	fd_set readSet;
	struct timeval tv;
	Respond respond;

	format_string(buff);
	while(1){
		tv.tv_sec = 0;
		tv.tv_usec =0;
		FD_SET(currentSockFD, &readSet);
		FD_SET(fileno(stdin), &readSet);
		
		select(currentSockFD +1, &readSet, NULL, NULL, &tv);
		if (FD_ISSET(fileno(stdin), &readSet)){
			getString(buff);
			fflush(stdin);
			if(strcmp(buff,"q")==0) return 2;
			else return 0;
		}
		if(FD_ISSET(currentSockFD, &readSet)){
			strcpy(mesg,"");
			size_recv = recv(currentSockFD,mesg,LEN,0);
			if(size_recv == 0){
				printf("\n\e[1;31mServer Disconnect\n\e[0m");
				close(currentSockFD);
				exit(0);
			}
			if (size_recv < 0) {
				printf("\e[1;31mError\n\e[0m");
				close(currentSockFD);
				exit(0);
			}
			if(size_recv > 0){
				check_respond(mesg);
			}
			format_string(buff);
			fflush(stdin);
			return 1;
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
				printf("\n\e[1;31mServer disconnect\n\e[0m");
				close(currentSockFD);
				exit(0);
			}
			if (size_recv < 0) {
				printf("\e[1;31mError\n\e[0m");
				close(currentSockFD);
				exit(0);
			}
			if(size_recv > 0){
				check_respond(mesg);
			}
			return -1;
		}
		 
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
		printf("\n\e[0;34mUserName: \e[0m");
		strcpy(tk,"");
		fgets(tk,LEN,stdin);
		tk[strlen(tk)-1]='\0';
		if(check_input(tk) == 0){
			printf("\n\e[0;34mTry Again!!\e[0m");
		}else break;
	}
	strcpy(user.userName,tk);
	while(1){
		printf("\e[0;34mPassword: \e[0m");
		get_pass(pass);
		if(check_input(pass) == 0){
			printf("\n\e[1;31mTry Again!!\n\e[0m");
		}else break;
	}
	strcpy(user.password,pass);
	user.typeRequest=LOGIN_REQUEST;
	send_request(&user);
	strcpy(mesg,"");
	recv(currentSockFD, mesg, LEN, 0);
	respond=(*(Respond*)mesg);
	if(check_type(respond) == LOGIN_RESPOND){
		loginRespond=(*(LoginRespond*)mesg);
		if(loginRespond.loginResult == LOGIN_SUCCESS){
			printf("\n%s\n",loginRespond.messenger);
			return 1;
		}else if(loginRespond.loginResult == LOGIN_INVALID_USERNAME){
			printf("\n%s\n",loginRespond.messenger);
			return 0;
		}else if(loginRespond.loginResult == LOGIN_INVALID_PASSWORD){
			printf("\n%s\n",loginRespond.messenger);
			return 0;
		}else if(loginRespond.loginResult == LOGIN_ONLINING){
			printf("\n%s\n",loginRespond.messenger);
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
		printf("\n\e[0;34mUserName: \e[0m");
		strcpy(tk,"");
		fgets(tk,LEN,stdin);
		tk[strlen(tk)-1]='\0';
		if(check_input(tk) == 0){
			printf("\n\e[1;31mTry Again!!\e[0m");
		}else break;
	}
	strcpy(user.userName,tk);
	strcpy(loginRequest.userName,tk);
	while(1){
		printf("\e[0;34mPassword: \e[0m");
		get_pass(pass);
		if(check_input(pass) == 0){
			printf("\n\e[1;31mTry Again!!\n\e[0m");
		}else break;
	}
	while(1){
		printf("\n\e[0;34mRe-Enter Password: \e[0m");
		get_pass(comf_pass);
		if(strcmp(pass,comf_pass)==0) break;
	}
	strcpy(user.password,pass);
	strcpy(loginRequest.password,pass);
	strcpy(user.passwordConfirm,comf_pass);
	user.typeRequest=REGISTER_REQUEST;
	loginRequest.typeRequest=LOGIN_REQUEST;
	send_request(&user);
	strcpy(mesg,"");
	recv(currentSockFD, mesg, LEN, 0);
	respond=(*(Respond*)mesg);
	if(respond.typeRespond == REGISTER_RESPOND){
		registerRespond=(*(RegisterRespond*)mesg);
		if(registerRespond.registerResult == REGISTER_SUCCESS){
			printf("\n%s",registerRespond.messenger);
			send_request(&loginRequest);
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
	send_request(&request);
	return 1;
}

void menu(){

	int choose;
	do{	
		printf("\e[0;34m");
		printf("\n***WAIT USER OR CHOOSE***\n");
		printf("\n1.SHOW ONLINE USER LIST");
		printf("\n2.CHOOSE USER");
		printf("\n3.COMBACK CHAT FRIEND");
		printf("\n4.SHOW ROOM LIST");		
		printf("\n5.CHOOSE ROOM");
		printf("\n6.COMBACK ROOM");
		printf("\n7.SHOW BLOCK LIST");
		printf("\n8.BLOCK USER");
		printf("\n9.UNBLOCK USER");
		printf("\n10.LOG OUT\n");
		printf("\e[0m");
		fflush(stdout);
		fflush(stdin);
		choose=wait_int();
		fflush(stdin);
		if(choose > 0){
			switch(choose){
				case 1 :
					printf("\nYou choose 1");
					show_user_list();
					break;
				case 2 :
					printf("\nYou choose 2");
					choose_user();
					if(check_currUserName(currenUserName)==1){
						chatting();
					}
					break;
				case 3 :
					printf("\nYou choose 3");
					chatting();
					break;
				case 4 :
					printf("\nYou choose 4");
					show_room_list();
					break ;
				case 5 :
					printf("\nYou choose 5");
					choose_room();
					break;
				case 6 :
					printf("\nYou choose 6");
					chatting_room();
					break;
				case 7 :
					printf("\nYou choose 7");
					show_block_list();
					break;
				case 8 :
					printf("\nYou choose 8");
					block_user();					
					break;
				case 9 :
					printf("\nYou choose 9");
					un_block_user();
					break;
				case 10:
					printf("\nYou choose 10");
					log_out();
					return ;
				default :
					printf("\nCHOOSE FALSE! AGAIN\n" );
					break;
			}
		}
	}while(1);
}


