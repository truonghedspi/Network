
#include "input.h"
#include "request.h"
#include "respond.h"
#include "my_socket.h"

const int OPEN_MAX = 50;
const int MAX_USER = 10;
const int BACKLOG = 50;
int currentSockFD = -1;
int numUserRegisted = 0;
struct pollfd clients[OPEN_MAX];
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
int readUsersFile(char * fileName, User userRegisted[]) {
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

int sendRespond(Respond respond) {
	char buff[250];

	memcpy(buff, &respond, sizeof(Respond));
	return send(currentSockFD, buff, sizeof(Respond), 0);
}

void handleLoginRequest(Request request) {
	LoginRequest loginRequest;
	LoginRespond loginRespond;
	Respond respond;
	User user;
	int userIndex = -1;

	printf("User Login!\n");
	memcpy(&loginRequest, &request, sizeof(LoginRequest));
	makeUser(&user, loginRequest.userName, loginRequest.password);
	loginRespond.typeRespond = LOGIN_RESPOND;
	userIndex = findUserIndex(user, userRegisted,numUserRegisted);

	if (userIndex < 0) {
		loginRespond.loginResult = LOGIN_INVALID_USERNAME;
		strcpy(loginRespond.messenger, "User name not found!");
		memcpy(&respond, &loginRespond, sizeof(Respond));
		sendRespond(respond);
		return;
	}

	if (strcmp(user.password, userRegisted[userIndex].password) != 0) {
		loginRespond.loginResult = LOGIN_INVALID_PASSWORD;
		strcpy(loginRespond.messenger, "Password invalid!");
		memcpy(&respond, &loginRespond, sizeof(Respond));
		sendRespond(respond);
		return;
	}

	if (userRegisted[userIndex].isOnline == FALSE) {
		loginRespond.loginResult = LOGIN_ONLINE_EXISTED;
		strcpy(loginRespond.messenger, "User was online!");
		memcpy(&respond, &loginRespond, sizeof(Respond));
		sendRespond(respond);
		return;
	}	

	loginRespond.loginResult = LOGIN_SUCCESS;
	strcpy(loginRespond.messenger, "Log in success");
	memcpy(&respond, &loginRespond, sizeof(Respond));
}

void handleLogoutRequest(Request request) {

}

void handleRegisterRequest(Request request) {
	
}

void handleChatWithFriendRequest(Request request) {

}

void handleGetListOnlineUserRequest(Request request) {

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

		case GET_LIST_USER_ONLINE_REQUEST:
			handleGetListOnlineUserRequest(request);
			break;
	}
}

int main() {
	const int PORT = 5500;
	int listenFD, clientFD, sockFD;
	socklen_t clientLen ;
	int nReady, maxIndex, i,  size;
	struct sockaddr_in  clientAddr;
	char buff[sizeof(Request)+1];

	numUserRegisted = readUsersFile("data.txt", userRegisted);
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
		nReady = poll(clients, maxIndex + 1, 3);
		if (clients[0].revents & POLLRDNORM) {
			clientLen = sizeof (clientAddr);
			clientFD = accept(listenFD, (struct sockaddr *) &clientAddr, &clientLen);
			printf("Client : %d\n connected", clientFD);

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
			clients[i].events = POLLWRNORM;
			if (i > maxIndex) {
				maxIndex = i;
			}
			if (--nReady <= 0){
				continue;
			}
		}

		for (i = 1; i <= maxIndex; ++i) {
			if ( (sockFD = clients[i].fd) < 0) {
				continue;
			}
			if (clients[i].revents & (POLLRDNORM | POLLERR)) {
					setCurrentSockFD(sockFD);
					size = recv(currentSockFD, buff, sizeof(Request), 0);
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