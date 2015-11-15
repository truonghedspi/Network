
#include "input.h"
#include "request.h"
#include "respond.h"
#include "my_socket.h"

#define MAX_LENGTH_USERNAME 15
#define MIN_LENGTH_USERNAME 6
#define MAX_LENGTH_PASSWORD 15
#define MIN_LENGTH_PASSWORD 6
#define SIZE_BLOCK_REQUEST 1000
#define SIZE_BLOCK_RESPOND 1000

char key[] = { ' ', '\n', '\t', 0 };
#define OPEN_MAX  50
#define MAX_USER  10
#define BACKLOG  50
int currentSockFD = -1;
int numUserRegisted = 0;
struct pollfd clients[OPEN_MAX];
int maxIndex;
User userRegisted[OPEN_MAX];

void setCurrentSockFD(int sockFD) {
	currentSockFD = sockFD;
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

//read, write file
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

int findUserIndex(User user, User* userRegisted, int numUserRegisted) {
	int i = 0;

	for (i = 0; i < numUserRegisted; ++i) {
		if (strcmp(user.userName, userRegisted[i].userName) == 0)
			return i;
	}
	return -1;
}

void makeUser(User* user, char* userName, char * password) {
	strcpy(user->userName, userName);
	strcpy(user->password, password);
}

int sendRespond(void * respond) {
	char buff[250];
	return send(currentSockFD, (char* )buff, SIZE_BLOCK_RESPOND, 0);
}

int sendLoginRespond(LoginResult loginResult, char* messenger) {
	LoginRespond loginRespond;

	loginRespond.typeRespond = LOGIN_RESPOND;
	loginRespond.loginResult = loginResult;
	strcpy(loginRespond.messenger, messenger);

	return sendRespond(&loginRespond);
}

int sendRegisterRespond(RegisterResult registerResult, char* messenger) {
	RegisterRespond registerRespond;

	registerRespond.typeRespond = REGISTER_RESPOND;
	registerRespond.registerResult = registerResult;
	strcpy(registerRespond.messenger, messenger);

	return sendRespond(&registerRespond);
}

void handleLoginRequest(Request request) {
	LoginRequest loginRequest;
	User user;
	int userIndex = -1;

	printf("User Login!\n");
	memcpy(&loginRequest, &request, SIZE_BLOCK_REQUEST);

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
	 if (userRegisted[userIndex].isOnline == TRUE) {
		sendLoginRespond(LOGIN_ONLINING, "User is onlining!");
		return;
	}

	userRegisted[userIndex].isOnline=TRUE;
	userRegisted[userIndex].sockFD = currentSockFD;
	printf("Client %s login success\n", user.userName);
	sendLoginRespond(LOGIN_SUCCESS, "Login success");
}


void handleLogoutRequest(Request request) {

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

void handleRegisterRequest(Request request) {
	RegisterRequest registerRequest;
	User user;
	int userIndex = -1;

	//Tranform to RegisterRequest
	printf("User Regist\n");
	memcpy(&registerRequest, &request, SIZE_BLOCK_REQUEST);

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
	user.isOnline = FALSE;
	user.sockFD = -1;
	numUserRegisted =  addUser(user, userRegisted, numUserRegisted);

	//write to file
	writeUsersFile("data.txt",user);
}

void handleChatWithFriendRequest(Request request) {

}

void sendGetOnlineUserListRespond() {
	GetOnlineUserListRespond getOnlineUserListRespond;
	int i = 0, numUsersOnline = 0;
	char onlineUserList[10][20];

	getOnlineUserListRespond.typeRespond = GET_ONLINE_USER_LIST_RESPOND;
	
	for (i = 0; i < numUserRegisted; ++i) {
		if (userRegisted[i].isOnline) {
			strcpy(onlineUserList[numUsersOnline], userRegisted[i].userName);
			++numUsersOnline;
			if (numUsersOnline == 10) {
				memcpy(getOnlineUserListRespond.onlineUserList, onlineUserList, 200);
				sendRespond(&getOnlineUserListRespond);
				numUsersOnline = 0;
			}
		}
	}
	//

	memcpy(getOnlineUserListRespond.onlineUserList, onlineUserList, 190);
	sendRespond(&getOnlineUserListRespond);
}

void handleGetListOnlineUserRequest(Request request) {
	sendGetOnlineUserListRespond();
}

void recognizeRequest(char* buff) {
	Request request;
	request = *((Request*) buff);

	switch(request.typeRequest) {
		case LOGIN_REQUEST:
			handleLoginRequest(request);
			break;

		case LOGOUT_REQUEST:
			handleLogoutRequest(request);
			break;

		case REGISTER_REQUEST:
			handleRegisterRequest(request);
			break;

		case CHAT_REQUEST:
			handleChatWithFriendRequest(request);
			break;

		case GET_ONLINE_USER_LIST_REQUEST:
			handleGetListOnlineUserRequest(request);
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
		userRegisted[i].isOnline = FALSE;
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
                      		close(currentSockFD);
							clients[i].fd = -1;

                   		} else {
                   			printf("error_sys\n");
                   			exit(0);
                   		}
                        	 
					} else if (size == 0) {
						close(currentSockFD);
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