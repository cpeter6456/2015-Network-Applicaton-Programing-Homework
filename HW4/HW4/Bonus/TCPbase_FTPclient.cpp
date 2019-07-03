#include "TCPbase_FTPheader.h"

using namespace std;

int DownloadFile(int SockDescriptor);
int UPloadFile(int SockDescriptor);


struct sockaddr_in server_addrIpv4;
int SerAddrlen;
int main(){
	char IP[NAME_SIZE];

	printf("Please Input IP Address , Default Port is 7777\n");
	scanf("%s",IP);
	SerAddrlen =sizeof(server_addrIpv4);
	server_addrIpv4.sin_family = AF_INET;
	
	server_addrIpv4.sin_port = htons(7777);//Server open 7777 port
	server_addrIpv4.sin_addr.s_addr = inet_addr(IP);

	int SockDescriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (connect(SockDescriptor, (struct sockaddr *)&server_addrIpv4, sizeof(server_addrIpv4)) < 0) {
		perror("connect_error");
	}

	printf("Connected\n");
	char Select[NAME_SIZE];
	while(1){
		printf("Do You Want Download or Upload or Quit(d/u/q):");
		scanf("%s",Select);

		if(Select[0]=='d'){

			

			DownloadFile( SockDescriptor);
		}
		else if(Select[0]=='u'){
			


			UPloadFile( SockDescriptor);
		}
		else if(Select[0]=='q'){printf("Quit\n");break;}
		else{
			printf("Error Input Please Try Again\n");
		}
	}



	
	return 0;
}
int DownloadFile(int SockDescriptor){


	FilePacket RecBuf;
	FileSelectPacket SendBuf;
	memset((void *)&SendBuf,0, sizeof(SendBuf));
	memset((void *)&RecBuf,0, sizeof(RecBuf));

	socklen_t SerAddrlen =sizeof(server_addrIpv4);

	printf("Please Input Want To Download FileName\n");
	scanf("%s",SendBuf.FileName);
	FILE *FilePtr;
	if ( (FilePtr = fopen(SendBuf.FileName, "wb")) == NULL){
		perror("Download:fopen");exit(1);
	}
	int SumBytes=0,WriteBytes=0,RecBytes=-1;
	

	SendBuf.Upload=0;
	SendBuf.Download=1;
	SendBuf.RefuseFile=0;

	//First Send Packet with Download FileName , 
	sendto(SockDescriptor,(void *)&SendBuf,sizeof(SendBuf),0,
		(struct sockaddr *)&server_addrIpv4, SerAddrlen);

	while(1){

		memset((void *)&RecBuf,0, sizeof(RecBuf));

		//Receive Packet
		RecBytes = recvfrom(SockDescriptor, (void *)&RecBuf,sizeof(RecBuf), 0 ,
			(struct sockaddr *)&server_addrIpv4, &SerAddrlen);

		if(RecBuf.RefuseFile==1){printf("File in use or not File Please try again\n");break;}
		else{
			//Write Packet

			WriteBytes = fwrite(RecBuf.DataBuf, sizeof(char), RecBuf.DataBufActuallySize, FilePtr);

			SumBytes+=WriteBytes;
			printf("SumBytes = %d , WriteBytes = %d\n",SumBytes,WriteBytes);
		}
		if(RecBytes == 0 || SumBytes >= RecBuf.TotalFileSize){
			if(RecBytes == 0 )printf("RecBytes == 0 \n");
			printf("File Fin BreakEnd\n");
			break;
		}
		

	}
	fclose(FilePtr);
	return 0;
}
int UPloadFile(int SockDescriptor){


	FilePacket SendDataBuf;
	FileSelectPacket SendBuf;
	memset((void *)&SendBuf,0, sizeof(SendBuf));
	memset((void *)&SendDataBuf,0, sizeof(SendDataBuf));

	socklen_t SerAddrlen =sizeof(server_addrIpv4);

	printf("Please Input Want To Upload FileName\n");
	scanf("%s",SendBuf.FileName);
	FILE *FilePtr;
	if ( (FilePtr = fopen(SendBuf.FileName, "rb")) == NULL){
		perror("UPload:fopen");exit(1);
	}

	if(fseek(FilePtr, 0, SEEK_END)!=0){perror("fseek1");exit(1);}
	//IMPORTANT : ftell should use in binary mode
	int CliTotalFileSize = ftell(FilePtr);
	if(CliTotalFileSize < 0){perror("ftell");exit(1);}
	//seek back to the beginning
	if(fseek(FilePtr, 0, SEEK_SET)!=0){perror("fseek2");exit(1);}
	
	int ReadBytes=0;
	
	SendBuf.Upload=1;
	SendBuf.Download=0;
	SendBuf.RefuseFile=0;
	SendBuf.TotalFileSize=CliTotalFileSize;

	//First Send Packet with Upload FileName , 
	sendto(SockDescriptor,(void *)&SendBuf,sizeof(SendBuf),0,
		(struct sockaddr *)&server_addrIpv4, SerAddrlen);

	SendDataBuf.Upload=1;
	SendDataBuf.Download=0;
	SendDataBuf.RefuseFile=0;
	SendDataBuf.TotalFileSize=CliTotalFileSize;

	while(!feof(FilePtr)){
		//Read File
		ReadBytes = fread(SendDataBuf.DataBuf, sizeof(char), sizeof(SendDataBuf.DataBuf), FilePtr);
		SendDataBuf.DataBufActuallySize = ReadBytes;
		printf("ReadBytes = %d\n",ReadBytes);
		//Send Packet
		sendto(SockDescriptor,(void *)&SendDataBuf,sizeof(SendDataBuf),0,
			(struct sockaddr *)&server_addrIpv4, SerAddrlen);

	}
	fclose(FilePtr);
	return 0;
}