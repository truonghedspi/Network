
#include <string.h>
#include <stdio.h>
#include <time.h>
#define LEN 204
typedef struct _Log{
	char time[30];
	char userNameSend[20];
	char constan[100];
}Log;
typedef enum {
	LOGIN_REQUEST,
	LOGOUT_REQUEST,
	REGISTER_REQUEST,
	CHAT_REQUEST,
	ROOM_REQUEST,
	GET_ONLINE_USER_LIST_REQUEST,
	GET_ROOM_LIST_REQUEST
}RequestType;
typedef enum {
	CHAT_FRIEND_SEND
}ChatType;
typedef struct _ChatRequest {
	RequestType typeRequest;
	ChatType chatType;
	char userNameReceiver[50];
	char messenger[146];
}ChatRequest;
int get_max_log(){
	char userNameSend[15];
	char userNameRecv[15];
	char fileName[35];
	Log chatLog;
  	FILE *f;
  	int userIndex,t,i=0;

  	strcpy(fileName,"DataLog/tranha1_tranha.dat");
  	
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
int get_log(char *buff,int i){
  	char userNameSend[15];
	char userNameRecv[15];
	char fileName[35];
  	FILE *f;
  	
  	int userIndex,t;

  	
  	strcpy(fileName,"DataLog/tranha1_tranha.dat");
  	
  	
  	f=fopen(fileName,"rb");
	if(fseek(f,i*sizeof(Log),SEEK_SET)==-1){
		printf("\nfseek error\n");
		fclose(f);
		return 0;
	}
	fread(&chatLog,sizeof(Log),1,f);
	if(feof(f)){
		printf("\nEOF");
		fclose(f);
		return 0;
	}else{
		fclose(f);
		printf("\nAAA: %s",chatLog.constan);
		
		return 1;
	}
	

}
void main(){
	Log chatLog[1000],chatLog2;
	char mesg[LEN];
	FILE *f;
	int i=0,y=1,max_log;

	time_t rawtime;
  	struct tm * timeinfo;
	/*while(1){
		f=fopen("DataLog/tranha1_tranha.dat","rb");
		if(fseek(f,i*sizeof(Log),SEEK_SET)==-1){
			printf("\nfseekaaaaaaa");
			break;
		}
		fread(&chatLog[i],sizeof(Log),1,f);
		if(feof(f)){
			break;
		}else{
			fclose(f);
			i++;
		}
	}*/
		max_log=get_max_log();
		max_log--;
		while(1){
			y=get_log(chatLog2,max_log);
			if(y==0) break;
			else{
				printf("\nNoi dung %s",chatLog2.constan);
				max_log--;
			}
		}
	
	
}

 