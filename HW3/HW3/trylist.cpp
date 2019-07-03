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
#include <iostream>
#include <sys/select.h>
#include <list>
using namespace std;
struct client
{
    struct sockaddr_in client_addr;
    int fd;
    int alive;
};
int main(){

	list<client> List;

	for(int i=0;i<20;i++){
		struct client nclient;
		nclient.fd=i;
		List.push_back(nclient);
	}
	list<client>::iterator i;
	cout << "ddd";
	for(i=List.begin(); i != List.end(); i++) cout << (*i).fd << "!!";
	cout << List.size();
	return 0;
}