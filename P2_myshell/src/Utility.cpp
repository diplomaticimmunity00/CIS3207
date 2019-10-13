#include "Utility.h"
#include <unistd.h>

#include "Common.h"

#include <fstream>

std::string file_to_string(const std::string &filename) {
	std::ifstream f(filename);
	std::string line;
	std::string fileContents;

	if(!f.is_open()) {
		return "Error opening file";
	}

	while(std::getline(f,line)) {
		fileContents += line + "\n";
	}
	return fileContents;

}

std::vector<std::string> sub_vec(const std::vector<std::string> &v, int start, int end) {
	std::vector<std::string> subv;
	for(int i=start;i<end;i++) {
		subv.push_back(v.at(i));
	}
	return subv;
}

std::vector<std::string> split(const std::string& s,char token) {
	std::string word = "";
	std::vector<std::string> v;
	for(int i=0;i<s.size();i++) {
		if(s[i] == token) {
			v.push_back(word);
			word = "";
		} else {
			word += s[i];
		}
	}
	v.push_back(word);
	return v;
}

std::string join(const std::vector<std::string> &v, char token) {
	std::string word = "";
	for(int i=0;i<v.size();i++) {
		word += v.at(i) + token;
	}

	return word.substr(0,word.size()-1);
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

int find(const std::vector<std::string> &args,const std::string &token) {
	for(int i=0;i<args.size();i++) {
		if(args.at(i) == token) {
			return i;
		}
	}
	return -1;
}

int find(const std::string &s, const std::string &token) {
	int index;
	for(int i=0;i<s.size();i++) {
		if(s.substr(i,token.size()) == token) {
			return i;
		}
	}
	return -1;
}

int rfind(const std::vector<std::string> &args,const std::string &token) {
	for(int i=args.size()-1;i>=0;i--) {
		if(args.at(i) == token) {
			return i;
		}
	}
	return -1;
}


bool file_exists(const std::string &filename) {
	std::ifstream file(filename);
	return (file.is_open())? 1:0;
}

void populate(char* argv[],const std::vector<std::string> &v) {
	int i=0;
	for(i;i<v.size();i++) {
		argv[i+1] = v.at(i).c_str();
	}
	argv[i+1] = 0;
}

bool verify(const std::string &filename) {
	std::string contents = file_to_string(filename);
	return find(contents,"STOP") != -1;
}

bool restore_stdout() {
	int test_stdout = dup(1);
    if(stdout_fd != test_stdout) {
       	dup2(stdout_fd,1);
    }
    close(test_stdout);
}

std::string strip(const std::string &s) {
	std::string newString = "";
	for(int i=0;i<s.size();i++) {
		if(s[i] != ' ') {
			newString += s[i];
		}
	}
	return newString;
}
