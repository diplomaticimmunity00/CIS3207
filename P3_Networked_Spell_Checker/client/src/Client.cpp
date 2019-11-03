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

#include <queue>
#include <vector>
#include <pthread.h>
#include <mutex>

#include <fstream>

#define NUM_CLIENTS 5
#define WORDS_MAX 100

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
int connections[NUM_CLIENTS];

std::queue<std::string> client_log_queue;
pthread_mutex_t clientLock;

pthread_cond_t clientLogFill;
pthread_cond_t clientLogEmpty;

std::vector<std::string> client_dict;

void init_client_dictionary(const std::string& filename) {
	std::ifstream words(filename);
	std::string newWord;
	while(std::getline(words,newWord)) {
		client_dict.push_back(newWord);
	}
}

void push_client_log_queue(const std::string &line) {
	pthread_mutex_lock(&clientLock);
    client_log_queue.push(line);
    pthread_cond_signal(&clientLogFill);
    pthread_mutex_unlock(&clientLock);
}

int new_connection() {
	int socket_desc = socket(AF_INET, SOCK_STREAM,0);

    struct addrinfo server_info, *res;
    int socket_fd;

    memset(&server_info, 0, sizeof server_info);
    server_info.ai_family = AF_UNSPEC;
    server_info.ai_socktype = SOCK_STREAM;

    getaddrinfo("127.0.0.1", "8888", &server_info, &res);

    socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    if(connect(socket_fd, res->ai_addr, res->ai_addrlen) < 0) {
		std::cout << "Connection error, exiting" << std::endl;
		exit(0);
	}	
	return socket_fd;
}

void client_routine(int socket_fd) {

	int wordcount = random()%WORDS_MAX;

	push_client_log_queue("Starting client with wordcount "+std::to_string(wordcount)+"...\n");

	if(socket_fd == -1) {
		std::cout << "Connection error, exiting" << std::endl;
		exit(0);
	}

	char* word;
	char buffer[1024] = {0};
	std::string logstr = "";
	for(int i=0;i<wordcount;i++) {

		
		word = (client_dict.at(random()%client_dict.size())+"\n").c_str();
		if(send(socket_fd,word,strlen(word),0) < 0) std::cout << "ERROR" << std::endl;
		//defunct

		/*if(read(socket_fd,buffer,1024) <= 0) {
			push_client_log_queue("Client "+std::to_string(socket_fd)+": Server disconnected");
			pthread_cond_signal(&clientLogFill);
			close(socket_fd);
			return;
		} else {
			std::string reply(buffer);
			if(reply.find("SERV") == std::string::npos) {
			*/
			std::string w(word);
			logstr = "Client "+std::to_string(socket_fd)+": "+w;
			/*
			} else {
				continue;
			}
		}
		*/
		push_client_log_queue(logstr);
		pthread_mutex_lock(&clientLock);
		pthread_cond_signal(&clientLogFill);
		pthread_mutex_unlock(&clientLock);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	//close connection remotely
	word = "0\n";
    if(send(socket_fd,word,strlen(word),0) < 0) std::cout << "ERROR" << std::endl;
	return;
}

void client_log_thread_routine() {
	std::string line = "";
	while(1) {
		pthread_mutex_lock(&clientLock);
		while(client_log_queue.empty()) {
			pthread_cond_wait(&clientLogFill,&clientLock);
		}
		line = client_log_queue.front();
		client_log_queue.pop();
		pthread_mutex_unlock(&clientLock);
		write_log(line);
	}
}

void spawn_threads() {
	//log thread
	
	for(int i=0;i<NUM_CLIENTS;i++) {
		connections[i] = new_connection();
		std::this_thread::sleep_for(std::chrono::milliseconds(300));
	}

	pthread_t logThread;
	if(pthread_create(&logThread,NULL,client_log_thread_routine,NULL) != 0) {
            std::cout << "Failed to create thread" << std::endl;
            exit(0);
    }
	for(int i=0;i<NUM_CLIENTS;i++) {
		if(pthread_create(&client_spool[i],NULL,client_routine,connections[i]) != 0) {
			std::cout << "Failed to create thread" << std::endl;
			exit(0);
		}
	}
}

int main(int argc, char** argv) {

	srand(time(NULL));

	std::cout << "Simulating " << NUM_CLIENTS << " clients..." << std::endl;

	clear_client_log();
	init_client_dictionary("words.txt");
	spawn_threads();
	for(int i=0;i<NUM_CLIENTS;i++) {
		pthread_join(client_spool[i],NULL);
		std::cout << "Client " << i+1 << " finished" << std::endl;
	}
	push_client_log_queue("Clients finished\n");
	return 0;
}
