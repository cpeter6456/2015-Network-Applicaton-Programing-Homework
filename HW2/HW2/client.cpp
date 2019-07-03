#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/sendfile.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>


#include "header.h"


File_Info the_file;
int main(int argc, char **argv){

	static int sockfd;
	static struct sockaddr_in server;
	struct hostent *host;
	

	char keybroad[S_BUFF_SIZE];
	

	/* Set up destination address. */
	server.sin_family = AF_INET;
	/* Fill in code. */
	

	printf("Please Input \"connect IP address\" \n");
	while(scanf("%s",keybroad) && strcmp(keybroad,"connect")!=0 ){
			printf("Please Input \"connect IP address\" \n");
	}

	scanf("%s",keybroad);
	printf("IP is %s \n",keybroad);
	if ( !(host = gethostbyname(keybroad)) ){
		/**#include <stdio.h>
		#include <errno.h>**/
		perror("gethostbyname_error");
	}

	server.sin_port = htons(7777);

	/**#include <arpa/inet.h>**/
	inet_pton(AF_INET, host->h_addr, &server.sin_addr);
	/* Allocate socket.
	 * Fill in code. */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	/* Connect to the server.
	 * Fill in code. */
	if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
		perror("connect_error");
	}
	
	strcpy(keybroad,"");
	while(scanf("%s",keybroad) && strcmp(keybroad,"goodbye")!=0 ){
		
		if(strcmp(keybroad,"upload")==0){


			printf("!!!\n");
			scanf("%s",keybroad);
			printf("Your file is %s\n",keybroad);

			FILE *fp;
			//char buf[BUFF_SIZE];
			if ( (fp = fopen(keybroad, "rb")) == NULL){
				perror("fopen");
				exit(1);
			}
			File_Info the_file;
			strcpy(the_file.name ,keybroad);

			struct stat stat_buf;  /* hold information about input file ,stat_buf.st_size is file size*/
			stat(keybroad, &stat_buf);
			
			the_file.f_size = stat_buf.st_size;

			char ttt[100];


			fixed_huf(&the_file,0,ttt);


			/** 0 for compress , 1 for decompress**/
			printf("TTT is %s\n",ttt);

			printf("huf %s num is %d \n",the_file.huf_name,the_file.huf_f_size);

			/**Send File_Info structure**/
			if(send(sockfd,&the_file, sizeof(File_Info), 0)  < 0) {
				perror("send_error");
			}
			FILE *fp_huf;
			if ( (fp_huf = fopen(the_file.huf_name, "rb")) == NULL){
				perror("fopen_huf");
				exit(1);
			}
			FILE *fp_huf_table;
			if ( (fp_huf_table = fopen("-code-table.txt", "rb")) == NULL){
				perror("fopen_huf");
				exit(1);
			}
			int numbytes=0;
			char huf_buf[the_file.huf_f_size];
			char table_buf[the_file.table_size];
			/**send huf file**/
			while(!feof(fp_huf)){
				numbytes = fread(huf_buf, sizeof(char), sizeof(huf_buf), fp_huf);
				printf("fread %d bytes, ", numbytes);
				numbytes = write(sockfd, huf_buf, numbytes);
				printf("Sending %d bytesn\n",numbytes);
			}
			numbytes=0;
			/**send huf table file**/
			while(!feof(fp_huf_table)){
				numbytes = fread(table_buf, sizeof(char), sizeof(table_buf), fp_huf_table);
				printf("table fread %d bytes, ", numbytes);
				numbytes = write(sockfd, table_buf, numbytes);
				printf("table Sending %d bytesn\n",numbytes);
			}
			fclose(fp);
			fclose(fp_huf);
			printf("End of trans\n");
		}
		else{printf("%s\n","you can use upload file\n");}
	}
	close(sockfd);

}