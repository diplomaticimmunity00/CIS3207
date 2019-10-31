#pragma once

#include <queue>
#include <unordered_set>
#include <pthread.h>

#include "Thread.h"


extern std::unordered_set<std::string> dict;

extern std::queue<int> socketQueue;
extern pthread_mutex_t socketLock;
extern int count;

extern Spool workers;
extern pthread_cond_t fill;
extern pthread_cond_t empty;

extern int port;
extern std::string dictionaryFile;
