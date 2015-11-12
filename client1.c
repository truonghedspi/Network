#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <arpa/inet.h>
#include <errno.h>
#include <poll.h>
#include <sys/select.h>
#include "request.h"
#include "respond.h"
#define LEN 204





typedef struct{
	char nich_name[LEN];
	int type;

} User_List;



int check_input(char buff[]){
	if(strlen(buff)<6 || strlen(buff)>=15) return 0;
	else return 1;

}
int check_type(Respond respond){
	return respond.typeRespond;
}

/*int take_user_list(char data[],User_List user_list[]){
	char mesg2[LEN];
	int i=0;
	char *pch;

	strcpy(mesg2,mesg);
	pch=strtok(mesg2,"|");
	pch=strtok(NULL,"|");
	while(1){
		pch=strtok(NULL,"|");
		if(pch==NULL) break;
		strcpy(user_list[i].nich_name,pch);
		i++;

	}
}*/

	
/*int add_partner(char nich_name[],User_List chat_list[]){
	int i=0;
	while(strcmp(chat_list[i].nich_name,"") != 0 ){
		i++;
	}
	strcpy(chat_list[i].nich_name,nich_name);
	return i+1;
}*/

int login(int sockfd){
	char mesg[LEN];
	char tk[100];
	char pass[100];
	LoginRequest user;
	Respond respond;
	LoginRespond loginrespond;
	int t, size;

	while(1){
		printf("\nNhap ten tai khoan: ");
		strcpy(tk,"");
		fgets(tk,LEN,stdin);
		tk[strlen(tk)-1]='\0';
		if(check_input(tk) == 0){
			printf("\nMoi nhap lai!!");
		}else break;
	}
	strcpy(user.userName,tk);
	while(1){
		printf("Nhap mat khau: ");
		strcpy(pass,"");
		fgets(pass,LEN,stdin);
		pass[strlen(pass)-1]='\0';
		if(check_input(pass) == 0){
			printf("\nMoi nhap lai!!\n");
		}else break;
	}
	strcpy(user.password,pass);
	user.typeRequest=LOGIN_REQUEST;
	memcpy(mesg,&user,LEN);
	send(sockfd, mesg,LEN, 0);
	respond=(*(Respond*)mesg);
	if(check_type(respond) == LOGIN_RESPOND){
		loginrespond=(*(LoginRespond*)mesg);
			if (strlen(loginrespond.messenger > 0))
			puts(loginrespond.messenger);
		else 
			printf("Data null\n");
		if(loginrespond.loginResult == LOGIN_SUCCESS){
			printf("\nDang nhap thanh cong");
			return 1;
		}
		if(loginrespond.loginResult == LOGIN_INVALID_USERNAME){
			printf("\nTai khoan khong dung");
			return 0;
		}
		if(loginrespond.loginResult == LOGIN_INVALID_PASSWORD){
			printf("\nMat khau khong dung");
			return 0;
		}
		if(loginrespond.loginResult == LOGIN_ONLINE_EXISTED){
			printf("\nTai khoan dang online");
			return 0;
		}
	} else {
		printf("ERROR\n");
	}

	
}

