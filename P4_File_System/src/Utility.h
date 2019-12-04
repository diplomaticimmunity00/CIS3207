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

std::vector<std::string> split(char c,const std::string&);

std::string convert(char*);

int find(const std::string&,char);
int find(const std::vector<char>&, char);

std::string strip(const std::string&);

bool isdigit(const std::string&);

bool file_exists(const std::string &filename);
