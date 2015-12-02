#include <time.h>
typedef struct _Log{
	char time[LEN];
	char userNameSend[LEN];
}Log;
int mo_file(char* userNameSend, char* userNameRecv, FILE* f){
	//FILE *f;
	char* filename1;
	char* filename2;

	strcpy(filename1,userNameSend);
	strcat(filename1,"_");
	strcat(filename1,userNameRecv);
	strcat(filename1,".dat");
	strcpy(filename2,userNameRecv);
	strcat(filename2,"_");
	strcat(filename2,userNameSend);
	strcat(filename2,".dat");

	f=fopen(filename1,"ab");
	if(f == NULL){
		f=fopen(filename2,"ab");
	}else return 1;
	if(f == NULL){
		f=fopen(filename1,"a+b");
	}else return 2
	return 0;
}
void ghi_log(FILE *f,ChatRequest chatRequest){
	char userNameSend[LEN];
	char userNameRecv[LEN];
	time_t rawtime;
  	struct tm * timeinfo;
  	Log chatlog;

  	time ( &rawtime );
  	timeinfo = localtime ( &rawtime );
	//lay ten nguoi gui userNameSend
	strcpy(chatlog.userNameSend,userNameSend);
	strcpy(chatlog.time,asctime (timeinfo));
	fwrite(&chatlog,sizeof(chatlog),1,f);
	fclose(f);

}
int lay_log(ChatRequest chatRequest){
	char userNameSend[LEN];
	char userNameRecv[LEN];
  	Log chatlog;
  	FILE * f;

  	//lay ten nguoi gui userNameSend
  	//lay ten nguoi nhan userNameRecv
  	f=mo_file(userNameSend,userNameRecv);

}