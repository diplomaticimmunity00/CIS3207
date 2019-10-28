#pragma once

#include "Common.h"
#include <iostream>
#include <fstream>
#include <string>

void init_dictionary();

template <class T>
void print(T t) {
	std::cout << t << std::endl;
}

std::string convert(char*);

int find(const std::string&,char);

std::string strip(const std::string&);
