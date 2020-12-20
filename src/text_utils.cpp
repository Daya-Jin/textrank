#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <cctype>
#include "text_utils.h"

using namespace std;


bool TextUtils::IsAscii(const std::string &word)
{
    unsigned char mask = 0x80;
    for (size_t i = 0; i < word.size(); ++i)
    {
        if ((unsigned char)word[i] & mask)
            return false;
    }
    return true;
}

bool TextUtils::IsAlphaNum(const std::string &word)
{
    if (word.empty())
        return false;
    for (size_t i = 1; i < word.size(); ++i)
    {
        if (!isalnum(word[i]))
            return false;
    }
    return true;
}



void TextUtils::Split(const string &s, const string &delim, vector<string> &elems, size_t max_split)
{
    elems.clear();
    if (s.empty())
        return;
    string::size_type next = string::npos;
    string::size_type current = 0;
    size_t len = delim.size();
    while (true) {
        next = s.find(delim, current);
        elems.push_back(s.substr(current, next - current));
        if (next == string::npos)
            break;
        current = next+len;
        if (max_split > 0 && max_split == elems.size()) {
            elems.push_back(s.substr(current));
            break;
        }  
    }  
}
