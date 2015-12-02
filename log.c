typedef struct _Log{

}Log;
FILE* mo_file(char* userNameSenb, char* userNameRecv){
	FILE *f;
	char* filename1;
	char* filename2;

	strcpy(filename1,userNameSenb);
	strcat(filename1,"_");
	strcat(filename1,userNameRecv);
	strcat(filename1,".dat");
	strcpy(filename2,userNameRecv);
	strcat(filename2,"_");
	strcat(filename2,userNameSenb);
	strcat(filename2,".dat");

	f=fopen(filename1,"ab");
	if(f == NULL){
		f=fopen(filename2,"ab");
	}
	if(f == NULL){
		f=fopen(filename1,"a+b");
	}
	return f;
}
int ghi_log(FILE *f,ChatRequest chatRequest){

}