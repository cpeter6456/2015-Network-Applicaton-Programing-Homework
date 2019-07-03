#include "chat_header.h"
using namespace std;
/**
struct in_addr sin_addr;

struct in_addr { 
    unsigned long s_addr;
           }
**/
struct client
{
    //struct sockaddr_in client_addr;
    char name[S_BUFF_SIZE];
    char old_message[S_BUFF_SIZE];
    int fd;
    int alive;
    unsigned long  IP;
};
int read_from_client (int filedes);

list<struct client> List;
int main(){


	static struct sockaddr_in server_addr;

	/**serverdescriptor , clientdescriptor**/
	int sock;
	
	/**socklen_t same as int**/	
	

	sock = socket(AF_INET, SOCK_STREAM, 0);
	/*use the Internet address family*/
	server_addr.sin_family = AF_INET;

	/*bind socket 'sock(socket descrption)'  to  port 5678*/
	server_addr.sin_port = htons(5678);

	/*bind: a client may connect to any of my address*/
	/**INADDR_ANY就是指定地址为0.0.0.0的地址，这个地址事实上表示不确定地址，或“所有地址”、“任意地址”。 一般来说，在各个系统中均定义成为0值**/
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	int reuse_addr = 1;
	socklen_t reuse_addr_len = sizeof(reuse_addr);
/** 
int setsockopt(int sockfd, int level, int optname,const void *optval, socklen_t optlen)
closesocket（一般不會立即關閉而經歷TIME_WAIT的過程）後想繼續重用該socket：
BOOL bReuseaddr=TRUE;
setsockopt(s,SOL_SOCKET ,SO_REUSEADDR,(const char*)&bReuseaddr,sizeof(BOOL));
**/
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, reuse_addr_len);

	/* Name and activate the socket.
	 * Fill in code. */
	if(bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		perror("bind_error");
	}
	if (listen(sock, 7) < 0) {
		perror("listen_error");
	}
	
	//int next=0;

	fd_set active_fd_set, read_fd_set;
	int i;
  	struct sockaddr_in clientname;
  	socklen_t size;


	
	//char client_buf[S_BUFF_SIZE];
 	FD_ZERO (&active_fd_set);
  	FD_SET (sock, &active_fd_set);

  	list<struct client>::iterator it;
