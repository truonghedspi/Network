int score(int count)
{
	switch(count){
		case 1: return 10;
		case 2: return 20;
		case 3: return 40;
		case 4: return 80;
		case 5: return 100;
		case 6: return 200;
		case 7: return 500;
		case 8: return 800;
		case 9: return 1000;
		case 10: return 2000;
		case 11: return 4000;
		case 12: return 6000;
		case 13: return 8000;
		case 14: return 10000;
		case 15: return 15000;
		default: return 0;
	}
}
void save_score(char account[32],int score)
{
	FILE *fp;
	char file_name[50]="user/";
	diem sc;
	time_t rawtime;
    struct tm * timeinfo;

	time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    strcpy(sc.thoi_gian,asctime(timeinfo));	
    sc.score=score;
    strcat(file_name,account);
	strcat(file_name,".dat");
	fp=fopen(file_name,"ab");
	fwrite(&sc,sizeof(diem),1,fp);
	fclose(fp);
}
int get_score(char account[32],protocol *p,int i)
{
	FILE *fp;
	char file_name[50]="user/";
	strcat(file_name,account);
	strcat(file_name,".dat");
	fp=fopen(file_name,"rb");
	if(fseek(fp,i*sizeof(diem),SEEK_SET)!=0){
		printf("Fseek failed!!\n");
		return 0;
	}
	fread(&(p->sc),sizeof(diem),1,fp);
	if(feof(fp)){
		return 0;
	}
	else {
		fclose(fp);
		return 1;
	}
}