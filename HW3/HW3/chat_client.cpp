#include "chat_header.h"


static int sockfd;
pthread_mutex_t RWmutex;
void clientlisten(void);
int main(int argc, char **argv){

	pthread_mutex_init (&RWmutex, NULL);

	static struct sockaddr_in server_addr4;
	struct hostent *host;
	char IPaddress[S_BUFF_SIZE],Port[S_BUFF_SIZE],Nickname[S_BUFF_SIZE],keybroad[S_BUFF_SIZE];
	server_addr4.sin_family = AF_INET;
	printf("Please Input connect IPaddress Port Nickname \nex.connect 192.168.1.1 1234 Caesar	 \n");
	while(scanf("%s%s%s%s",keybroad,IPaddress,Port,Nickname) && strcmp(keybroad,"connect")!=0 ){
			printf("Please Input connect IPaddress Port Nickname \nex.connect 192.168.1.1 5678 Caesar  \n");
	}
	int c;
	while ((c = getchar()) != '\n' && c != EOF);
	
	printf("IP is %s \n",IPaddress);
	/**
	if ( !(host = gethostbyname(IPaddress)) ){
		//#include <stdio.h>
		//#include <errno.h>
		perror("gethostbyname_error");
	}**/
	server_addr4.sin_addr.s_addr = inet_addr(IPaddress);
	server_addr4.sin_port = htons(atoi(Port));
	/**#include <arpa/inet.h>**/
	/**inet_pton函数原型如下[将“点分十进制” －> “整数”],用来验证IP是否合法,AF_INET is ipv4,AF_INET6 is ipv6**/
	//inet_pton(AF_INET, host->h_addr, &server_addr4.sin_addr);

	
	/** Allocate socket.**/
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	/** Connect to the server.*/
	if (connect(sockfd, (struct sockaddr *)&server_addr4, sizeof(server_addr4)) < 0) {
		perror("connect_error");
	}

	printf("connect success can use (bye,chat somebodys+ \"message\" ) \n");

	write(sockfd, &Nickname, sizeof(Nickname));

	pthread_t thread_listen;
	pthread_create(&thread_listen, NULL , (void* (*)(void*))clientlisten , 0);
	//char cc;
//printf("HAHA\n");
memset(&keybroad,0,sizeof(keybroad));
	while(gets(keybroad) && strncmp(keybroad,"bye",3)!=0 ){
		
	
		//printf("!%s!len = %d\n",keybroad,strlen(keybroad));
		if(strncmp(keybroad,"chat ",5)!=0){/**Format error **/
			//printf("HAHA1\n");
			
			printf("use bye or chat somebodys+ \"message\"  \n");
		}
		else{/**Send to server**/
		//printf("HAHA2\n");
			//struct chat_node chat;
			
			int len = strlen(keybroad);
			int numbytes;keybroad[len]='\0';
			numbytes = write(sockfd, &keybroad, len);
			//printf("numbytes = %d\n",numbytes);
		}
		memset(&keybroad,0,sizeof(keybroad));
	}
	printf("END\n");
	fflush(stdout);
	if(pthread_cancel(thread_listen)!=0)
		perror("pthread_cancel_error");

	close(sockfd);
	return 0;
}
void clientlisten(void){
	int ret=1;
	char read_buffer[S_BUFF_SIZE];
	while (  ret ){
		memset(&read_buffer,0,S_BUFF_SIZE);
		ret=recv(sockfd,&read_buffer, sizeof(read_buffer), 0);
		printf("%s\n",read_buffer);

		
	}
	return;
	
}