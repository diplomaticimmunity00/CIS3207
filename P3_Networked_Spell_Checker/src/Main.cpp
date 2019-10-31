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
#include "Thread.h"

#define DEFAULT_PORT 8888
#define DEFAULT_DICTIONARY "words.txt"

void start_log_thread();

int main(int argc, char** argv) {

	for(int i=1;i<argc;i++) {
		if(isdigit(convert(argv[i]))) {
			if(port < 0) port=std::stoi(argv[i]);
		} else {
			dictionaryFile = convert(argv[i]);
		}
	}

	if(port < 0) port = DEFAULT_PORT;

	if(!file_exists(dictionaryFile)) {
		dictionaryFile = DEFAULT_DICTIONARY;
		if(!file_exists(dictionaryFile)) {
			print("Error: Dictionary not found, exiting");
			exit(0);
		}
	}

	clear_log();

	init_dictionary(dictionaryFile);
	
	start_log_thread();
	workers.activate();
	//while(!workers.ready);

	pthread_mutex_init(&socketLock,NULL);

	//pthread_cond_init(&fill,NULL);
	//pthread_cond_init(&empty,NULL);

	print("Starting server...");

	int new_socket, c;
	struct sockaddr_in server, client;

	int socket_desc = socket(AF_INET, SOCK_STREAM,0);

	if(socket_desc == -1) {
		print("SOCK ERROR");
		exit(0);
	}

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);
	if(bind(socket_desc, (struct sockaddr*)&server, sizeof(server)) < 0) {
		print("ERROR BINDING");
		exit(0);
	}

	listen(socket_desc, 3);

	print("Listening on " + std::to_string(ntohs(server.sin_port)) + "...");

	while(1) {
		c = sizeof(struct sockaddr_in);
		new_socket = accept(socket_desc, (struct sockaddr*)&client, (socklen_t*)&c);
		print("Received connection");
		if(new_socket < 0) {
			print("CONNECTION ERROR");
			exit(0);
		}
		std::string occupied = std::to_string(workers.occupied_threads());
		std::string total = std::to_string(workers.size);
		std::string greeting = "Connected to server! ("+occupied+"/"+total+")\n";
		char* hello = greeting.c_str();
		send(new_socket , hello , strlen(hello) , 0 );
		push_socket_queue(new_socket);
		print("Signaling a thread...");
		pthread_cond_signal(&fill);
		if(!workers.has_free_thread()) {
			print("All threads occupied, notifying client");
			char* wait = "Waiting for a slot to open...\n";
			send(new_socket , wait , strlen(wait) , 0 );
		}
	}
	print("Server closing");
	return 0;
}
