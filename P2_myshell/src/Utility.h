#include <iostream>
#include <string>
#include <vector>

//print
template <class T>
void print(T info) {
	std::cout << info << std::endl;
}

//get vector elements within range 1 starting at index 2
std::vector<std::string> sub_vec(const std::vector<std::string>&,int,int);

//split string into vector by token
std::vector<std::string> split(const std::string&,char);

//conjoin vector elements into string separated by char
std::string join(const std::vector<std::string>&,char);
