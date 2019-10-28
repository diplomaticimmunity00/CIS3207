#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h>
#include <string.h>

#include <queue>
#include <pthread.h>
#include <mutex>

#include "Utility.h"

#define THREADS 3

std::queue<int> socketQueue;
pthread_mutex_t socketLock;

pthread_cond_t fill;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;

void work_from_queue();

void spawn_threads() {
	pthread_t threads[THREADS];
	for(size_t i=0;i<THREADS;i++) {
		if(pthread_create(&threads[i],NULL,work_from_queue,NULL) != 0) {
			std::cout << "Failed to create thread" << std::endl;
			exit(0);
		}
	}
}

void close_socket(int socket) {
	char* hello = "Closing connection to server\n";
    send(socket , hello , strlen(hello) , 0 );
    close(socket);
    std::cout << "Closing socket " << socket << std::endl;
}

int pop_socket_queue() {
	//pthread_mutex_lock(&socketLock);
    int new_socket = socketQueue.front();
	socketQueue.pop();
	//pthread_mutex_unlock(&socketLock);
	return new_socket;
}

void push_socket_queue(int socket) {
	socketQueue.push(socket);
	pthread_cond_signal(&fill);
}

void work_from_queue() {

	std::string message = "";
    std::string check;
    char buffer[1024] = {0};
	int new_socket = 100;

	while(1) {
		//wait until a socket is available
		pthread_mutex_lock(&socketLock);
		while(socketQueue.empty()) {
			pthread_cond_wait(&fill,&socketLock);
		}

		new_socket = pop_socket_queue();
		pthread_cond_signal(&empty);
		pthread_mutex_unlock(&socketLock);
		print("SERVICING SOCKET "+std::to_string(new_socket)+" FROM THREAD "+std::to_string(pthread_self()));
		while(read(new_socket,buffer,1024) >= 0) {
			message = strip(convert(buffer));
			message = message.substr(0,find(message,'\n'));
			if(dict.find(message) != dict.end()) {
				check = "\r" + message + " OK\n";
			} else {
				check = "\r" + message + " MISSPELLED\n";
			}
			send(new_socket, check.c_str(), strlen(check.c_str()), 0);
		}
		print("CLOSING SOCKET "+std::to_string(new_socket));
		close_socket(new_socket);
	}
}

int main() {

	init_dictionary();
	spawn_threads();

	pthread_mutex_init(&socketLock,NULL);

	pthread_cond_init(&fill,NULL);
	pthread_cond_init(&empty,NULL);

	int new_socket, c;
	struct sockaddr_in server, client;

	int socket_desc = socket(AF_INET, SOCK_STREAM,0);

	if(socket_desc == -1) {
		std::cout << "SOCK ERROR" << std::endl;
		exit(0);
	}

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(8888);
	if(bind(socket_desc, (struct sockaddr*)&server, sizeof(server)) < 0) {
		std::cout << "ERROR BINDING" << std::endl;
		exit(0);
	}

	listen(socket_desc, 3);

	std::cout << "Listening on " << ntohs(server.sin_port) << "..." << std::endl;

	while(1) {
		c = sizeof(struct sockaddr_in);
		new_socket = accept(socket_desc, (struct sockaddr*)&client, (socklen_t*)&c);
		std::cout << "Received connection" << std::endl;
		if(new_socket < 0) {
			std::cout << "CONNECTION ERROR" << std::endl;
			exit(0);
		}
		char* hello = "Connected to server!\nBegin sending words...\n";
    	send(new_socket , hello , strlen(hello) , 0 );
		push_socket_queue(new_socket);
	}
	return 0;
}
