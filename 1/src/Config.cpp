#include "Config.h"

int find(const std::string &s, char c) {
    for(int i=0;i<s.size();i++) {
        if(s[i] == c) return i;
    }
    return -1;
}
