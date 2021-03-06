#include "my_socket.h"

#define MAX_LENGTH_USERNAME 15
#define MIN_LENGTH_USERNAME 6
#define MAX_LENGTH_PASSWORD 15
#define MIN_LENGTH_PASSWORD 6
#define SIZE_BLOCK_REQUEST 204
#define SIZE_BLOCK_RESPOND 204

char key[] = { ' ', '\n', '\t', 0 };

#define OPEN_MAX  50
#define MAX_USER  10
#define BACKLOG  50
#define MAX_ROOM 10

int currentSockFD = -1;
int numUserRegisted = 0;
struct pollfd clients[OPEN_MAX];
int maxIndex;
User userRegisted[OPEN_MAX];
Room rooms[MAX_ROOM];

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
Block readBlockList(char * userBlocker);
int writeBlockList(Block blockList,char * userBlocker);
int check_block(char userBlocker[],char userBlocked[]);
void handleBlockUserRequest(BlockUserRequest request);
void handleUnBlockUserRequest(BlockUserRequest request);
void sendGetBlockListRespond();
void handleGetBlockListRespond();

int main() {
	const int PORT = 5500;
	int listenFD, clientFD, sockFD;
	socklen_t clientLen ;
	int nReady, i,  size;
	struct sockaddr_in  clientAddr;
	char buff[sizeof(Request)+1];

	numUserRegisted = readUsersFile("data.txt");
	for (i = 0; i < numUserRegisted; ++i) {
		userRegisted[i].status = OFFLINE;
	}

	listenFD = initConnect(PORT);

	clients[0].fd = listenFD;
	clients[0].events = POLLRDNORM;
	for (i = 1; i < OPEN_MAX; ++i) {
		clients[i].fd = -1;
	}

	maxIndex = 0;

	initRoom(rooms, MAX_ROOM);

	while(1) {
		nReady = poll(clients, maxIndex + 1, 0);
		if (clients[0].revents & POLLRDNORM) {
			clientLen = sizeof (clientAddr);
			clientFD = accept(listenFD, (struct sockaddr *) &clientAddr, &clientLen);
			printf("Client : %d connected", clientFD);
			
			for (i = 1; i < OPEN_MAX; ++i) {
				if (clients[i].fd < 0) {
					clients[i].fd = clientFD;
					break;
				}
			}

			if (i == OPEN_MAX) {
				myERROR("Too many clients");
			}
			clients[i].events = POLLRDNORM;
			if (i > maxIndex) {
				maxIndex = i;
			}
			if (--nReady <= 0){
				continue;
			}
		}

		for (i = 1; i <= maxIndex; ++i) {
			sockFD = clients[i].fd;
			if (sockFD < 0) {
				continue;
			}
			if (clients[i].revents & (POLLRDNORM | POLLWRNORM)) {
					printf("have signal %d\n",clients[i].fd);
					setCurrentSockFD(sockFD);
					size = recv(currentSockFD, buff, SIZE_BLOCK_REQUEST, 0);
					buff[size]='\0';
					if (size < 0) {
						if (errno == ECONNRESET) {
                      		if (userRegisted[findUserIndexWithSockFD(currentSockFD)].status == ONLINE) {
								printf("User is online\n");
								handleClientDisconnect(currentSockFD);
							}
							clients[i].fd = -1;

                   		} else {
                   			printf("error_sys\n");
                   			exit(0);
                   		}
                        	 
					} else if (size == 0) {
						if (userRegisted[findUserIndexWithSockFD(currentSockFD)].status == ONLINE) {
							handleClientDisconnect(currentSockFD);
						}
						clients[i].fd = -1;
					} else {
						printf("recognizeRequest\n");
						recognizeRequest(buff);
					}
			}
		}
	}
	return 0;
}

int initConnect(const int PORT) {
	int sockFD;
	struct sockaddr_in serverAddr;

	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	sockFD = socket(AF_INET, SOCK_STREAM, 0);

	if (sockFD < 0) {
		myERROR("Can't create new listener Socket");
	}
	if (bind(sockFD, (struct sockaddr* ) &serverAddr, sizeof serverAddr) < 0) {
		close(sockFD);
		myERROR("ERROR while binding!");
	}
	if (listen(sockFD, BACKLOG) == -1) {
		close(sockFD);
		myERROR("ERROR while listening from Server");
	}
	printf("Waiting for client\n");

	return sockFD;
}

