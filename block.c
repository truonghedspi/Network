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
#include <time.h>
#include "my_socket.h"
#define LEN 204
int initConnect(const int PORT);
int findUserIndexWithSockFD(int sockFD) ;
int findUserIndex(char * userName, User* userRegisted, int numUserRegisted);
void setCurrentSockFD(int sockFD);
int readUsersFile(char * fileName);
void writeUsersFile(char *fileName, User user);
void makeUser(User* user, char* userName, char * password);
int sendRespond(void * respond);
int checkValidPassword(char* password);
int checkValidUserName(char* userName);
int addUser(User user, User userRegisted[], int numUserRegisted);
int find_file_name(char* userNameSend, char* userNameRecv,char fileName[]);
int write_log(ChatRequest chatRequest);
int get_max_log(ChatRequest chatRequest);
int get_log(ChatRequest chatRequest,char *buff,int i);
///TRUONG OC CHO
//void readBlockList(char blockList[100][50], int  * numUserBlock, char * _userName);
//void writeBlockList(char blockList[100][50], int numUserBlock, char* fileName);
//int findUserIndexInBlockList(char list[100][50], int numUserBlock, char* userName);
//void handleBlockUserRequest(BlockUserRequest request);
//void handleUnblockUserRequest(BlockUserRequest request);
void notifyChangeStatusAll(char* userName, UserStatus status);
void setOnline(int userIndex);
void setOffline(int userIndex);
void handleClientDisconnect(int sockFD);
int sendRegisterRespond(RegisterResult registerResult, char* messenger);
void handleRegisterRequest(RegisterRequest registerRequest);
int sendLoginRespond(LoginResult loginResult, char* messenger);
void handleLoginRequest(LoginRequest loginRequest);
void handleLogoutRequest();
void handleChatLogRequest(ChatRequest chatRequest);
void handleChatWithFriendRequest(ChatRequest chatRequest);
void sendGetOnlineUserListRespond();
void handleGetListOnlineUserRequest();
int findRoomIndex(char* roomName, Room* rooms, int numRooms);
void initRoom(Room* rooms, int numRooms);
void handleGetRoomListRequest();
int findIndexUserInRoom(Room room, char* userName);
void removeUserInRoom(Room *room, int index);
void sendRoomAll(char* userName, char * messenger, Room room, RoomResult roomResult);
void handleRoomJoin(RoomRequest request);
void handleChatRoomRequest(RoomRequest request);
void handleRoomOut(char * roomName);
void handleRoomRequest(RoomRequest request);
void recognizeRequest(char* buff);

Block readBlockList(char * userBlocker){
	FILE *f;
	char userName[50];
	char fileName[50];
	int i=0;
	Block blockList;

	strcpy(fileName,"DataBlock/");
	strcat(fileName, userBlocker);
	strcat(fileName, "_block.txt");
	f = fopen(fileName, "r");
	if (f == NULL) {
		return;
	}
	while(!feof(f)) {
		if(fscanf(f,"%s",userName)<0) break;
		strcpy(blockList.userList[i], userName);
		fgetc(f);
		i++;
	}
	blockList.numberBlock=i;
	fclose(f);
	return blockList;
}

void writeBlockList(Block blockList,char * userBlocker){
	FILE *f;
	int i = 0;
	char fileName[50];

	strcpy(fileName,"DataBlock/");
	strcat(fileName,userBlocker);
	strcat(fileName,"_block.txt");
	f = fopen(fileName, "w");
	if (f == NULL) {
		printf("Not enought memory\n");
		return;
	}
	for (i = 0; i < blockList.numberBlock; i++) {
		fprintf(f, "%s\n", blockList.userList[i]);
	}
	fclose(f);
}

int check_block(char userBlocker[],char userBlocked[]){
	
	int i;
	Block blockList1,blockList2;

	blockList1=readBlockList(userBlocker);
	blockList2=readBlockList(userBlocked);
	for(i=0;i<blockList1.numberBlock;i++){
		if(strcmp(blockList1.userList[i],userBlocked)==0) return 1;
	}
	for(i=0;i<blockList2.numberBlock;i++){
		if(strcmp(blockList2.userList[i],userBlocker)==0) return 2;
	}
	return 0;
}

void handleBlockUserRequest(BlockUserRequest request){
	int userIndex = -1;
	char userBlocked[20];
	char userBlocker[20];
	int socketBlocker,socketBlocked;
	Block blockList;
	BlockUserRespond respond;
	UserChangeStatusRespond statusRespond;
	int t;

	respond.typeRespond = BLOCK_RESPOND;	
	statusRespond.typeRespond=USER_CHANGE_STATUS_RESPOND;
	statusRespond.userStatus=OFFLINE;
	userIndex = findUserIndexWithSockFD(currentSockFD);
	strcpy(userBlocked,request.blockUserName);
	socketBlocked=findUserIndex(userBlocked, userRegisted, numUserRegisted);
	socketBlocker=currentSockFD;
	strcpy(userBlocker,userRegisted[userIndex].userName);

	if (strcmp(userBlocker,userBlocked) == 0) {
		respond.blockResult = BLOCK_YOU;
		strcpy(respond.messenger, "You can not block you!");
		sendRespond(&respond);
		return;
	}
	t=check_block(userBlocker,userBlocked);
	if(t==1){//blocker da block blocked
		respond.blockResult = BLOCK_USER_BLOCKING;
		strcpy(respond.messenger, "You can't block this user again");
		sendRespond(&respond);
		return;
	}else if(t==2){//blocked da block blocker
		respond.blockResult = BLOCK_USER_UNAVAILABLE;
		strcpy(respond.messenger, "This user is block you!");
		sendRespond(&respond);
		return;
	}else{//blocker chua block blocked
		blockList=readBlockList(userBlocker);
		strcpy(blockList.userList[blockList.numberBlock],userBlocked);
		writeBlockList(blockList,userBlocker);
		respond.blockResult = BLOCK_SUCCESS;
		sendRespond(&respond);

		strcpy(statusRespond.userName,userBlocked);
		sendRespond(&statusRespond);

		setCurrentSockFD(socketBlocked);
		strcpy(statusRespond.userName,userBlocker);
		sendRespond(&statusRespond);
		return;
	}
}