int sign_up(int sockfd){
	char mesg[LEN];
	char tk[20];
	char pass[20];
	char comf_pass[20];
	int t;
	RegisterRequest user;
	Respond respond;
	RegisterRespond registerrespond;

	while(1){
		printf("\nNhap ten tai khoan: ");
		strcpy(tk,"");
		fgets(tk,LEN,stdin);
		tk[strlen(tk)-1]='\0';
		if(check_input(tk) == 0){
			printf("\nMoi nhap lai!!");
		}else break;
	}
	strcpy(user.userName,tk);
	while(1){
		printf("Nhap mat khau: ");
		strcpy(pass,"");
		fgets(pass,LEN,stdin);
		pass[strlen(pass)-1]='\0';
		if(check_input(pass) == 0){
			printf("\nMoi nhap lai!!\n");
		}else break;
	}
	while(1){
		printf("Xac nhan mat khau: ");
		strcpy(comf_pass,"");
		fgets(comf_pass,LEN,stdin);
		comf_pass[strlen(comf_pass)-1]='\0';
		if(strcmp(pass,comf_pass)==0) break;
	}
	strcpy(user.password,pass);
	user.typeRequest=REGISTER_REQUEST;
	memcpy(mesg,&user,LEN);
	send(sockfd, mesg,LEN, 0);
	recv(sockfd, mesg, LEN, 0);
	respond=(*(Respond*)mesg);
	if(respond.typeRespond == REGISTER_RESPOND){
		registerrespond=(*(RegisterRespond*)mesg);
		if(registerrespond.registerResult == REGISTER_SUCCESS){
			printf("\nDang ki thanh cong");
			return 1;
		}
		if(registerrespond.registerResult == REGISTER_INVALID_USERNAME){
			printf("\nTai khoan loi");
			return 0;
		}
		if(registerrespond.registerResult == REGISTER_INVALID_PASSWORD){
			printf("\nMat khau loi");
			return 0;
		}
		if(registerrespond.registerResult == REGISTER_USER_EXISTED){
			printf("\nTai khoan da ton tai");
			return 0;
		}
	}

	
}

/*int answer_chat_request(int sockfd,char data[],User_List chat_list[]){
	int i;
	char nich_name[LEN];
	char mesg[LEN];
	char *pch;
	Form form_s,form_t;
	char yes[2];

	strcpy(nich_name,data);
	printf("\n@%s muon chat voi ban!!",nich_name);
	printf("\nBan co dong y(y/n): ");
	fgets(yes,3,stdin);
	if(yes[0] == 'y' || yes[0] == 'Y') {
		add_partner(nich_name,chat_list);
		form_t.type=1;
		strcpy(form_t.data,nich_name);
		//ep form_t ve char
		form_t=form_t(char);
		form_s.type=5;
		strcpy(form_s.data,form_t);
		//ep form_s ve char
		mesg=form_s(char);
		send(sockfd,mesg,strlen(mesg),0);
		return 1;
	}
	else {
		form_t.type=0;
		strcpy(form_t.data,nich_name);
		//ep form_t ve char
		form_t=form_t(char);
		form_s.type=5;
		strcpy(form_s.data,form_t);
		//ep form_s ve char
		mesg=form_s(char);
		send(sockfd,buff,strlen(buff),0);
		return 0;
	}
}*/

/*void choose_user(int sockfd,User_List chat_list[]){
	int i,t,y,a;
	User_List user_list[100];
	char mesg[LEN],buff[LEN];
	Form form_s,form_r,form_t;
	Request request;
	Respond respond;
	GetOnlineUserListRespond listRespond;

	request.typeRequest=GET_LIST_USER_ONLINE_REQUEST;
	memcpy(mesg,&request);
	send(sockfd,meag,strlen(meag),0);
	strcpy(mesg,"");
	recv(sockfd,mesg,LEN,0);
	respond=(*(Respond*)mesg);
	if(check_type(respond)==GET_LIST_USER_ONLINE_REQUEST) {
		//ep form_r.data ve Form
		form_t=form_r.data(Form);
		t=check_type(form_t);
		if(t == 1){
			//ep form_t.data ve kieu User_List
			user_list=form_t.data(User_List);
		}else if(t == 0){
		printf("\nKhong co user khac online!!");
		return;
		}
	}
	for ( i = 0; i < a; i++){
		printf("\n%d.@%s",i+1,user_list[i].nich_name);
	}
	while(1){
		printf("\nChon user de chat: ");
		scanf("%d",&y);
		fgets(buff3,LEN,stdin);
		if(0<y&&y<=a) break;
	}
	strcpy(mesg,"");
	form_s.type=4;
	strcpy(form_s.data,user_list[y-1].nich_name);
	//ep form_s ve kieu char
	mesg=form_s(char);
	send(sockfd,mesg,strlen(mesg),0);
	strcpy(mesg,"");
	recv(sockfd,mesg,LEN,0);
	//ep mesg ve kieu Form
	form_r=mest(Form)
	t = check_type(form_r);
	if(t == 5){
		//ep form_r.data ve kieu Form
		form_t=form_r.data(Form);
		t = check_type(form_t);
		if(t == 1){
			printf("\n@%s da dong y chat\n",user_list[y-1].nich_name);
			add_partner(user_list[y-1].nich_name,chat_list);
			fgets(buff,LEN,stdin);
			chatting(sockfd,chat_list);
			return;
		}else if(t == 0){
		printf("\nUser khong dong y chat!!\n");
		return;
		}
	}
}*/