int findUserIndexWithSockFD(int sockFD) {
	int i = 0;

	for (i = 0; i < numUserRegisted; ++i) {
		if (userRegisted[i].sockFD == sockFD)
			return i;
	}

	return -1;
}

int findUserIndex(char * userName, User* userRegisted, int numUserRegisted) {
	int i = 0;

	for (i = 0; i < numUserRegisted; ++i) {
		if (strcmp(userName, userRegisted[i].userName) == 0)
			return i; 
	}
	return -1;
}

void setCurrentSockFD(int sockFD) {
	currentSockFD = sockFD;
}

int readUsersFile(char * fileName) {
	FILE *f;
	char userName[20],password[20];
	int i = 0;

	f = fopen(fileName, "r");
	if (f == NULL) {
		printf("File %s not exist!\n", fileName);
		exit(0);
	}

	while (fscanf(f, "%s", userName) > 0) {
		strcpy(userRegisted[i].userName, userName);
		fgetc(f);
		fscanf(f, "%s", password);
		strcpy(userRegisted[i].password, password);
		++i;
	}

	fclose(f);

	return i;
}

void writeUsersFile(char *fileName, User user) {
	FILE *f;

	f = fopen(fileName, "r+");
	if (f == NULL) {
		printf("File %s not exist\n", fileName);
		exit(0);
	}
	fseek(f, 0, SEEK_END);
	fputc(10,f);
	fprintf(f, "%s %s", user.userName, user.password);
	fclose(f);
}

void makeUser(User* user, char* userName, char * password) {
	strcpy(user->userName, userName);
	strcpy(user->password, password);
}

int sendRespond(void * respond) {
	return send(currentSockFD, (char* )respond, SIZE_BLOCK_RESPOND, 0);
}

int checkValidPassword(char* password) {
	if (strlen(password) < 6 || strlen(password) > 15) {
		return FALSE;
	}
	if (checkWhiteSpace(password)) {
		return FALSE;
	}
	return TRUE;
}

int checkValidUserName(char* userName) {
 	return checkValidPassword(userName);
}

int addUser(User user, User userRegisted[], int numUserRegisted) {
	userRegisted[numUserRegisted] = user;
	return ++numUserRegisted;
}

/////////////////////////////LOG/////////////////////////
int find_file_name(char* userNameSend, char* userNameRecv,char fileName[]){
	char fileName1[35];
	char fileName2[35];
	FILE *f;

	strcpy(fileName1,"DataLog/");
	strcpy(fileName2,"DataLog/");
	strcat(fileName1,userNameSend);
	strcat(fileName1,"_");
	strcat(fileName1,userNameRecv);
	strcat(fileName1,".dat");
	strcat(fileName2,userNameRecv);
	strcat(fileName2,"_");
	strcat(fileName2,userNameSend);
	strcat(fileName2,".dat");

	f=fopen(fileName1,"rb");
	if(f != NULL){
		strcpy(fileName,fileName1);
		fclose(f);
		return 1;
	}else {
		f=fopen(fileName2,"rb");
		if(f != NULL){
			strcpy(fileName,fileName2);
			fclose(f);
			return 1;
		}else{
			strcpy(fileName,fileName1);
			return 1;
			
		}
	}
	return 0;
}

int write_log(ChatRequest chatRequest){
	char userNameSend[15];
	char userNameRecv[15];
	char fileName[35];
	time_t rawtime;
  	struct tm * timeinfo;
  	Log chatLog;
  	FILE *f;
  	int userIndex,t;


  	userIndex =  findUserIndexWithSockFD(currentSockFD);
	strcpy(userNameSend,userRegisted[userIndex].userName);
	strcpy(userNameRecv,chatRequest.userNameReceiver);
	t=find_file_name(userNameSend,userNameRecv,fileName);
  	if(t==1){
  		f=fopen(fileName,"ab");
  		fseek(f,0,SEEK_SET);
  		time ( &rawtime );
  	  	timeinfo = localtime ( &rawtime );
  	  	strcpy(chatLog.time,asctime (timeinfo));
  	  	chatLog.time[strlen(chatLog.time)-1]='\0';
  		strcpy(chatLog.userNameSend,userNameSend);
  		strcpy(chatLog.constan,chatRequest.messenger);
  		fwrite(&chatLog,sizeof(Log),1,f);
  		fclose(f);
  		return 1;
  	}else return 0;
}