void handleUnBlockUserRequest(BlockUserRequest request){
	int userIndex = -1;
	char userBlocked[20];
	char userBlocker[20];
	int socketBlocker,socketBlocked;
	Block blockList;
	BlockUserRespond respond;
	UserChangeStatusRespond statusRespond;
	int t,i;

	respond.typeRespond = BLOCK_RESPOND;	
	statusRespond.typeRespond=USER_CHANGE_STATUS_RESPOND;
	statusRespond.userStatus=ONLINE;
	
	strcpy(userBlocked,request.blockUserName);
	socketBlocked=findUserIndex(userBlocked, userRegisted, numUserRegisted);
	
	userIndex = findUserIndexWithSockFD(currentSockFD);
	socketBlocker=currentSockFD;
	strcpy(userBlocker,userRegisted[userIndex].userName);
	if (strcmp(userBlocker,userBlocked) == 0) {
		respond.blockResult = UNBLOCK_YOU;
		strcpy(respond.messenger, "You can't unblock you!");
		sendRespond(&respond);
		return;
	}
	t=check_block(userBlocker,userBlocked);
	if(t==0){//2 thang chua block nhau
		respond.blockResult = UNBLOCK_USER_NOT_BLOCKED;
 		strcpy(respond.messenger, "User who you want block not blocked!");
 		sendRespond(&respond);
 		return;
	}else if(t==2){//blocker bi block boi blocked
		respond.blockResult = UNBLOCK_USER_UNAVAILABLE;
 		strcpy(respond.messenger, "This user is block you, can't ublock!");
 		sendRespond(&respond);
 		return;
	}else if( t==1){//blocked co trong danh sach block cua blocker
		blockList=readBlockList(userBlocker);
		for(i=0;i<blockList.numberBlock;i++){
			if(strcat(blockList.userList[i],userBlocked)==0)
				break;
		}
		if(i==blockList.numberBlock)
			printf("\nCo loi o day");
		while(1){
			if(i==blockList.numberBlock)
				break;
			strcpy(blockList.userList[i],blockList.userList[i+1]);
			i++;
		}
		strcpy(blockList.userList[i],"");
		writeBlockList(blockList,userBlocker);
		respond.blockResult=UNBLOCK_SUCCESS;
		sendRespond(&respond);

		strcpy(statusRespond.userName,userBlocked);
		sendRespond(&statusRespond);

		setCurrentSockFD(socketBlocked);
		strcpy(statusRespond.userName,userBlocker);
		sendRespond(&statusRespond);
		return;
	}
}

void sendGetBlockListRespond(){
	GetBlockListRespond blockListRespond;
	int i = 0;
	Block blockList;
	int userIndex = -1;
	char userBlocker[20];
	int numBlockList=0;

	blockListRespond.typeRespond=GET_BLOCK_LIST_RESPOND;
	blockListRespond.numBlockList=0;
	userIndex = findUserIndexWithSockFD(currentSockFD);
	strcpy(userBlocker,userRegisted[userIndex].userName);

	blockList=readBlockList(userBlocker);
	for(i=0;i<blockList.numberBlock;i++){
		strcpy(blockListRespond.userNameBlock[numBlockList],blockList.userList[i]);
		numBlockList++;
		if(numBlockList==2){
			blockListRespond.numBlockList=numBlockList;
			blockListRespond.getBlockRespondResult=CONT;
			sendRespond(&blockListRespond);
			numBlockList=0;
		}
	}
	blockListRespond.numBlockList=numBlockList;
	blockListRespond.getBlockRespondResult=END;
	sendRespond(&blockListRespond);
}
void handleGetBlockListRespond(){
	sendGetBlockListRespond();
}

void main(){
	int t;
	Block blockList,blockList2;

	strcpy(blockList.userList[0],"tranha");
	strcpy(blockList.userList[1],"tranha1");
	strcpy(blockList.userList[2],"tranha2");
	strcpy(blockList.userList[3],"tranha3");
	blockList.numberBlock=4;

	writeBlockList(blockList,"tranha");
	blockList2=readBlockList("tranha");
	printf("\numberBlock: %d", blockList2.numberBlock);
	printf("\nblockL: %s",blockList2.userList[0]);
	printf("\nblockL: %s",blockList2.userList[1]);
	printf("\nblockL: %s",blockList2.userList[2]);
	printf("\nblockL: %s",blockList2.userList[3]);
	t=check_block("tranha","tranha1");
	printf("\n%d",t);
}