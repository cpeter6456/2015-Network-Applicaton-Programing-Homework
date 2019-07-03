all:chat_server chat_client


CC = g++
chat_server:chat_server.o 
	$(CC) -o chat_server chat_server.o -lpthread
chat_client:chat_client.o 
	$(CC) -o chat_client chat_client.o -lpthread



%.o:%.cpp
	$(CC) -c -Wall $<

clean :
	rm -f *.o chat_client chat_server 
