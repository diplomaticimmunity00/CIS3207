#include "Utility.h"
#include "disk.h"

#include <string>
#include <vector>
#include <fstream>
#include <ctime>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

void clear_log(const std::string &filename) {
    std::ofstream log(filename);
    log << "";
    log.close();
}

std::vector<std::string> split(char c, const std::string& s) {

	std::vector<std::string> v;
	std::string thisEntry = "";
	for(int i=0;i<s.size();i++) {
		if(s[i] == c) {
			v.push_back(thisEntry);
			thisEntry = "";
		} else {
			thisEntry += s[i];
		}
	}
	if(thisEntry != "") {
		v.push_back(thisEntry);
	}
	return v;
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
		if(s[i] != ' ' and s[i] != '\t') newString += s[i];
	}
	return newString;
}

void clear_buffer(char *buf,size_t nbyte) {
	int fd = open("/dev/zero", O_RDONLY);
	read(fd, buf, nbyte);
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
