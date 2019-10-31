#pragma once
#include <pthread.h>
#include <vector>
#include <iostream>

struct Worker {

    int id;
    pthread_t thread;

    bool occupied;

    Worker(int _id): id(_id),occupied(false) {}

	void work_from_queue();
};

//wrapper function
void worker_routine(Worker*);
void push_socket_queue(int);

struct Spool {

	int size;
	bool ready = false;
	std::vector<Worker*> workers;
	void *(*routine) (void *);

	void activate() {
		for(size_t i=0;i<this->size;i++) {
			if(pthread_create(&this->workers.at(i)->thread,NULL,worker_routine,this->workers.at(i)) != 0) {
				std::cout << "Failed to create thread" << std::endl;
				exit(0);
			}
		}
		this->ready = true;
	}

	int free_threads() {
		int free = 0;
		for(int i=0;i<this->workers.size();i++) {
            if(!this->workers.at(i)->occupied) {
                free++;
            }
        }
		return free;
	}

	int occupied_threads() {
		return this->size-this->free_threads();
	}

	bool has_free_thread() {
		return this->free_threads() > 0;
	}
		
	Spool(int _size): size(_size) {
		for(int i=0;i<this->size;i++) {
			this->workers.push_back(new Worker(i+1));
		}
	}

};
