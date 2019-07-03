#include "TCPbase_FTPheader.h"

using namespace std;



int main(){
	srand (time(NULL));
	struct sockaddr_in server_addrIpv4,client_addrIpv4;

	int SockDescriptor,ClientDescriptor;

	SockDescriptor = socket(AF_INET, SOCK_STREAM, 0);

	server_addrIpv4.sin_family = AF_INET;
	server_addrIpv4.sin_port = htons(7777);
	server_addrIpv4.sin_addr.s_addr = htonl(INADDR_ANY);

	int reuse_addr = 1;
	socklen_t reuse_addr_len = sizeof(reuse_addr);
	//solution of Bind:Address already in use , just reuse it
	setsockopt(SockDescriptor, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, reuse_addr_len);

	if(bind(SockDescriptor, (struct sockaddr *)&server_addrIpv4, sizeof(server_addrIpv4)) < 0) {
		perror("bind_error");exit(1);
	}
	if (listen(SockDescriptor, 7) < 0) {
		perror("listen_error");exit(1);
	}
	socklen_t CliAddressLen = sizeof(client_addrIpv4);
	int ChildProcessOut=0;
	while(1){

		if(ChildProcessOut==1){break;}

		pid_t  pid;
		if ( (ClientDescriptor = accept(SockDescriptor, (struct sockaddr*)&client_addrIpv4, &CliAddressLen)) < 0) {	
			perror("accept_error");exit(1);
		}

     	pid = fork();
     	switch(pid){
			case -1:{
				perror("fork_error");exit(1);
			}
     		case  0:{
     			printf("Client's IP address %s , Port is: %d\n",
						inet_ntoa(client_addrIpv4.sin_addr),(int) ntohs(client_addrIpv4.sin_port));
     			printf("In Child(return value is 0)\n");
     			close(SockDescriptor);

     			int RecBytes=0,ReadBytes=0;
     			FileSelectPacket RecSelectBuf;

     			memset((void *)&RecSelectBuf,0, sizeof(RecSelectBuf));
     			while(1){


     				memset((void *)&RecSelectBuf,0, sizeof(RecSelectBuf));
     				RecBytes = recvfrom(ClientDescriptor, (void *)&RecSelectBuf,sizeof(RecSelectBuf), 0 ,
						(struct sockaddr *)&client_addrIpv4, &CliAddressLen);


     				if(RecBytes == 0 ){
						if(RecBytes == 0 )printf("RecBytes == 0 \n");
						printf("Client quit\n");
						break;
					}

					FILE *FilePtr;
					FilePacket SendBuf;
					memset((void *)&SendBuf,0, sizeof(SendBuf));

     				if(RecSelectBuf.Download==1){
     					//Client Want Download
     					
     					//If not File etc...
						if ( (FilePtr = fopen(RecSelectBuf.FileName, "rb")) == NULL){
							perror("Download:fopen");
							SendBuf.RefuseFile=1;
							sendto(ClientDescriptor,(void *)&SendBuf,sizeof(SendBuf),0,
								(struct sockaddr *)&client_addrIpv4, CliAddressLen);

							continue;
						}



						if(fseek(FilePtr, 0, SEEK_END)!=0){perror("fseek1");exit(1);}
						//IMPORTANT : ftell should use in binary mode
						int CliTotalFileSize = ftell(FilePtr);
						if(CliTotalFileSize < 0){perror("ftell");exit(1);}
						//seek back to the beginning
						if(fseek(FilePtr, 0, SEEK_SET)!=0){perror("fseek2");exit(1);}
						//Set TotalFileSize
						SendBuf.TotalFileSize = CliTotalFileSize;

						//File ok Read and Send File
						while(!feof(FilePtr)){
							//Read File
							ReadBytes = fread(SendBuf.DataBuf, sizeof(char), sizeof(SendBuf.DataBuf), FilePtr);
							SendBuf.DataBufActuallySize = ReadBytes;
							printf("ReadBytes = %d\n",ReadBytes);
							//Send Packet
							sendto(ClientDescriptor,(void *)&SendBuf,sizeof(SendBuf),0,
								(struct sockaddr *)&client_addrIpv4, CliAddressLen);

						}
						fclose(FilePtr);
     				}
     				else if(RecSelectBuf.Upload==1){
     					//Client Want Upload
     					printf("RecSelectBuf.FileName = %s\n",
								RecSelectBuf.FileName);
     					if ( (FilePtr = fopen(RecSelectBuf.FileName, "wb")) == NULL){
							perror("UPload:fopen"); exit(1);
						}

						int SumBytes=0,WriteBytes=0,RecBytes=-1;
						FilePacket RecBuf;
						printf("Waiting For Client File Data\n");
						while(1){

							memset((void *)&RecBuf,0, sizeof(RecBuf));
							
							//Receive Packet
							RecBytes = recvfrom(ClientDescriptor, (void *)&RecBuf,sizeof(RecBuf), 0 ,
								(struct sockaddr *)&client_addrIpv4, &CliAddressLen);

						
							//Write Packet
							WriteBytes = fwrite(RecBuf.DataBuf, sizeof(char), RecBuf.DataBufActuallySize, FilePtr);

							SumBytes+=WriteBytes;
							printf("RecBytes = %d , SumBytes = %d , WriteBytes = %d\n",
								RecBytes,SumBytes,WriteBytes);
							
							if(RecBytes == 0 || SumBytes >= RecBuf.TotalFileSize){
								if(RecBytes == 0 )printf("RecBytes == 0 \n");
								printf("File Fin BreakEnd\n");
								break;
							}
							

						}//while(1)
						fclose(FilePtr);




     				}//RecSelectBuf.Upload==1



     			}//Child while loop
     			ChildProcessOut=1;//Client Quit and Child Process should exit
     			close(ClientDescriptor);
     		}
     		default:{
     			printf("In Parent(return value is child pid)\n");
     			close(ClientDescriptor);


     		}


     	}//Switch


	}//Big While(1)
	return 0;
}