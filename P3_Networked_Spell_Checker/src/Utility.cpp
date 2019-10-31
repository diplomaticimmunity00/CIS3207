#include "Utility.h"
#include <string>
#include <vector>
#include <fstream>

void init_dictionary(const std::string& filename) {
	std::ifstream words(filename);
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
	//skip null terminator
    return newstring.substr(0,newstring.size()-1);
}

int find(const std::string &s, char c) {
    for(int i=0;i<s.size();i++) {
        if(s[i] == c) {
            return i;
        }
    }
    return -1;
}

int find(const std::vector<char> &v, char c) {
	for(int i=0;i<v.size();i++) {
		if(v.at(i) == c) {
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

bool isdigit(const std::string& s) {
	std::vector<char> digits = {'0','1','2','3','4','5','6','7','8','9'};
	for(int i=0;i<s.size();i++) {
		if(find(digits,s[i]) == -1) {
			return false;
		}
	}
	return true;
}

bool file_exists(const std::string &filename) {
    std::ifstream file(filename);
    return (file.is_open())? 1:0;
}