//http://blog.sina.com.cn/s/blog_6151984a0100etj1.html
//http://www.yolinux.com/TUTORIALS/LinuxTutorialC++STL.html#LIST
//http://www.gnu.org/software/libc/manual/html_node/Server-Example.html
	while(1){

      /* Block until input arrives on one or more active sockets. */
      read_fd_set = active_fd_set;
      if (select (FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0)
        {
          perror ("select_error");
          exit (EXIT_FAILURE);
        }

      /* Service all the sockets with input pending. */
      for (i = 0; i < FD_SETSIZE; ++i)/**run table**/
        if (FD_ISSET (i, &read_fd_set))
        {
            if (i == sock)
              {
                /* Connection request on original socket. */
                int newcd;
                size = sizeof (clientname);
                newcd = accept (sock,
                              (struct sockaddr *) &clientname,
                              &size);
                if (newcd < 0)
                  {
                    perror ("accept_error");
                    exit (EXIT_FAILURE);
                  }
                fprintf (stdout,
                         "Server:client connect from host %s, port %hu.\n",
                         inet_ntoa (clientname.sin_addr),
                         ntohs (clientname.sin_port));
              	char buffer[S_BUFF_SIZE];
			  	

			  	read (newcd, buffer, S_BUFF_SIZE);
			  	fprintf (stdout,
                         "Server:client Nickname is %s.\n",
                     	buffer);

			  	char t_buf[S_BUFF_SIZE];
			  	memset(&t_buf,0,S_BUFF_SIZE);
			  	strcat(t_buf,buffer);
			  	strcat(t_buf," is On Line!!!");
			  	for(it=List.begin(); it != List.end(); ++it){
					if((*it).alive == 1 )
						write((*it).fd, &t_buf, strlen(t_buf) );
				}

			  	int flag=0;
			  	for(it=List.begin(); it != List.end(); ++it){
			  		if((*it).alive == 0 && 
			  			strcmp( (*it).name,buffer)==0 &&
			  			(*it).IP == clientname.sin_addr.s_addr){
			  			(*it).alive = 1;flag=1;
			  			if(strlen((*it).old_message) > 0 )
			  				write((*it).fd, & ((*it).old_message), strlen((*it).old_message) );
			  			memset(&((*it).old_message),0,S_BUFF_SIZE);
			  			break;
			  		}
			  	}
		  		//if(flag==1)printf("Old One\n");
			  	
			  	

				if(flag == 0 ){
			  		struct client cli;
			  		cli.fd=newcd;cli.alive=1;strcpy(cli.name,buffer);
			  		cli.IP = clientname.sin_addr.s_addr;
			  		memset(&cli.old_message,0,S_BUFF_SIZE);
			  		//memcpy( &(cli.client_addr) ,&clientname,sizeof (clientname));
			  		List.push_back(cli);
			  	}


                FD_SET (newcd, &active_fd_set);
              }
            else
              {
                /* Data arriving on an already-connected socket. */
                if (read_from_client (i) < 0)
                  {
                    close (i);
                    FD_CLR (i, &active_fd_set);
                  }
              }
        }

	}

	return 0;

}
int read_from_client (int filedes)
{
	char sender_name[S_BUFF_SIZE];
	memset(&sender_name,0,S_BUFF_SIZE);
	list<struct client>::iterator it;
	for(it=List.begin(); it != List.end(); ++it){
		if((*it).fd == filedes )
			strncpy(sender_name,(*it).name,S_BUFF_SIZE);
	}
	//printf("sender name is %s!!\n",sender_name);
  	char buffer[S_BUFF_SIZE];
  	memset(&buffer,0,S_BUFF_SIZE);
	int nbytes;

	nbytes = read (filedes, buffer, S_BUFF_SIZE);
	if (nbytes < 0)
	{
		/* Read error. */
		perror ("read_error");
		exit (EXIT_FAILURE);
	}
	else if (nbytes == 0){
		char tempbuffer[S_BUFF_SIZE];
		memset(&tempbuffer,0,S_BUFF_SIZE);
		strcat(tempbuffer,sender_name);
		strcat(tempbuffer," is off line!!!!!");
		printf("%s is offline!!\n",sender_name);
		for(it=List.begin(); it != List.end(); ++it){
			if( strcmp(sender_name,(*it).name)==0 )(*it).alive = 0;
			if((*it).alive == 1 )
				write((*it).fd, &tempbuffer, strlen(tempbuffer) );
		}
		
		/* End-of-file. */
		return -1;
	}
	else
	{
		/* Data read. */
		//fprintf (stdout, "Server: got message: %s\n", buffer);
		int l,i,j,r;
		for(l=0;l<strlen(buffer);l++){
			if(buffer[l]=='"')break;
		}

		for(r=l+1;r<strlen(buffer);r++){
			if(buffer[r]=='"')break;
		}
		char name[S_BUFF_SIZE];
		char message[S_BUFF_SIZE];
		//printf("l=%d r=%d",l,r);
		for(i=5,j=0;i<l;i++,j++)
			name[j]=buffer[i];
		name[j]='\0';
		for(i=l+1,j=0;i<r;i++,j++)
			message[j]=buffer[i];
		message[j]='\0';

		//fprintf (stdout, "Server: name_set=!%s!\nmessage=!%s!\n", name,message);


		char *delim = " ";
		char * pch;
		//printf ("Splitting string \"%s\" into tokens:\n",str);
		pch = strtok(buffer,delim);
		
		char tempbuffer[S_BUFF_SIZE];
		memset(&tempbuffer,0,S_BUFF_SIZE);
		
		strcat(tempbuffer,sender_name);
		//printf("%s!!\n",tempbuffer);
		strcat(tempbuffer," : ");
		strcat(tempbuffer,message);
		//printf("%s!!\n",tempbuffer);
		while (pch != NULL)
		{
			if(  strncmp(pch,"\"",1) ==0)break;
			if(  strncmp(pch,"chat",4) ==0){
				pch = strtok (NULL, delim);continue;
			}
			//printf ("name is %s\n",pch);
			
			int find_flag=0;
			for(it=List.begin(); it != List.end(); ++it){
				if(strcmp(pch,(*it).name) == 0 ){
					if((*it).alive==1){/**online**/
						write((*it).fd, &tempbuffer, strlen(tempbuffer) );
						find_flag=1;
					}
					else{/**offline**/
						find_flag=1;		
						time_t t1 = time(NULL);
						char *now = ctime(&t1);				
						strcat((*it).old_message,tempbuffer);
						strcat((*it).old_message," AT THE ");
						strcat((*it).old_message,now);
						strcat((*it).old_message,"\n");
					}

				}
			}
			


			if(find_flag==0){
				char notfind[S_BUFF_SIZE];memset(&notfind,0,S_BUFF_SIZE);
				strcat(notfind,pch);
				strcat(notfind," dosen't exist\n");
				printf ("notfind is %s\n",notfind);
				write(filedes, &notfind, strlen(notfind) );
			}
			pch = strtok (NULL, delim);
		}    

		


      return 0;
    }
}