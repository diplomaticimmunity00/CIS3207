#include "Utility.h"

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
