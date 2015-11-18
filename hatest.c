#include <stdio.h>
#include "respond.h"
#include "request.h"
#define LEN 204 

void wait(char buff[LEN]){
	
	int rv;
	int size_recv;
	char mesg[LEN];
	fd_set readSet;
	struct timeval tv;
	Respond respond;

	strcpy(buff,"");
	while(1){
		tv.tv_sec = 0;
		tv.tv_usec =0;
		FD_SET(currentSockFD, &readSet);
		FD_SET(fileno(stdin), &readSet);
		
		select(currentSockFD +1, &readSet, NULL, NULL, &tv);
		if(FD_ISSET(currentSockFD, &readSet)){
			strcpy(mesg,"");
			size_recv = recv(currentSockFD,mesg,LEN,0);
			if(size_recv == 0){
				printf("Server disconnect\n");
				close(currentSockFD);
				exit(0);
			}
			if (size_recv < 0) {
				printf("Error\n");
				close(currentSockFD);
				exit(0);
			}
			if(size_recv > 0)check_respond(mesg);
			if(--rv <= 0){
                break;
            }
		}
		if (FD_ISSET(fileno(stdin), &readSet)){	
			fgets(buff,LEN,stdin);
			break;
		} 
	}
	//return buff;
}
void menu(){
	char buff[LEN];

	main_menu();
	wait(buff);
	switch(buff[0]){
		case '1' :
			printf("\nBan chon 1");
			clear_user_list();
			online_user_list_request();
			break;
		case '2' :
			//creat_room(chatList);
			//chat
			break;
		case '3' :
			//invite_user_into_room(chatList) 
			break ;
		case '4' :
			log_out();
			return;
		default :
			break;
	}
}
void main(){
	int sockfd;
	struct sockaddr_in serv_addr;
	char choose[2];
	int t;
	char buff[LEN];

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
    while(1){
    	currentSockFD = sockfd;
    	printf("\n*****MENU*****\n");
    	printf("\n1.Dang nhap\n2.Dang ki\n3.Thoat" );
    	printf("\nBan chon: ");
    	fgets(choose,3,stdin);
    	switch(choose[0]){

    		case '1': 
    				printf("\nBan chon Dang Nhap");
    				t = login();
    			    if(t == 1) {
    			    	menu();
        			    do{
    						wait(buff);	
    					} while (1);}
   					break;
    		case '2': 	
    				printf("\nBan chon Dang Ki");
    				t = sign_up();
    			    //if(t == 1) menu(sockfd);
    			    
    			    break;
    		case '3':
    			close(currentSockFD);
    			return;
    		default : break;
    			

    	}
    }
}