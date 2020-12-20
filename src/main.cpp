#include<iostream>
#include<fstream>
#include<sstream>
#include<cstdlib>
#include "text_rank.h"
#include "text_utils.h"

using namespace std;

int main() {
    string input_file="../data/news.seg";
    int choose_field = 2;
    string out_file="../data/news.word";

    ifstream fin(input_file.c_str());
    if (!fin.is_open()) {
        cout << "fail to open file: " << input_file << endl;
        return -1;
    }
    ofstream fout(out_file.c_str());
    if (!fin.is_open()) {
        cout << "fail to open file: " << out_file << endl;
        return -1;
    }

    int window_length = 3;
    int max_iter_num = 100;
    double d = 0.85;
    double least_delta = 0.0001;
    size_t topN = 5;

    TextRank ranker(window_length, max_iter_num, d, least_delta);

    string line;
    vector<string> fields;
    vector<string> token_vec;    // 词列表
    vector<pair<string, double> > keywords;
    vector<pair<string, double> > keywords2;

    while (getline(fin, line)) {
        TextUtils::Split(line, "\t", fields);
        const string &token_str = fields[choose_field];
        TextUtils::Split(token_str, " ", token_vec);
        ranker.ExtractKeyword(token_vec, keywords, topN);
        ranker.ExtractHighTfWords(token_vec, keywords2, topN);

        for (size_t i = 0; i < keywords.size(); ++i) {
            if (i != 0)
                fout << ' ';
            fout << keywords[i].first << '(' << keywords[i].second << ')';
        }
        fout << '\t';
        for (size_t i = 0; i < keywords2.size(); ++i) {
            if (i != 0)
                fout << ' ';
            fout << keywords2[i].first << '(' << keywords2[i].second << ')';
        }
        fout << '\t' << line << endl;
    }

    fin.close();
    fout.close();
    return 0;
}
