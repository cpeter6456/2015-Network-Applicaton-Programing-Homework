all:server client


CC = g++
server:server.o fixed_huf.o
	$(CC) -o server server.o fixed_huf.o
client:client.o fixed_huf.o
	$(CC) -o client client.o fixed_huf.o



%.o:%.cpp
	$(CC) -c -Wall $<

clean :
	rm -f *.o client server 
