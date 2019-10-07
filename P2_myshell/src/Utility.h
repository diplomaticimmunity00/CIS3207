#pragma once

#include <iostream>
#include <string>
#include <vector>

//print
template <class T>
void print(T info) {
	std::cout << info << std::endl;
}

//file_to_string
std::string file_to_string(const std::string &filename);

//get vector elements within range 1 starting at index 2
std::vector<std::string> sub_vec(const std::vector<std::string>&,int,int);

//split string into vector by token
std::vector<std::string> split(const std::string&,char);

//conjoin vector elements into string separated by char
std::string join(const std::vector<std::string>&,char);

std::string convert(char* old);