int get_max_log(ChatRequest chatRequest){
	char userNameSend[15];
	char userNameRecv[15];
	char fileName[35];
	Log chatLog;
  	FILE *f;
  	int userIndex,i=0;

  	userIndex =  findUserIndexWithSockFD(currentSockFD);
	strcpy(userNameSend,userRegisted[userIndex].userName);
	strcpy(userNameRecv,chatRequest.userNameReceiver);
  	find_file_name(userNameSend,userNameRecv,fileName);
  	while(1){
  		f=fopen(fileName,"rb");
  		if(fseek(f,i*sizeof(Log),SEEK_SET)==-1){
			printf("\nfseek error");
			break;
		}
		fread(&chatLog,sizeof(Log),1,f);
		if(feof(f)){
			break;
		}else{
			fclose(f);
			i++;
		}
  	}
  	return i;
}

int get_log(ChatRequest chatRequest,char *buff,int i){
  	char userNameSend[15];
	char userNameRecv[15];
	char fileName[35];
  	FILE *f;
  	int userIndex;
  	Log chatLog;

  	
  	userIndex =  findUserIndexWithSockFD(currentSockFD);
	strcpy(userNameSend,userRegisted[userIndex].userName);
	strcpy(userNameRecv,chatRequest.userNameReceiver);
  	find_file_name(userNameSend,userNameRecv,fileName);
  	f = fopen(fileName,"rb");
	if(fseek(f,i*sizeof(Log),SEEK_SET)==-1){
		printf("\nfseek error");
		fclose(f);
		return 0;
	}
	fread(&chatLog,sizeof(Log),1,f);
	if(feof(f)){
		fclose(f);
		return 0;
	}else{
		fclose(f);
		strcpy(buff,chatLog.time);
		strcat(buff,"-");
		strcat(buff,chatLog.userNameSend);
		strcat(buff,"-");
		strcat(buff,chatLog.constan);
		return 1;
	}
}


//-----------------------------BLOCK-----------------------------------------------

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
		blockList.numberBlock=0;
		return blockList;
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

int writeBlockList(Block blockList,char * userBlocker){
	FILE *f;
	int i = 0;
	char fileName[50];

	strcpy(fileName,"DataBlock/");
	strcat(fileName,userBlocker);
	strcat(fileName,"_block.txt");
	f = fopen(fileName, "w");
	if (f == NULL) {
		printf("Not enought memory\n");
		return 0;
	}
	for (i = 0; i < blockList.numberBlock; i++) {
		fprintf(f, "%s\n", blockList.userList[i]);
	}
	fclose(f);
	return 1;
}

int check_block(char userBlocker[],char userBlocked[]){
	
	int i;
	Block blockList1,blockList2;
	blockList1=readBlockList(userBlocker);
	if(blockList1.numberBlock > 0){
		for(i=0;i<blockList1.numberBlock;i++){
			if(strcmp(blockList1.userList[i],userBlocked)==0) return 1;
		}
	}else{
		blockList2=readBlockList(userBlocked);
		if(blockList2.numberBlock>0){
			for(i=0;i<blockList2.numberBlock;i++){
				if(strcmp(blockList2.userList[i],userBlocker)==0) return 2;
			}
		}
	}
	return 0;
}

