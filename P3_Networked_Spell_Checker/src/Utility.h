#pragma once

#include "Common.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>

void init_dictionary(const std::string&);

char* get_timestamp();

void clear_log();

template <class T>
void print(T t) {

	std::cout << t << std::endl;
	
	std::ofstream log(logfile,std::ios_base::app);

    if(!log.is_open()) {
        std::cerr << "ERROR writing to logfile" << std::endl;
        return;
    } else {
		char* timestamp = get_timestamp();
        log << timestamp << " : " << t << std::endl;
        log.close();
    }


}

std::string convert(char*);

int find(const std::string&,char);
int find(const std::vector<char>&, char);

std::string strip(const std::string&);

bool isdigit(const std::string&);

bool file_exists(const std::string &filename);
