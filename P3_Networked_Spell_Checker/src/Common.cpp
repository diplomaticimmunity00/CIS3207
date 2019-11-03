#include "Common.h"
#include "Thread.h"

#include <queue>
#include <unordered_set>

std::unordered_set<std::string> dict;

std::queue<std::string> printQueue;
std::queue<int> socketQueue;
pthread_mutex_t socketLock;
pthread_mutex_t printLock;

unsigned int count = 0;

Spool workers = Spool(NUM_WORKERS);
pthread_cond_t fill = PTHREAD_COND_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;

pthread_cond_t fillJobs = PTHREAD_COND_INITIALIZER;
pthread_cond_t emptyJobs = PTHREAD_COND_INITIALIZER;

std::string logfile = "latest.log";

int port = -1;
std::string dictionaryFile = DEFAULT_DICTIONARY;