void handleBlockUserRequest(BlockUserRequest request){
	int userIndex = -1;
	char userBlocked[20];
	char userBlocker[20];
	int socketBlocked;
	Block blockList;
	BlockUserRespond respond;
	UserChangeStatusRespond statusRespond;
	int t;

	respond.typeRespond = BLOCK_RESPOND;
	strcpy(respond.userNameBlock,userBlocked);	
	statusRespond.typeRespond=USER_CHANGE_STATUS_RESPOND;
	statusRespond.userStatus=OFFLINE;
	userIndex = findUserIndexWithSockFD(currentSockFD);
	strcpy(userBlocked,request.blockUserName);
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
		blockList.numberBlock++;
		if(writeBlockList(blockList,userBlocker)==1){
		
			strcpy(respond.userNameBlock,userBlocked);
			respond.blockResult = BLOCK_SUCCESS;
			sendRespond(&respond);
			sendGetBlockListRespond();
	
			strcpy(statusRespond.userName,userBlocked);
			sendRespond(&statusRespond);
	
			socketBlocked=findUserIndex(userBlocked, userRegisted, numUserRegisted);
			setCurrentSockFD(userRegisted[socketBlocked].sockFD);
			strcpy(statusRespond.userName,userBlocker);
			sendRespond(&statusRespond);
			return;
		}
	}
}

