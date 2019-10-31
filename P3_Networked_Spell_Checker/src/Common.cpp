#include "Common.h"
#include "Thread.h"

#define NUM_WORKERS 4
#define DEFAULT_PORT 8888
#define DEFAULT_DICTIONARY "words.txt"
#define SOCKETS_MAX 50

#include <queue>
#include <unordered_set>

std::unordered_set<std::string> dict;

std::queue<int> socketQueue;
pthread_mutex_t socketLock;

Spool workers = Spool(NUM_WORKERS);
pthread_cond_t fill = PTHREAD_COND_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;

int port = -1;
std::string dictionaryFile = DEFAULT_DICTIONARY;