/*void menu(int sockfd){

	char choose[2];
	int max_user_list;
	int rv,t,x;
	char mesg[LEN];
	User_List user_list[100];
	User_List chat_list[10];
	fd_set readSet;
	struct timeval tv;
	Respond respond;

	while(1){
		printf("\n***DOI 1 USER KHAC HOAC CHON CAC MUC DUOI DAY***\n");
		printf("\n1.Chon 1 user dang online de chat");
		printf("\n2.Chon 1 phong de chat");
		printf("\n3.Thoat");
		printf("\nBan chon: ");
		printf("\n");
		while(1){
			tv.tv_sec = 5;
			tv.tv_usec =0;
			FD_SET(sockfd, &readSet);
			FD_SET(fileno(stdin), &readSet);
			select(sockfd +1, &readSet, NULL, NULL, &tv);
	
			if (FD_ISSET(fileno(stdin), &readSet)){
				fgets(choose,3,stdin);
				switch(choose[0]){
		    		case '1' :
		    			choose_user(sockfd,chat_list);
		    			break;
		    		case '2' :
		    			choose_room(sockfd,chat_list);
		    			break;
		    		case '3' :
		    			log_out(sockfd);
		    			return ;
		    		default :
		    			break;
		    	}
			}else if(FD_ISSET(sockfd, &readSet)){
				strcpy(mesg,"");
				recv(sockfd,mesg,LEN,0);
				respond=(*(Respond*)mesg);
				if(respond.typeRespond == CHAT_RESPOND) {

				}
			}
		}
	}

}*/

void main(){
	int sockfd;
	struct sockaddr_in serv_addr;
	char choose[5];
	int t;
	fd_set readSet;
	struct timeval tv;

	tv.tv_sec = 0;
	tv.tv_usec = 0;
    serv_addr.sin_family = AF_INET;       
    serv_addr.sin_port = htons(5500);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
    size_t size =100;

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    	printf("\nLoi tao socket!!");
    }
    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\nConnect loi!!\n");
       return ;
    }else
    FD_ZERO(&readSet);

    while(1){
    	printf("\n*****MENU*****\n");
    	printf("\n1.Dang nhap\n2.Dang ki\n3.Thoat" );
    	printf("\nBan chon: ");
    	FD_SET(fileno(stdin), &readSet);
    	FD_SET(sockfd, &readSet);
    	select(sockfd+1, &readSet, NULL, NULL, &tv);

    		fgets(choose,3,stdin);
    		switch(choose[0]){

	    		case '1': 
	    				printf("\nBan chon Dang Nhap");
	    				t = login(sockfd);
	    			    //if(t == 1) menu(sockfd);
	    			    
	    			    break;
	    		case '2': 	
	    				printf("\nBan chon Dang Ki");
	    				t = sign_up(sockfd);
	    			    //if(t == 1) menu(sockfd);
	    			    
	    			    break;
	    		case '3':
	    			return;
	    		default : break;
    		
    	}

    	if (FD_ISSET(sockfd, &readSet)) {

    	}
    	
    }



}