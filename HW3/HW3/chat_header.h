
#ifndef CHAT_HEADER
#define CHAT_HEADER

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

#define S_BUFF_SIZE 256
#define ALL_NAME_SIZE 256
struct chat_node
{

	char name_set[ALL_NAME_SIZE];
    char message[S_BUFF_SIZE];
};


#endif

