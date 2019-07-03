all:UDPbase_FTPserver UDPbase_FTPclient


CC = g++
UDPbase_FTPserver:UDPbase_FTPserver.o 
	$(CC) -o UDPbase_FTPserver UDPbase_FTPserver.o -lpthread
UDPbase_FTPclient:UDPbase_FTPclient.o 
	$(CC) -o UDPbase_FTPclient UDPbase_FTPclient.o -lpthread



%.o:%.cpp
	$(CC) -c -Wall $<

clean :
	rm -f *.o UDPbase_FTPclient UDPbase_FTPserver 
