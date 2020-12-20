#ifndef TEXT_UTILS_H_
#define TEXT_UTILS_H_

#include <string>
#include <map>
#include <set>
#include <vector>
#include <ctime>

class TextUtils
{
public:
    // @brief 判断是否是ASCII字符
    static bool IsAscii(const std::string &word);

    // @brief 判断是否是字母数字词
    static bool IsAlphaNum(const std::string &word);

    // @brief 分割字符串
    static void Split(const std::string &s, const std::string &delim, std::vector<std::string> &elems, size_t max_split = 0);
};

#endif
