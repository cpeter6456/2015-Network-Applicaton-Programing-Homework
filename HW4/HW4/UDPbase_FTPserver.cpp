#include "UDPbase_FTPheader.h"

using namespace std;



int main(){
	srand (time(NULL));
	struct sockaddr_in server_addrIpv4,client_addrIpv4;

	int sd;

	sd = socket(AF_INET, SOCK_DGRAM, 0);

	server_addrIpv4.sin_family = AF_INET;
	server_addrIpv4.sin_port = htons(7777);
	server_addrIpv4.sin_addr.s_addr = htonl(INADDR_ANY);


	int reuse_addr = 1;
	socklen_t reuse_addr_len = sizeof(reuse_addr);
	//solution of Bind:Address already in use , just reuse it
	setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, reuse_addr_len);

	if(bind(sd, (struct sockaddr *)&server_addrIpv4, sizeof(server_addrIpv4)) < 0) {
		perror("bind_error");exit(1);
	}

	int RecBytes=0,SenBytes=0;
	TcpPacket TcpBuf;
	FILE *FilePtr;
	//FileSequence is Server want accept packet sequence (1~n)
	int FileSize=0,FileSequence=1;	
	

	//int FirstPacket=1;
	while(1){
		FileSequence=1;FileSize=0;RecBytes=0;SenBytes=0;FilePtr=NULL;
		printf("\n\nWait NewFile\n");
		while(1){
			memset((void *)&TcpBuf,0, sizeof(TcpBuf));
			RecBytes=0;SenBytes=0;

			socklen_t CliAddrlen = sizeof(client_addrIpv4);
			printf("\nrecvfrom\n");
			RecBytes = recvfrom(sd, (void *)&TcpBuf,sizeof(TcpBuf), 0 ,
		            (struct sockaddr *)&client_addrIpv4, &CliAddrlen);

			if(RecBytes < 0){perror("recvfrom:RecBytes<0");exit (1);}
			if(RecBytes == 0){printf("RecBytes = 0\n");}

			//Droping Packet

			float p	= (random()%100)/100.0;
			printf("p = %f\n",p);
			if(p < 0.1){
				TcpAckPacket NextBuf;
				memset((void *)&NextBuf,0, sizeof(NextBuf));
				NextBuf.Next=1;
				NextBuf.IsAck=0;
				SenBytes = sendto(sd,(void *)&NextBuf,sizeof(NextBuf),0,
	                      (struct sockaddr *)&client_addrIpv4, CliAddrlen);
				//printf("Drop Server should not know\n");
				continue;
			}
			printf("RecBytes Packet = %d\n",RecBytes);
			
			if(FileSequence<TcpBuf.PacketSequence || TcpBuf.CliEndCheck == 1){
				//When before packet drop , the next packet will know packet drop  
				printf("Before packet drop , FileSequence = %d\n",FileSequence);
				TcpAckPacket AckBuf;
				memset((void *)&AckBuf,0, sizeof(AckBuf));

				//FileSequence is Server want accept packet sequence (1~n)
				AckBuf.Next=0;
				AckBuf.AckSequence = FileSequence;
				AckBuf.IsAck=1;
				//memcpy((void *)&(AckBuf.SrcAddrIpv4),,sizeof());

				SenBytes = sendto(sd,(void *)&AckBuf,sizeof(AckBuf),0,
	                      (struct sockaddr *)&client_addrIpv4, CliAddrlen);
				printf("SenBytes Packet = %d\n",SenBytes);
			}
			else if(FileSequence == TcpBuf.PacketSequence){
				printf("Client's IP address %s , Port is: %d\n",
						inet_ntoa(client_addrIpv4.sin_addr),(int) ntohs(client_addrIpv4.sin_port));

				if(FileSequence == 1){
					if ( (FilePtr = fopen(TcpBuf.FileName, "wb")) == NULL){
						perror("fopen");exit(1);
					}
					
				}
				if(FilePtr == NULL){perror("FilePtr == NUL");exit(1);}
				//Success and send ack 
				//Only success will write data to file 
				//So Client change it's sequence to same with Server's ACK sequence
				
				int WriteByte = fwrite(TcpBuf.DataBuf, sizeof(char),TcpBuf.DataBufActuallySize,FilePtr);
				printf("WriteByte = %d , DataBufActuallySize = %d , TotalFileSize = %d\n",
					WriteByte,TcpBuf.DataBufActuallySize,TcpBuf.TotalFileSize);
				FileSize+=WriteByte;

				//Send Ack
				TcpAckPacket AckBuf;
				//Success so FileSequence++
				FileSequence++;
				AckBuf.Next=0;
				AckBuf.AckSequence = FileSequence;
				AckBuf.IsAck=1;

				printf("FileSequence = %d , TcpBuf.PacketSequence = %d\n",FileSequence,TcpBuf.PacketSequence);


				SenBytes = sendto(sd,(void *)&AckBuf,sizeof(AckBuf),0,
	                      (struct sockaddr *)&client_addrIpv4, CliAddrlen);


				printf("SenBytes Packet = %d\n",SenBytes);
				//If File End
				if(FileSize >= TcpBuf.TotalFileSize){
					printf("FileEnd  Close and Break inner While\n");
					fclose(FilePtr);
					break;
				}
			}
			else{}
			
		}//Inner While
	}//Out While 

	return 0;
}