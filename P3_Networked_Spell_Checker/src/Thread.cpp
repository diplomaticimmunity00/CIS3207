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

void push_print_queue(std::string line) {
	pthread_mutex_lock(&printLock);
	printQueue.push(line);
	pthread_cond_signal(&fillJobs);
	pthread_mutex_unlock(&printLock);
}

void close_socket(int socket) {
	close(socket);
	push_print_queue("CLOSING SOCKET "	+ std::to_string(socket));
}

int pop_socket_queue() {
	//pthread_mutex_lock(&socketLock);
	int new_socket = socketQueue.front();
	socketQueue.pop();
	//pthread_mutex_unlock(&socketLock);
	return new_socket;
}

std::string pop_print_queue() {
	//pthread_mutex_lock(&printLock);
	std::string line = printQueue.front();
	printQueue.pop();
	//pthread_mutex_unlock(&printLock);
	return line;
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

void await_job(std::string &line) {

	//wait until a socket is available
	pthread_mutex_lock(&printLock);
	while(printQueue.empty()) {
		//print("Sleeping thread "+std::to_string(w->id)+"...");
		pthread_cond_wait(&fillJobs,&printLock);
	}
	line = pop_print_queue();
	pthread_cond_signal(&emptyJobs);
	pthread_mutex_unlock(&printLock);
}


void push_socket_queue(int socket) {
	pthread_mutex_lock(&socketLock);
	socketQueue.push(socket);
	pthread_mutex_unlock(&socketLock);
}

void write_to_log(std::string line) {

	//print(line);return;

	std::ofstream log(logfile,std::ios_base::app);
	if(!log.is_open()) {
		std::cerr << "ERROR writing to logfile" << std::endl;
		return;
	} else {
		char* timestamp = get_timestamp();
		log << " -" << line << std::endl;
		log.close();
	}
}

void log_thread_routine() {
	std::string line = "";
	while(1) {
		await_job(line);
		write_to_log(line);
	}
}

void start_log_thread() {
	pthread_t thread;
	if(pthread_create(&thread,NULL,log_thread_routine,NULL) != 0) {
		std::cout << "Failed to create thread" << std::endl;
		exit(0);
	}
}

void Worker::work_from_queue() {

	std::string message = "";
	std::string check = "";
	char buffer[1024] = {0};
	int new_socket;

	while(1) {
		//wait until a socket is available
		await_socket(new_socket,this);
		push_print_queue("SERVICING SOCKET "+std::to_string(new_socket)+" FROM THREAD "+std::to_string(this->id));
		char* accepted = "Begin sending words!\n";
		send(new_socket, accepted, strlen(accepted), 0 );
		while(read(new_socket,buffer,1024) > 0) {
			push_print_queue("Received word from socket "+std::to_string(new_socket));
			message = strip(convert(buffer));
			message = message.substr(0,find(message,'\n'));
			if(message == "0") {
				push_print_queue("Received disconnect from socket "+std::to_string(new_socket));
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
