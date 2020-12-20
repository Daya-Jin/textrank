// 分词，仅留下需要的词

#include <iostream>
#include <cstdlib>
#include "src/text_utils.h"
#include "Jieba.hpp"

using namespace std;

const char *const DICT_PATH = "../lib/cppjieba/dict/jieba.dict.utf8";
const char *const HMM_PATH = "../lib/cppjieba/dict/hmm_model.utf8";
const char *const USER_DICT_PATH = "../lib/cppjieba/dict/user.dict.utf8";
const char *const IDF_PATH = "../lib/cppjieba/dict/idf.utf8";
const char *const STOP_WORD_PATH = "../lib/cppjieba/dict/stop_words.utf8";

int main() {
    string input_file = "../data/news.u8";
    int text_field = 1;
    string out_file = "../data/news.seg";

    cppjieba::Jieba jieba(DICT_PATH,
                          HMM_PATH,
                          USER_DICT_PATH,
                          IDF_PATH,
                          STOP_WORD_PATH);

    ifstream fin(input_file.c_str());
    if (!fin.is_open()) {
        cout << "Fail to open file: " << input_file << endl;
        return -1;
    }
    ofstream fout(out_file.c_str());
    if (!fout.is_open()) {
        cout << "Fail to open file: " << out_file << endl;
        return -1;
    }

    string line;
    vector<pair<string, string> > res;
    vector<string> fields;
    const size_t char_len = 3; // utf-8 char
    const size_t least_word_len = char_len * 2; // two chars
    while (getline(fin, line)) {
        TextUtils::Split(line, "\t", fields);

        const string &text = fields[text_field];    // 正文

        res.clear();
        jieba.Tag(text, res);

        fout << line << '\t';    // 原文写回
        bool start = true;
        for (size_t i = 0; i < res.size(); ++i) {
            const string &word = res[i].first;    // word
            const string &pos = res[i].second;    // 词性
            if (!TextUtils::IsAscii(word) && word.size() < least_word_len)
                continue;
            if (TextUtils::IsAlphaNum(word) && word.size() < 3)
                continue;
            if (pos[0] == 'n' ||       //名词
                pos == "t" ||      //时间词
                pos == "s" ||      //处所词
                pos == "f" ||      //方位词
                pos == "v" ||      //动词
                pos == "a" ||      //形容词
                pos == "vn" ||     //名动词
                pos == "an" ||     //名形词
                pos == "eng" ||    //英文词
                pos == "x")        //未登录词
            {
                if (start) {
                    fout << res[i].first;
                    start = false;
                } else
                    fout << ' ' << res[i].first;
            }
        }
        fout << endl;
    }
    fin.close();
    fout.close();
    return 0;
}
