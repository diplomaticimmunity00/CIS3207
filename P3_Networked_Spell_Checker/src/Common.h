#pragma once

#define NUM_WORKERS 4
#define DEFAULT_PORT 8888
#define DEFAULT_DICTIONARY "words.txt"
#define SOCKETS_MAX 4

#include <queue>
#include <unordered_set>
#include <pthread.h>
#include <string>

#include "Thread.h"


extern std::unordered_set<std::string> dict;

extern std::queue<int> socketQueue;
extern std::queue<std::string> printQueue;

extern pthread_mutex_t printLock;
extern pthread_mutex_t socketLock;

extern unsigned int count;

extern std::string logfile;

extern Spool workers;
extern pthread_cond_t fill;
extern pthread_cond_t empty;

extern pthread_cond_t fillJobs;
extern pthread_cond_t emptyJobs;

extern int port;
extern std::string dictionaryFile;
