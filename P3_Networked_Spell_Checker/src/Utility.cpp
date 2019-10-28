#include "Utility.h"
#include <string>

void init_dictionary() {
	std::ifstream words("words.txt");
	std::string newWord;
	while(std::getline(words,newWord)) {
		dict.insert(newWord);
	}
}

std::string convert(char* old) {
    std::string newstring;
    char c = old[0];
    int i = 0;
    while(c != 0) {
        c = old[i++];
        newstring += c;
    }
    return newstring;
}

int find(const std::string &s, char c) {
    for(int i=0;i<s.size();i++) {
        if(s[i] == c) {
            return i;
        }
    }
    return -1;
}

std::string strip(const std::string &s) {
	std::string newString = "";
	for(int i=0;i<s.size();i++) {
		if(s[i] != ' ') newString += s[i];
	}
	return newString;
}
