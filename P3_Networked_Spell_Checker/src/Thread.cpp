#include "Thread.h"
#include "Utility.h"
#include "Common.h"

#include <queue>
#include <pthread.h>
#include <mutex>

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

void close_socket(int socket) {
	close(socket);
	print("CLOSING SOCKET "  + std::to_string(socket));
}

int pop_socket_queue() {
	//pthread_mutex_lock(&socketLock);
	int new_socket = socketQueue.front();
	socketQueue.pop();
	//pthread_mutex_unlock(&socketLock);
	return new_socket;
}

void await_socket(int &socket,Worker* w) {

	//wait until a socket is available
	pthread_mutex_lock(&socketLock);
	w->occupied = false;
	while(socketQueue.empty()) {
		//print("Sleeping thread "+std::to_string(w->id)+"...");
		pthread_cond_wait(&fill,&socketLock);
	}
	socket = socketQueue.front();
	socketQueue.pop();
	pthread_cond_signal(&empty);
	w->occupied = true;
	pthread_mutex_unlock(&socketLock);
}

void push_socket_queue(int socket) {
	pthread_mutex_lock(&socketLock);
	socketQueue.push(socket);
	pthread_mutex_unlock(&socketLock);
}

void Worker::work_from_queue() {

	std::string message = "";
	std::string check = "";
	char buffer[1024] = {0};
	int new_socket;

	while(1) {
		//wait until a socket is available
		await_socket(new_socket,this);
		print("SERVICING SOCKET "+std::to_string(new_socket)+" FROM THREAD "+std::to_string(this->id));
		char* accepted = "Begin sending words!\n";
		send(new_socket, accepted, strlen(accepted), 0 );
		while(read(new_socket,buffer,1024) > 0) {
			print("Received word from socket "+std::to_string(new_socket));
			message = strip(convert(buffer));
			message = message.substr(0,find(message,'\n'));
			if(message == "0") {
				print("Received disconnect from socket "+std::to_string(new_socket));
				char* goodbye = "Closed connection to server.\nGoodbye!\n";
				send(new_socket, goodbye, strlen(goodbye), 0 );
				break;
			}
			if(dict.find(message) != dict.end()) {
				check = "\r" + message + " OK\n";
			} else {
				check = "\r" + message + " MISSPELLED\n";
			}
			send(new_socket, check.c_str(), strlen(check.c_str()), 0);
		}
		close_socket(new_socket);
	}
}

void worker_routine(Worker* w) {
	w->work_from_queue();
}
