#include "UDPbase_FTPheader.h"

using namespace std;

int main(){
	char CliFileName[NAME_SIZE],IP[NAME_SIZE];
	printf("Please Input Server IP address\n");
	scanf("%s",IP);
	printf("Default Port is 7777\n");
	
	while(1){	

		printf("Please Input Want To Transmit FileName or quit(Q or q)\n");
		scanf("%s",CliFileName);
		if(CliFileName[0]=='Q' || CliFileName[0]=='q' )break;
		FILE *FilePtr;
		if ( (FilePtr = fopen(CliFileName, "rb")) == NULL){
			perror("fopen");exit(1);
		}

		struct sockaddr_in server_addrIpv4;
		
		server_addrIpv4.sin_family = AF_INET;
		//Server open 7777 port
		server_addrIpv4.sin_port = htons(7777);
		server_addrIpv4.sin_addr.s_addr = inet_addr(IP);

		int SockDescriptor = socket(AF_INET, SOCK_DGRAM, 0);

		int ReadBytes=0,CliSequence=1;
		TcpPacket TcpBuf;
		TcpAckPacket RecBuf;
		if(fseek(FilePtr, 0, SEEK_END)!=0){perror("fseek1");exit(1);}
		//IMPORTANT : ftell should use in binary mode
		int CliTotalFileSize = ftell(FilePtr);
		if(CliTotalFileSize < 0){perror("ftell");exit(1);}
		//seek back to the beginning
		if(fseek(FilePtr, 0, SEEK_SET)!=0){perror("fseek2");exit(1);}

		TcpBuf.TotalFileSize = CliTotalFileSize;
		TcpBuf.Next=0;TcpBuf.IsAck=0;
		memcpy( (void*)&(TcpBuf.FileName),CliFileName,sizeof(CliFileName));
		memcpy((void *)&(TcpBuf.DesAddrIpv4),&server_addrIpv4,sizeof(server_addrIpv4));

		socklen_t SerAddrlen =sizeof(server_addrIpv4);

		int LastServerAckNumber=0;
		while(!feof(FilePtr)){
			TcpBuf.CliEndCheck=0;
			//Clean DataBuf
			memset((void *)&(TcpBuf.DataBuf),0, sizeof(DATA_BUF_SIZE));


			ReadBytes = fread(TcpBuf.DataBuf , sizeof(char), DATA_BUF_SIZE , FilePtr);
			printf("\nReadBytes(From File) = %d\n", ReadBytes);


			TcpBuf.PacketSequence = CliSequence;
			TcpBuf.DataBufActuallySize = ReadBytes;
			//Send To Server
			int SenBytes = sendto(SockDescriptor,(void *)&TcpBuf,sizeof(TcpBuf),0,
		              (struct sockaddr *)&server_addrIpv4, SerAddrlen);
			printf("SenBytes = %d  , ", SenBytes);
			
			

			int RecBytes = recvfrom(SockDescriptor, (void *)&RecBuf,sizeof(RecBuf), 0 ,
		        (struct sockaddr *)&server_addrIpv4, &SerAddrlen);
			printf("RecBytes = %d\n", RecBytes);
			if(RecBuf.IsAck == 1)
				printf("RecBuf.AckSequence = %d , CliSequence = %d(will ++ ) \n",RecBuf.AckSequence,CliSequence);
			
			CliSequence++;

			if(RecBuf.IsAck == 1){

				if(RecBuf.AckSequence < CliSequence){
				//Occur at Server Know that before packet have loss

				//So Retransmit all packet(loss & after loss)
					printf("Occur at Server Know that before packet have loss\n");
					CliSequence = RecBuf.AckSequence;
					LastServerAckNumber = RecBuf.AckSequence;
					if(fseek(FilePtr, (CliSequence-1)*DATA_BUF_SIZE , SEEK_SET) !=0 ){
						perror("fseek3");exit(1);
					}

				}
				else if(RecBuf.AckSequence == CliSequence ){
				//Occur at Server had Receive Packet That I Send at This Time
					LastServerAckNumber = RecBuf.AckSequence;

				}
				else{
					perror("RecBuf.AckSequence > CliSequence !!!");exit(1);
				}
			}
			else{
				//IsAck==0
				printf("IsAck == 0\n");
			}

			//Solution of Server loss packet at FileEnd packet
			//CliEndCheck is only at Client fin , but know Server not(because Server's Ack sequence not enough)
			if(feof(FilePtr) && LastServerAckNumber < CliSequence){
				while(1){
					printf("End Check !! LastServerAckNumber = %d\n",LastServerAckNumber);
					memset((void *)&(TcpBuf.DataBuf),0, sizeof(DATA_BUF_SIZE));

					TcpBuf.CliEndCheck=1;

					sendto(SockDescriptor,(void *)&TcpBuf,sizeof(TcpBuf),0,
			              (struct sockaddr *)&server_addrIpv4, SerAddrlen);

					recvfrom(SockDescriptor, (void *)&RecBuf,sizeof(RecBuf), 0 ,
			        (struct sockaddr *)&server_addrIpv4, &SerAddrlen);


					//Occure at Server loss this packet
					if(RecBuf.IsAck==0)continue;
					if(RecBuf.IsAck == 1){
					//In Here , IsAck == 1 , means Server know had loss packet , and Server Send Ture Sequence
					//So Retransmit all packet(loss & after loss)
						printf("Occur at Server Know that before packet have loss\n");
						printf("Ack RecBuf.AckSequence = %d\n",RecBuf.AckSequence);
						CliSequence = RecBuf.AckSequence;
						if(fseek(FilePtr, (CliSequence-1)*DATA_BUF_SIZE , SEEK_SET) !=0 ){
							perror("fseek3");exit(1);
						}
						break;
					}
				}

			}

		}
		

	fclose(FilePtr);

	}

	return 0;
}