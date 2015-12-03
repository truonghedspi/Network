
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
int mo_file(char* userNameSend, char* userNameRecv, FILE* f){
	char* filename1;
	char* filename2;

	strcpy(filename1,"/DataStructure/");
	strcpy(filename2,"/DataStructure/");
	strcat(filename1,userNameSend);
	strcat(filename1,"_");
	strcat(filename1,userNameRecv);
	strcat(filename1,".dat");
	strcat(filename2,userNameRecv);
	strcat(filename2,"_");
	strcat(filename2,userNameSend);
	strcat(filename2,".dat");

	f=fopen(filename1,"ab");
	if(f == NULL){
		f=fopen(filename2,"ab");
	}else return 1;
	if(f == NULL){
		f=fopen(filename1,"a+b");
	}else return 1;
	return 0;
}
int write_log(ChatRequest chatRequest){
	char userNameSend[LEN];
	char userNameRecv[LEN];
	time_t rawtime;
  	struct tm * timeinfo;
  	Log chatlog;
  	FILE *f;
  	int userIndex,t;

  	//userIndex =  findUserIndexWithSockFD(currentSockFD);

	strcpy(userNameSend,"tranha");
	strcpy(userNameRecv,"tranha1");
	t=mo_file(userNameSend,userNameRecv,f);
  	if(t==1){
  		time ( &rawtime );
  	  	timeinfo = localtime ( &rawtime );
  		strcpy(chatlog.userNameSend,userNameSend);
  		strcpy(chatlog.time,asctime (timeinfo));
  		fwrite(&chatlog,sizeof(Log),1,f);
  		fclose(f);
  		return 1;
  	}else {
  		fclose(f);
  		return 0;
  	}

}
int get_log(char userNameSend[],char userNameRecv[],Log chatlog,int i){
  	FILE * f;
  	int t;

  	t=mo_file(userNameSend,userNameRecv,f);
  	if(t == 0)printf("\nlog empty");
  	else{
  		if(fseek(f,i*sizeof(Log),SEEK_SET)!=0){
		printf("Fseek failed!!\n");
		return 0;
		}
		fread(&chatlog,sizeof(Log),1,f);
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
/*void main(){

	Log chatLog[6];
	int i=3;

	FILE *f;
	f=fopen("test.dat","a");
	strcpy(chatLog[0].time,"mot");
	strcpy(chatLog[0].userNameSend,"ha1");
	strcpy(chatLog[1].userNameSend,"ha2");
	strcpy(chatLog[2].userNameSend,"ha3");
	strcpy(chatLog[2].time,"ba");
	strcpy(chatLog[1].time,"hai");
	fwrite(&chatLog[1],sizeof(Log),1,f);
	fclose(f);
	f=fopen("test.dat","a");
	fwrite(&chatLog[2],sizeof(Log),1,f);
	fclose(f);
	f=fopen("test.dat","a");
	fwrite(&chatLog[0],sizeof(Log),1,f);
	fclose(f);
	f=fopen("test.dat","r");
	while(1){
		if(fseek(f,i*sizeof(Log),SEEK_SET)!=0){
		printf("Fseek failed!!\n");
		//break;
		}
		fread(&chatLog[i],sizeof(Log),1,f);

		if(feof(f)){
			printf("\nHet file");
			break;
		}
		else {
			printf("\nVan con");
			i++;
		}
	}
	printf("\n%s",chatLog[3].userNameSend );
	printf("\n%s",chatLog[4].userNameSend );
	printf("\n%s",chatLog[5].userNameSend );


}
*/
/*int get_log(ChatRequest chatRequest,Log chatLog,int i){
  	char userNameSend[15];
	char userNameRecv[15];
	char fileName[35];
  	FILE *f;
  	int userIndex,t;

  	userIndex =  findUserIndexWithSockFD(currentSockFD);
	strcpy(userNameSend,userRegisted[userIndex].userName);
	strcpy(userNameRecv,chatRequest.userNameReceiver);
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

}*/
void main(){
	Log chatLog[1000],chatLog2;
	char mesg[LEN];
	FILE *f;
	int i=0,y=1;

	time_t rawtime;
  	struct tm * timeinfo;
	
		/*f=fopen("DataLog/tranha1_tranha.dat","wb");
  		time ( &rawtime );
  	  	timeinfo = localtime ( &rawtime );
  	  	strcpy(chatLog.time,asctime (timeinfo));
  	  	chatLog.time[strlen(chatLog.time)-1]='\0';
  		strcpy(chatLog.userNameSend,"trandanhha");
  		strcpy(chatLog.constan,"Toi la ha");
  		fwrite(&chatLog,sizeof(Log),1,f);
  		fclose(f);*/


	while(1){
		f=fopen("DataLog/tranha1_tranha.dat","rb");
		//fseek(f, 0, SEEK_END);
		/*while(1){
			fseek(f,y*sizeof(Log),SEEK_SET);
			if(feof(f)) break;
			y++;
		}*/
		/*printf("\nYYYYYY: %d",y );
		if(fseek(f,-10*sizeof(Log),SEEK_SET)==-1){
			rewind(f);
		}*/
		
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
		
		
	}
	printf("iiiiiiiiii%d\n",i );
	for(y=0;y<i;y++){
			printf("\nTime: %s",chatLog[y].time);
			printf("\nUser: %s",chatLog[y].userNameSend);
			printf("\nCons: %s",chatLog[y].constan);
		}

	/*f=fopen("DataLog/tranha1_tranha.dat","rb");
	fseek(f,2*sizeof(Log),SEEK_SET);
	fread(&chatLog,sizeof(Log),1,f);
	printf("\nTime: %s",chatLog.time);
	printf("\nUser Name: %s",chatLog.userNameSend);
	printf("\nConstan: %s\n",chatLog.constan);

	fclose(f);
*/}

 