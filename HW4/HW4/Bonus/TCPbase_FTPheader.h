
#ifndef FTP_HEADER
#define FTP_HEADER

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
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include <sys/select.h>
#include <list>
using namespace std;
#define NAME_SIZE 128
#define S_BUFF_SIZE 256
#define ALL_NAME_SIZE 256
#define DATA_BUF_SIZE 1024

struct FilePacket
{
	int Upload;
	int Download;
	int RefuseFile;

	int TotalFileSize;
	int DataBufActuallySize;

	char FileName[NAME_SIZE];
	char DataBuf[DATA_BUF_SIZE];
};
struct FileSelectPacket
{
	int Upload;
	int Download;
	int RefuseFile;
	
	int TotalFileSize;
	int DataBufActuallySize;

	char FileName[NAME_SIZE];

};


#endif

