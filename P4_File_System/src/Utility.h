#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>

void init_dictionary(const std::string&);

char* get_timestamp();

void clear_log(const std::string&);

template <class T>
void print(T t) {

	std::cout << t << std::endl;
	
}

void clear_buffer(char*,size_t);

template <class T>
int find(const std::vector<T> &v, const T &t) {
	for(int i=0;i<v.size();i++) {
		if(v.at(i) == t) return i;
	}
	return -1;
}

template <class T>
int remove(std::vector<T> &v, const T &t) {
	for(int i=0;i<v.size();i++) {
		if(v.at(i) == t) v.erase(v.begin()+i);
		return i;
	}
	return -1;
}


std::vector<std::string> split(char c,const std::string&);

std::string convert(char*);

int find(const std::string&,char);

std::string strip(const std::string&);

bool isdigit(const std::string&);

bool file_exists(const std::string &filename);
