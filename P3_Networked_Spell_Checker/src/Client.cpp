#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <random>

#include <chrono>
#include <thread>

#include <vector>
#include <pthread.h>
#include <mutex>

#include <fstream>

#include "Utility.h"
#include "Thread.h"
#include "Common.h"

#define NUM_CLIENTS 10
#define WORDS_MAX 120

void clear_client_log() {
    std::ofstream log("client.log");
    log << "";
    log.close();
}

void write_log(const std::string &s) {
	std::ofstream log("client.log",std::ios_base::app);

    if(!log.is_open()) {
        std::cerr << "ERROR writing to logfile" << std::endl;
        return;
    } else {
        log << s;
        log.close();
    }
}

pthread_t client_spool[NUM_CLIENTS];

pthread_mutex_t clientLock;

std::vector<std::string> client_dict;

void init_client_dictionary(const std::string& filename) {
	std::ifstream words(filename);
	std::string newWord;
	while(std::getline(words,newWord)) {
		client_dict.push_back(newWord);
	}
}

void client_routine() {

	int wordcount = random()%WORDS_MAX;

	print("Starting client...");

	int new_socket, c;
	struct sockaddr_in server, client;

	int socket_desc = socket(AF_INET, SOCK_STREAM,0);

	struct addrinfo hints, *res;
	int sockfd;

	// first, load up address structs with getaddrinfo():

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	getaddrinfo("127.0.0.1", "8888", &hints, &res);

	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	connect(sockfd, res->ai_addr, res->ai_addrlen);

	if(socket_desc == -1) {
		print("SOCK ERROR");
		exit(0);
	}
	
	char* word;
	char buffer[1024] = {0};
	for(int i=0;i<wordcount;i++) {
		word = (client_dict.at(random()%client_dict.size())+"\n").c_str();
		send(sockfd,word,strlen(word),0);

		//if(random()%2 == 1) std::this_thread::sleep_for(std::chrono::milliseconds(10));

		pthread_mutex_lock(&clientLock);
		if(0) {
			write_log("Server disconnected");
			pthread_mutex_unlock(&clientLock);
			return;
		} else {
			write_log("Client "+std::to_string(sockfd)+": "+convert(word));
		}
		pthread_mutex_unlock(&clientLock);
	}

	word = "0\n";
    send(sockfd,word,strlen(word),0);
	close(sockfd);
}

void spawn_threads () {
	for(size_t i=0;i<NUM_CLIENTS;i++) {
		if(pthread_create(&client_spool[i],NULL,client_routine,NULL) != 0) {
			std::cout << "Failed to create thread" << std::endl;
			exit(0);
		}
	}
}

int main(int argc, char** argv) {
	clear_client_log();
	init_client_dictionary("words.txt");
	spawn_threads();
	for(int i=0;i<NUM_CLIENTS;i++) {
		pthread_join(client_spool[i],NULL);
	}
	print("Clients finished");
	return 0;
}
