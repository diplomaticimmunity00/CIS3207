#include "Utility.h"

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