void handleUnBlockUserRequest(BlockUserRequest request){
	int userIndex = -1;
	char userBlocked[20];
	char userBlocker[20];
	int socketBlocked;
	Block blockList;
	BlockUserRespond respond;
	UserChangeStatusRespond statusRespond;
	int t,i;

	strcpy(respond.userNameBlock,userBlocked);	
	respond.typeRespond = BLOCK_RESPOND;	
	statusRespond.typeRespond=USER_CHANGE_STATUS_RESPOND;
	statusRespond.userStatus=ONLINE;
	
	strcpy(userBlocked,request.blockUserName);
	
	
	userIndex = findUserIndexWithSockFD(currentSockFD);
	//socketBlocker=currentSockFD;
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
		blockList.numberBlock--;
		if(writeBlockList(blockList,userBlocker)==1){
		
			respond.blockResult=UNBLOCK_SUCCESS;
			strcpy(respond.userNameBlock,userBlocked);
			sendRespond(&respond);
			//sendGetBlockListRespond();
	
			socketBlocked=findUserIndex(userBlocked, userRegisted, numUserRegisted);
			if(userRegisted[socketBlocked].status==ONLINE){
				strcpy(statusRespond.userName,userBlocked);
				sendRespond(&statusRespond);
			
				setCurrentSockFD(userRegisted[socketBlocked].sockFD);
				strcpy(statusRespond.userName,userBlocker);
				sendRespond(&statusRespond);
			}
			return;
		}
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

//---------------------NOTIFY CHANGE STATUS------------------------------------------------

void notifyChangeStatusAll(char* userName, UserStatus status) {
	int i = 0;
	UserChangeStatusRespond changeStatusRespond;

	changeStatusRespond.typeRespond = USER_CHANGE_STATUS_RESPOND;
	changeStatusRespond.userStatus = status;
	strcpy(changeStatusRespond.userName,userName);
	for (i = 0; i < numUserRegisted; ++i) {
		if (userRegisted[i].status == ONLINE && strcmp(userRegisted[i].userName, userName) != 0 && check_block(userName,userRegisted[i].userName) == 0) {
			setCurrentSockFD(userRegisted[i].sockFD);
			sendRespond(&changeStatusRespond);
			printf("Send %s change status\n", userName);
		}
	}
}

void setOnline(int userIndex) {
	printf("Set user online\n");
	userRegisted[userIndex].status = ONLINE;
	notifyChangeStatusAll(userRegisted[userIndex].userName, ONLINE);
}

void setOffline(int userIndex) {
	int i = 0;
	int sockTemp = currentSockFD;

	printf("Set offline user\n");
	userRegisted[userIndex].status = OFFLINE;
	for (i = 0; i < MAX_ROOM; ++i) {
		if (findIndexUserInRoom(rooms[i], userRegisted[userIndex].userName) != -1) {
			handleRoomOut(rooms[i].roomName);
		}
	}
	currentSockFD = sockTemp;
	notifyChangeStatusAll(userRegisted[userIndex].userName, OFFLINE);
}

//read, write file
void handleClientDisconnect(int sockFD) {
	int userIndex = -1;

	userIndex = findUserIndexWithSockFD(sockFD);
	if (userIndex < 0 || userIndex >= numUserRegisted)
		return;
	setOffline(userIndex);
	close(sockFD);
}
//--------------------------------------------------------------------------

//------------HANDLE REGISTER----------------------------------------------

int sendRegisterRespond(RegisterResult registerResult, char* messenger) {
	RegisterRespond registerRespond;

	registerRespond.typeRespond = REGISTER_RESPOND;
	registerRespond.registerResult = registerResult;
	strcpy(registerRespond.messenger, messenger);

	return sendRespond(&registerRespond);
}

void handleRegisterRequest(RegisterRequest registerRequest) {
	User user;
	int userIndex = -1;

	//Tranform to RegisterRequest
	printf("User Regist\n");

	//Check valid
	if (strcmp(registerRequest.password, registerRequest.passwordConfirm) != 0) {
		sendRegisterRespond(REGISTER_INVALID_PASSWORD, "Password != password Confirm ");
		return;
	}

	if (!checkValidPassword(registerRequest.password)) {
		sendRegisterRespond(REGISTER_INVALID_PASSWORD, "Password must 6 ~ 15 and have no white space!");
		return;
	}

	if (!checkValidUserName(registerRequest.userName)) {
		sendRegisterRespond(REGISTER_INVALID_USERNAME, "User Name must 6 ~ 15 and have no white space!");
		return;	
	}

	//make User with username + pasword
	makeUser(&user, registerRequest.userName, registerRequest.password);
	userIndex = findUserIndex(user.userName, userRegisted,numUserRegisted);

	if (userIndex >= 0 && userIndex < numUserRegisted) {
		sendRegisterRespond(REGISTER_USER_EXISTED, "User existed!");
		return;
	}


	sendRegisterRespond(REGISTER_SUCCESS, "Register success");
	//update status
	setOffline(userIndex);
	user.sockFD = -1;
	numUserRegisted =  addUser(user, userRegisted, numUserRegisted);

	//write to file
	writeUsersFile("data.txt",user);
}

//--------------------------------------------------------------------------

//---------------------HANDLE LOGIN-----------------------------------------
int sendLoginRespond(LoginResult loginResult, char* messenger) {
	LoginRespond loginRespond;

	loginRespond.typeRespond = LOGIN_RESPOND;
	loginRespond.loginResult = loginResult;
	strcpy(loginRespond.messenger, messenger);

	return sendRespond(&loginRespond);
}

void handleLoginRequest(LoginRequest loginRequest) {
	int userIndex = -1;

	printf("User Login!\n");
	userIndex = findUserIndex(loginRequest.userName, userRegisted,numUserRegisted);
	if (userIndex < 0) {
		sendLoginRespond(LOGIN_INVALID_USERNAME,"User name not found!");
		return;
	}
	if (strcmp(loginRequest.password, userRegisted[userIndex].password) != 0) {
		sendLoginRespond(LOGIN_INVALID_PASSWORD, "Password invalid!");
		return;
	}
	if (userRegisted[userIndex].status == ONLINE) {
		sendLoginRespond(LOGIN_ONLINING, "User is onlining!");
		return;
	}
	
	userRegisted[userIndex].sockFD = currentSockFD;
	printf("Client %s login success\n", loginRequest.userName);
	sendLoginRespond(LOGIN_SUCCESS, "Login success");
	setOnline(userIndex);
}
//--------------------------------------------------------------------------

void handleLogoutRequest() {
	int userIndex = -1;

	userIndex = findUserIndexWithSockFD(currentSockFD);
	setOffline(userIndex);
}

///---------------------CHAT FRIEND------------------------
/////////////////////////chat log request//
void handleChatLogRequest(ChatRequest chatRequest){
	ChatRespond chatRespond;
	int i=0,max_log;

	chatRespond.typeRespond=CHAT_RESPOND;
	chatRespond.chatResult=CHAT_LOG_RESPOND;
	strcpy(chatRespond.userNameSender,"");
	max_log=get_max_log(chatRequest);
	max_log--;
	while(1){
		if(i==10 || max_log<0) break;
		if(get_log(chatRequest,chatRespond.messenger,max_log)==0) break;
		printf("%s\n",chatRespond.messenger );
		sendRespond(&chatRespond);
		max_log--;
		i++;
	}
}


///////////////////////
void handleChatWithFriendRequest(ChatRequest chatRequest) {
	int indexSender = -1;
	int indexReceiver = -1;
	ChatRespond chatRespond;
	User user;

	printf("Chat handle\n");
	chatRespond.typeRespond = CHAT_RESPOND;
	indexSender = findUserIndexWithSockFD(currentSockFD);
	
	if(chatRequest.chatType==CHAT_FRIEND_SEND){
		strcpy(chatRespond.userNameSender, userRegisted[indexSender].userName);
		strcpy(user.userName, chatRequest.userNameReceiver);
		indexReceiver = findUserIndex(user.userName, userRegisted, numUserRegisted);
		if(check_block(chatRespond.userNameSender,chatRequest.userNameReceiver) != 0){
			chatRespond.chatResult = CHAT_USER_BLOCK;
			sendRespond(&chatRespond);
			return;
		}
	
		if (indexReceiver == -1) {
			chatRespond.chatResult = CHAT_USER_NOT_EXISTED;
			sendRespond(&chatRespond);
			return;
		}
	
		if (userRegisted[indexReceiver].status == OFFLINE) {
			chatRespond.chatResult = CHAT_USER_OFFLINE;
			sendRespond(&chatRespond);
			return;
		}

		chatRespond.chatResult = CHAT_SUCCESS;
		sendRespond(&chatRespond);
	
	////log
		write_log(chatRequest);

	///////////////	
		chatRespond.chatResult = CHAT_FRIEND_RECV;
		strcpy(chatRespond.messenger, chatRequest.messenger);
		strcpy(chatRespond.userNameSender,userRegisted[indexSender].userName);
		setCurrentSockFD(userRegisted[indexReceiver].sockFD);	
		sendRespond(&chatRespond);
	
		
	}else if(chatRequest.chatType==CHAT_LOG_REQUEST){
		handleChatLogRequest(chatRequest);
	}
}

//----------------------SEND USER ONLINE---------------------------

void sendGetOnlineUserListRespond() {
	GetOnlineUserListRespond getOnlineUserListRespond;
	int i = 0;
	char onlineUserList[10][19];
	int numUsersOnline = 0;
	int userIndex = -1;

	getOnlineUserListRespond.numUsersOnline = 0;
	getOnlineUserListRespond.typeRespond = GET_ONLINE_USER_LIST_RESPOND;
	userIndex = findUserIndexWithSockFD(currentSockFD);
	printf("\nsend get ol user list");
	for (i = 0; i < numUserRegisted; ++i) {
		//--neu thang do trong block list thi next
		if(check_block(userRegisted[userIndex].userName,userRegisted[i].userName) != 0){	
				continue;
			}
		if(userRegisted[i].sockFD == currentSockFD) 
			continue;
		if (userRegisted[i].status == ONLINE) {
			strcpy(onlineUserList[numUsersOnline], userRegisted[i].userName);
			++numUsersOnline;
			if (numUsersOnline == 2) {
				memcpy(getOnlineUserListRespond.onlineUserList, onlineUserList, 190);
				getOnlineUserListRespond.numUsersOnline = numUsersOnline;
				if(i==(numUserRegisted-1)){
					getOnlineUserListRespond.getListRespondResult=END;
				}else 
					getOnlineUserListRespond.getListRespondResult=CONT;
				sendRespond(&getOnlineUserListRespond);
				numUsersOnline = 0;
			}
		}
	}

	memcpy(getOnlineUserListRespond.onlineUserList, onlineUserList, 190);
	getOnlineUserListRespond.numUsersOnline = numUsersOnline;
	getOnlineUserListRespond.getListRespondResult=END;
	sendRespond(&getOnlineUserListRespond);
}

void handleGetListOnlineUserRequest() {
	sendGetOnlineUserListRespond();
}
//------------------ROOM----------------------------

int findRoomIndex(char* roomName, Room* rooms, int numRooms) {
	int i = 0;

	for (i = 0; i < numRooms; ++i) {
		if (strcmp(roomName, rooms[i].roomName) == 0)
			return i;
	}

	return -1;
}

void initRoom(Room* rooms, int numRooms) {
	int i =0;

	strcpy(rooms[0].roomName, "Room1");
	strcpy(rooms[1].roomName, "Room2");
	strcpy(rooms[2].roomName, "Room3");
	strcpy(rooms[3].roomName, "Room4");
	strcpy(rooms[4].roomName, "Room5");
	strcpy(rooms[5].roomName, "Room6");
	strcpy(rooms[6].roomName, "Room7");
	strcpy(rooms[7].roomName, "Room8");
	strcpy(rooms[8].roomName, "Room9");
	strcpy(rooms[9].roomName, "Room10");

	for (i = 0; i < numRooms; ++i) {
		rooms[i].maxUser = 10;
		rooms[i].numberUser = 0;
	}
}

void handleGetRoomListRequest() {
	GetRoomListRespond respond;
	int i =0;

	respond.typeRespond = GET_ROOM_LIST_RESPOND;
	respond.roomNumber = MAX_ROOM;
	for (i = 0; i < MAX_ROOM; i++) {
		strcpy(respond.roomList[i], rooms[i].roomName);
		respond.numberUser[i] = rooms[i].numberUser;
	}
	sendRespond(&respond);
}

int findIndexUserInRoom(Room room, char* userName) {
	int userIndex = -1;

	for (userIndex = 0; userIndex < room.numberUser; ++userIndex) {
		if (strcmp(room.userList[userIndex], userName)==0)
			return userIndex;
	}

	return -1;
}


void removeUserInRoom(Room *room, int index) {
	int i = 0;

	for (i = index; i < room->numberUser - 1; ++i) {
		strcpy(room->userList[i], room->userList[i+1]);
	}

	--room->numberUser;
}

void sendRoomAll(char* userName, char * messenger, Room room, RoomResult roomResult) {
	int i = 0;
	int userIndex;
	RoomRespond respond; 

	respond.typeRespond = ROOM_RESPOND;
	respond.roomResult = roomResult;
	strcpy(respond.userName, userName);
	strcpy(respond.messenger, messenger);
	strcpy(respond.roomName, room.roomName);
	for (i = 0; i < room.numberUser; ++i) {
		userIndex = findUserIndex(room.userList[i], userRegisted, numUserRegisted);
		if (strcmp(userName, room.userList[i]) == 0 || check_block(userName,room.userList[i]) != 0)
			continue;

		setCurrentSockFD(userRegisted[userIndex].sockFD);
		sendRespond(&respond);
	}
}

void handleRoomJoin(RoomRequest request) {
	int userIndex;
	int roomIndex;
	int numberUser = -1;
	RoomRespond respond;

	respond.typeRespond = ROOM_RESPOND;
	strcpy(respond.roomName,request.roomName);
	userIndex = findUserIndexWithSockFD(currentSockFD);
	roomIndex = findRoomIndex(request.roomName, rooms, MAX_ROOM);
	numberUser = rooms[roomIndex].numberUser;
	if (roomIndex == -1) {
		respond.roomResult  = JOIN_FALSE;
		//strcpy(respond.roomName, rooms[roomIndex].roomName);
		strcpy(respond.messenger, "Room is not existed!");
		sendRespond(&respond);
		return ;
	}

	if (numberUser > 2) {
		respond.roomResult  = JOIN_FALSE;
		//strcpy(respond.roomName, rooms[roomIndex].roomName);
		strcpy(respond.messenger, "Room is full!");
		sendRespond(&respond);
		return;
	}

	if (findIndexUserInRoom(rooms[roomIndex], userRegisted[userIndex].userName) != -1) {
		respond.roomResult  = JOIN_FALSE;
		//strcpy(respond.roomName, rooms[roomIndex].roomName);
		strcpy(respond.messenger, "you're in this room!");
		sendRespond(&respond);
		return;	
	}

	strcpy(rooms[roomIndex].userList[numberUser],userRegisted[userIndex].userName);
	++rooms[roomIndex].numberUser;

	respond.roomResult = JOIN_SUCCESS;
	sendRespond(&respond);
	sendRoomAll(userRegisted[userIndex].userName, "", rooms[roomIndex], USER_JOIN_ROOM);
	notifyChangeStatusAll(respond.roomName,ADD);
}

void handleChatRoomRequest(RoomRequest request) {
	int userIndex;
	int roomIndex;
	RoomRespond respond;

	respond.typeRespond = ROOM_RESPOND;
	roomIndex = findRoomIndex(request.roomName, rooms, MAX_ROOM);
	userIndex = findUserIndexWithSockFD(currentSockFD);

	if (roomIndex == -1) {
		respond.roomResult = CHAT_ROOM_FALSE;
		strcpy(respond.messenger, "Room not existed!");
		sendRespond(&respond);
		return;
	}
	respond.roomResult = CHAT_ROOM_SUCCESS;
	sendRoomAll(userRegisted[userIndex].userName, request.messenger, rooms[roomIndex], CHAT_ROOM);
}

void handleRoomOut(char * roomName) {
	int userIndex;
	int roomIndex;
	int indexUserInRoom = -1;
	RoomRespond respond;

	respond.typeRespond = ROOM_RESPOND;
	strcpy(respond.roomName,roomName);
	userIndex = findUserIndexWithSockFD(currentSockFD);
	roomIndex = findRoomIndex(roomName, rooms, MAX_ROOM);
	if (roomIndex == -1) {
		printf("Room not existed!\n");
		return ;
	}

	indexUserInRoom = findIndexUserInRoom(rooms[roomIndex], userRegisted[userIndex].userName);
	if (indexUserInRoom == -1) {
		respond.roomResult  = OUT_FALSE;
		strcpy(respond.roomName, rooms[roomIndex].roomName);
		strcpy(respond.messenger, "Room not existed!");
		sendRespond(&respond);
		return;
	}
	
	respond.roomResult = OUT_SUCCESS;
	sendRespond(&respond);
	sendRoomAll(userRegisted[userIndex].userName, "", rooms[roomIndex], USER_OUT_ROOM);
	removeUserInRoom(&rooms[roomIndex], indexUserInRoom);
	notifyChangeStatusAll(respond.roomName,SUB);
}

void handleRoomRequest(RoomRequest request) {
	switch (request.roomType) {
		case JOIN_ROOM:
			handleRoomJoin(request);
			break;

		case OUT_ROOM:
			handleRoomOut(request.roomName);
			break;

		case CHAT_ROOM_REQUEST:
			handleChatRoomRequest(request);
			break;
	}
}
//-------------------------------

void recognizeRequest(char* buff) {
	Request request;
	LoginRequest loginRequest;
	RegisterRequest registerRequest;
	ChatRequest chatRequest;
	RoomRequest roomRequest;
	BlockUserRequest blockUserRequest;

	request = *((Request*) buff);
	switch(request.typeRequest) {
		case LOGIN_REQUEST:
			loginRequest = *((LoginRequest* )buff);
			handleLoginRequest(loginRequest);
			break;

		case LOGOUT_REQUEST:
			handleLogoutRequest();
			break;

		case REGISTER_REQUEST:
			registerRequest = *((RegisterRequest* )buff);
			handleRegisterRequest(registerRequest);
			break;

		case CHAT_REQUEST:
			chatRequest = *((ChatRequest* )buff);
			handleChatWithFriendRequest(chatRequest);
			break;

		case GET_ONLINE_USER_LIST_REQUEST:
			handleGetListOnlineUserRequest();
			break;

		case GET_ROOM_LIST_REQUEST:
			handleGetRoomListRequest();
			break;
		case GET_BLOCK_LIST_REQUEST:
			handleGetBlockListRespond();
			break;
		case ROOM_REQUEST:
			roomRequest = *((RoomRequest* )buff);
			handleRoomRequest(roomRequest);
			break;
		case BLOCK_REQUEST:
			blockUserRequest = *((BlockUserRequest* )buff);
			if (blockUserRequest.blockType == BLOCK){		
				handleBlockUserRequest(blockUserRequest);}
			if (blockUserRequest.blockType == UNBLOCK){
				handleUnBlockUserRequest(blockUserRequest);}
			break;
		default:
			printf("Khong nhan dang \n");
			break;

	}
}