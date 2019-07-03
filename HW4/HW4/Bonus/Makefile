all:TCPbase_FTPserver TCPbase_FTPclient


CC = g++
UDPbase_FTPserver:TCPbase_FTPserver.o 
	$(CC) -o TCPbase_FTPserver TCPbase_FTPserver.o -lpthread
UDPbase_FTPclient:UDPbase_FTPclient.o 
	$(CC) -o TCPbase_FTPclient TCPbase_FTPclient.o -lpthread



%.o:%.cpp
	$(CC) -c -Wall $<

clean :
	rm -f *.o TCPbase_FTPclient TCPbase_FTPserver 
