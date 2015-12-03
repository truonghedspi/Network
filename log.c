#include <time.h>
typedef struct _Log{
	char time[15];
	char userNameSend[15];
	char constan[LEN]
}Log;
int find_file_name(char* userNameSend, char* userNameRecv,char fileName[]){
	char* fileName1;
	char* fileName2;

	strcpy(fileName1,"/DataLog/");
	strcpy(fileName2,"/DataLog/");
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
			f=fopen(fileName1,"wb");
			strcpy(fileName,fileName1);
			fclose(f);
			return 1;
			
		}
	}
	return 0;
}
int write_log(ChatRequest chatRequest){
	char userNameSend[LEN];
	char userNameRecv[LEN];
	char fileName[LEN];
	time_t rawtime;
  	struct tm * timeinfo;
  	Log chatLog;
  	FILE *f;
  	int userIndex;


  	userIndex =  findUserIndexWithSockFD(currentSockFD);

	strcpy(userNameSend,userRegisted[userIndex].userName);
	strcpy(chatRequest.userNameReceiver,usernameRecv);
	t=find_file_name(userNameSend,usernameRecv,fileName);
  	if(t==1){
  		f=fopen(fileName,"ab");
  		time ( &rawtime );
  	  	timeinfo = localtime ( &rawtime );
  	  	strcpy(chatLog.time,asctime (timeinfo));
  		strcpy(chatLog.userNameSend,userNameSend);
  		strcpy(chatLog.constan,chatRequest.messenger)
  		fwrite(&chatLog,sizeof(Log),1,f);
  		fclose(f);
  		return 1;
  	}else return 0;

}

int rewrite(char fileName[]){
	Log chatLog[10];
	FILE *f;
	int i;

	f=fopen(fileName,"rb");
	if(fseek(f,10*sizeof(Log),SEEK_SET) == 0){

	}

}
int get_log(char userNameSend[],char userNameRecv[],Log chatLog;int i){
  	FILE * f;
  	int t;
  	char fileName[LEN];

  	t=find_file_name(userNameSend,userNameRecv,fileName);
  	
  	if(t==1){
  		f=fopen(fileName,"ab");
  		if(fseek(f,-10*sizeof(Log),SEEK_SET)!=0){
			rewind(f);
		}
		if(fseek(f,i*sizeof(Log),SEEK_SET)!=0){
			printf("fseek failed!!\n");
			return 0;
		}
		fread(&chatLog,sizeof(Log),1,f);
		if(feof(f)){
			return 0;
		}
		else {
			fclose(f);
			return 1;
		}
  	}
  	return 0;

}
//xu ly khi nhan dc chatType ==  CHAT_LOG_REQUEST
i=0;
ChatRespond respond;
while(1){
	get_log(userNameSend,userNameRecv,chatLog,i)
	strcpy(respond.messenger,chatLog.time);
	strcat(respond.messenger,chatLog.userNameSend);
	strcat(respond.messenger,chatLog.constan);
	respond.chatResult=CHAT_LOG_RESPOND;
	//va gui tra respond cho thang da yeu cau
}