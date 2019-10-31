#pragma once

#include "Common.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

void init_dictionary(const std::string&);

template <class T>
void print(T t) {
	std::cout << t << std::endl;
}

std::string convert(char*);

int find(const std::string&,char);
int find(const std::vector<char>&, char);


std::string strip(const std::string&);

bool isdigit(const std::string&);

bool file_exists(const std::string &filename);
