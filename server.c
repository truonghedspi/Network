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

int currentSockFD = -1;
int numUserRegisted = 0;
struct pollfd clients[OPEN_MAX];
int maxIndex;
User userRegisted[OPEN_MAX];

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

int findUserIndex(User user, User* userRegisted, int numUserRegisted) {
	int i = 0;

	for (i = 0; i < numUserRegisted; ++i) {
		if (strcmp(user.userName, userRegisted[i].userName) == 0)
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

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

//---------------------NOTIFY CHANGE STATUS------------------------------------------------
void notifyChangeStatusAll(char* userName, UserStatus status) {
	int i = 0;
	UserChangeStatusRespond changeStatusRespond;

	changeStatusRespond.typeRespond = USER_CHANGE_STATUS_RESPOND;
	changeStatusRespond.userStatus = status;
	strcpy(changeStatusRespond.userName,userName);

	for (i = 0; i < numUserRegisted; ++i) {
		if (userRegisted[i].status == ONLINE && strcmp(userRegisted[i].userName, userName) != 0) {
			setCurrentSockFD(userRegisted[i].sockFD);
			sendRespond(&changeStatusRespond);
			printf("Send %s change status\n", userName);
		}
	}
}

void setOnline(User* user) {
	user->status = ONLINE;
	notifyChangeStatusAll(user->userName, ONLINE);
}

void setOffline(User* user) {
	user->status = OFFLINE;
	notifyChangeStatusAll(user->userName, OFFLINE);
}

//read, write file
void handleClientDisconnect(int sockFD) {
	int userIndex = -1;

	userIndex = findUserIndexWithSockFD(sockFD);
	if (userIndex < 0 || userIndex >= numUserRegisted)
		return;
	setOffline(&userRegisted[userIndex]);
	close(sockFD);
}
//--------------------------------------------------------------------------

//------------HANDLE REGISTER----------------------------------------------
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
	userIndex = findUserIndex(user, userRegisted,numUserRegisted);

	if (userIndex >= 0 && userIndex < numUserRegisted) {
		sendRegisterRespond(REGISTER_USER_EXISTED, "User existed!");
		return;
	}


	sendRegisterRespond(REGISTER_SUCCESS, "Register success");
	//update status
	setOffline(&user);
	user.sockFD = -1;
	numUserRegisted =  addUser(user, userRegisted, numUserRegisted);

	//write to file
	writeUsersFile("data.txt",user);
}

int sendRegisterRespond(RegisterResult registerResult, char* messenger) {
	RegisterRespond registerRespond;

	registerRespond.typeRespond = REGISTER_RESPOND;
	registerRespond.registerResult = registerResult;
	strcpy(registerRespond.messenger, messenger);

	return sendRespond(&registerRespond);
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
	User user;
	int userIndex = -1;

	printf("User Login!\n");
	makeUser(&user, loginRequest.userName, loginRequest.password);
	userIndex = findUserIndex(user, userRegisted,numUserRegisted);
	if (userIndex < 0) {
		sendLoginRespond(LOGIN_INVALID_USERNAME,"User name not found!");
		return;
	}
	if (strcmp(user.password, userRegisted[userIndex].password) != 0) {
		sendLoginRespond(LOGIN_INVALID_PASSWORD, "Password invalid!");
		return;
	}
	 if (userRegisted[userIndex].status == ONLINE) {
		sendLoginRespond(LOGIN_ONLINING, "User is onlining!");
		return;
	}
	
	userRegisted[userIndex].sockFD = currentSockFD;
	printf("Client %s login success\n", user.userName);
	sendLoginRespond(LOGIN_SUCCESS, "Login success");
	setOnline(&userRegisted[userIndex]);
}
//--------------------------------------------------------------------------

void handleLogoutRequest(LogoutRequest logoutRequest) {
	handleClientDisconnect(currentSockFD);
}

///---------------------CHAT FRIEDN------------------------

void handleChatWithFriendRequest(ChatRequest chatRequest) {
	int indexSender = -1;
	int indexReceiver = -1;
	ChatRespond chatRespond;
	User user;

	printf("Chat handle\n");
	chatRespond.typeRespond = CHAT_RESPOND;

	indexSender = findUserIndexWithSockFD(currentSockFD);
	strcpy(chatRespond.userNameSender, userRegisted[indexSender].userName);

	strcpy(user.userName, chatRequest.userNameReceiver);
	indexReceiver = findUserIndex(user, userRegisted, numUserRegisted);

	if (indexReceiver == -1) {
		chatRespond.chatResult = CHAT_USER_NOT_EXISTED;
		sendRespond(&chatRespond);
	}

	if (userRegisted[indexReceiver].status == OFFLINE) {
		chatRespond.chatResult = CHAT_USER_OFFLINE;
		sendRespond(&chatRespond);
	}

	chatRespond.chatResult = CHAT_SUCCESS;
	sendRespond(&chatRespond);


	chatRespond.chatResult = CHAT_FRIEND_RECV;
	strcpy(chatRespond.messenger, chatRequest.messenger);
	strcpy(chatRespond.userNameSender,userRegisted[indexReceiver].userName);
	setCurrentSockFD(userRegisted[indexReceiver].sockFD);	
	sendRespond(&chatRespond);
}

//-----------SEND USER ONLINE-------------------------------

void sendGetOnlineUserListRespond() {
	GetOnlineUserListRespond getOnlineUserListRespond;
	int i = 0;
	char onlineUserList[10][19];
	int numUsersOnline = 0;

	getOnlineUserListRespond.numUsersOnline = 0;
	getOnlineUserListRespond.typeRespond = GET_ONLINE_USER_LIST_RESPOND;
	
	for (i = 0; i < numUserRegisted; ++i) {
		if(userRegisted[i].sockFD == currentSockFD) 
			continue;
		if (userRegisted[i].status == ONLINE) {
			strcpy(onlineUserList[numUsersOnline], userRegisted[i].userName);
			++numUsersOnline;
			if (numUsersOnline == 10) {
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
//---------------------------------------------------------

void recognizeRequest(char* buff) {
	Request request;
	LoginRequest loginRequest;
	LogoutRequest logoutRequest;
	RegisterRequest registerRequest;
	ChatRequest chatRequest;
	GetOnlineUserListRequest getOnlineUserListRequet;
	//ChatRoomRequest chatRoomRequest;

	request = *((Request*) buff);
	switch(request.typeRequest) {
		case LOGIN_REQUEST:
			loginRequest = *((LoginRequest* )buff);
			handleLoginRequest(loginRequest);
			break;

		case LOGOUT_REQUEST:
			logoutRequest = *((LogoutRequest* )buff);
			handleLogoutRequest(logoutRequest);
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
			getOnlineUserListRequet = *((GetOnlineUserListRequest* )buff);
			handleGetListOnlineUserRequest();
			break;

		case CHAT_ROOM_REQUEST:
			//chatRoomRequest = *((ChatRoomRequest* )buff);
			//handleChatRoomRequest(chatRoomRequest);
			break;
	}
}

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
					printf("Co hang\n");
					setCurrentSockFD(sockFD);
					size = recv(currentSockFD, buff, SIZE_BLOCK_REQUEST, 0);
					buff[size]='\0';
					if (size < 0) {
						if (errno == ECONNRESET) {
                      		handleClientDisconnect(currentSockFD);
							clients[i].fd = -1;

                   		} else {
                   			printf("error_sys\n");
                   			exit(0);
                   		}
                        	 
					} else if (size == 0) {
						handleClientDisconnect(currentSockFD);
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