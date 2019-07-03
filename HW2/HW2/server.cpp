#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sendfile.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "header.h"


File_Info the_file;
int main(int argc, char **argv) {

static struct sockaddr_in server,client_addr;
	int sd,cd;
	
	socklen_t client_addr_len;

	
	/* Create the socket.*/

	sd = socket(AF_INET, SOCK_STREAM, 0);
	/* Initialize address.*/

	/*use the Internet address family*/
	server.sin_family = AF_INET;

	/*bind socket 'sd(socket descrption)'  to  port 5678*/
	server.sin_port = htons(7777);

	/*bind: a client may connect to any of my address*/
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	int reuse_addr = 1;
	socklen_t reuse_addr_len = sizeof(reuse_addr);
/** 
int setsockopt(int sockfd, int level, int optname,const void *optval, socklen_t optlen)
**/
	setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, reuse_addr_len);


	/* Name and activate the socket.
	 * Fill in code. */
	if(bind(sd, (struct sockaddr *)&server, sizeof(server)) < 0) {
		DIE("bind_error");
	}
	if (listen(sd, 7) < 0) {
		DIE("listen_error");
	}

		/* main loop : accept connection; fork a child to have dialogue */
	
printf("wow\n");
		/* Wait for a connection.
		 * Fill in code. */
/**
if((client = accept(sd, NULL, NULL)) < 0){
perror("accept");
exit(errno);
}
accept系统调用从待处理的已连接队列中选取第一个连接请求，为之建立一个新的socket。
accept调用的返回值是新建立连接的描述符；新的socket可以用于read、write和poll/select系统调用。
**/
		client_addr_len = sizeof(client_addr);
		if ( (cd = accept(sd, (struct sockaddr*)&client_addr, &client_addr_len)) < 0) {	
			DIE("accept_error");
		}
	
		
		

		printf("A client has connected it's IP address is: %s\n", inet_ntoa(client_addr.sin_addr));
		printf("Port is: %d\n", (int) ntohs(client_addr.sin_port));
		printf("cd = %d\n",cd);
		if(client_addr.sin_family==AF_INET)
			printf("Protocal is TCP!!!\n");
	
		int ret=1;
		

		while ( (ret=recv(cd,&the_file, sizeof(File_Info), 0)) ){

			//printf("ret = %d\n",ret);
			if(ret == 0)break;
			printf("Original file name is %s file size is %d\n",the_file.name ,the_file.f_size);
			printf("huf_file name is %s file size is %d\n",the_file.huf_name ,the_file.huf_f_size);
			printf("table name is %s file size is %d\n","-code-table.txt" ,the_file.table_size);
			FILE *fp;
			if ( (fp = fopen(the_file.huf_name , "wb")) == NULL){
				perror("fopen");
				exit(1);
			}
			FILE *fp_table;
			if ( (fp_table = fopen("-code-table.txt", "wb")) == NULL){
				perror("fopen");
				exit(1);
			}
			int numbytes=-1;
			
				//Receive file from server
			//char buf[BUFF_SIZE];
			char huf_buf[the_file.huf_f_size];
			char table_buf[the_file.table_size];
			/**Here is receive from original file**/
			int total=0;
			while(1){
				if(numbytes == 0 || total >= the_file.huf_f_size){
					printf("break1\n");
					break;
				}
				numbytes = read(cd, huf_buf, sizeof(huf_buf));
				//printf("read %d bytes, ", numbytes);
				fflush(stdout);
				
				numbytes = fwrite(huf_buf, sizeof(char), numbytes, fp);

				total+=numbytes;
				//printf("total =%d\n",total);
				if(numbytes == 0 || total >= the_file.huf_f_size){
					//printf("break2 total =%d\n",total);
					break;
				}
				//printf("fwrite %d bytesn\n", numbytes);
			}
			fclose(fp);
			printf("You are End Of Trans huf-file \n" );
			numbytes=-1;

			/**carefor not -1, use 0 **/
			total=0;
			while(1){
				if(numbytes == 0 || numbytes >= the_file.table_size){
					//printf("break1\n");
					break;
				}
				numbytes = read(cd, table_buf, sizeof(table_buf));
		
				//printf("table read %d bytes, ", numbytes);
				fflush(stdout);
				
				numbytes = fwrite(table_buf, sizeof(char), numbytes, fp_table);

				total+=numbytes;
				if(numbytes == 0 || total >= the_file.table_size){
					//printf("break2\n");
					break;
				}
				//printf("ftable write %d bytesn\n", numbytes);
			}
			fclose(fp_table);



			printf("You are End Of Trans table(Here it END)\n" );
			
			//printf("huf %s\n",the_file.huf_name);
			char tttt[100];
			fixed_huf(&the_file,1,tttt);
			

			memset(&the_file,0,sizeof(File_Info));
			printf("You are End Of decompress file \n");
		}
		
		printf("client exit\n");
		close(cd);
		//exit(0); /* child does not want to be a parent */

	
}