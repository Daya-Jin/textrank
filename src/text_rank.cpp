#include <iostream>
#include <algorithm>
#include <cmath>
#include "text_rank.h"

using namespace std;


void TextRank::ExtractHighTfWords(const vector<string> &token_vec, vector<pair<string, double> > &keywords,
                                  const size_t topN) {
    keywords.clear();
    if (token_vec.empty())
        return;
    map<string, double> word_tf;
    for (size_t i = 0; i < token_vec.size(); ++i) {
        const string &word = token_vec[i];
        if (word_tf.find(word) == word_tf.end())
            word_tf[word] = 1;
        else
            word_tf[word] += 1;
    }
    vector<pair<string, double> > word_tf_vec(word_tf.begin(), word_tf.end());
    sort(word_tf_vec.begin(), word_tf_vec.end(), value_great());
    for (size_t i = 0; i < word_tf_vec.size(); ++i) {
        if (i == topN)
            break;
        keywords.push_back(word_tf_vec[i]);
    }
}

void TextRank::ExtractKeyword(const vector<string> &token_vec, vector<pair<string, double> > &keywords,
                              const size_t topN) {
    keywords.clear();
    if (token_vec.empty())
        return;

    Clear();

    map<size_t, set<size_t> > word_neighbors;
    map<size_t, double> idx2score;

    BuildWordRelation(token_vec, word_neighbors);

#ifdef _DEBUG
    cout << "token num: " << token_vec.size() << '\t' << word_neighbors.size() << endl;
#endif

    CalcWordScore(word_neighbors, idx2score);

    vector<pair<size_t, double> > score_vec(idx2score.begin(), idx2score.end());
    sort(score_vec.begin(), score_vec.end(), value_great());

    for (size_t i = 0; i < score_vec.size(); ++i) {
        if (i == topN)    // 只取topN
            break;
        const string &word = this->m_words[score_vec[i].first];
        keywords.push_back(make_pair(word, score_vec[i].second));
    }
}

void TextRank::UpdateWeightMap(size_t i, size_t j) {
    // 无向图对称，只需记录一边
    if (i > j) {
        size_t tmp = i;
        i = j;
        j = tmp;
    }

    pair<size_t, size_t> key(i, j);
    if (this->m_edge2weight.find(key) != this->m_edge2weight.end())
        this->m_edge2weight[key] += 1;
    else
        this->m_edge2weight.insert(make_pair(key, 1.0));
}

double TextRank::GetWeight(size_t i, size_t j) const {
    if (i > j) {
        size_t tmp = i;
        i = j;
        j = tmp;
    }
    pair<size_t, size_t> key(i, j);
    if (this->m_edge2weight.find(key) != this->m_edge2weight.end())
        return this->m_edge2weight.at(key);
    return 0;
}

int TextRank::BuildWordRelation(const vector<string> &token_vec, map<size_t, set<size_t> > &word_adjtab) {
    this->m_edge2weight.clear();
    word_adjtab.clear();

    const size_t n = token_vec.size();

    // word2idx
    for (size_t i = 0; i < n; ++i) {
        const string &word = token_vec[i];
        if (this->m_word2idx.find(word) == this->m_word2idx.end()) {
            this->m_words.push_back(word);
            this->m_word2idx.insert(make_pair(word, this->m_words.size() - 1));
        }
    }

    for (size_t i = 0; i < n; ++i) {
        const string &word = token_vec[i];
        size_t idx1 = this->m_word2idx.at(word);

        if (word_adjtab.find(idx1) == word_adjtab.end()) {
            word_adjtab.insert(make_pair(idx1, set<size_t>()));
        }

        for (size_t j = 1; j < this->m_window_length; ++j) {
            if (i + j >= n)    // 超出单词总数量
                break;
            size_t idx2 = this->m_word2idx.at(token_vec[i + j]);
            if (idx2 == idx1)
                continue;

            UpdateWeightMap(idx1, idx2);

            word_adjtab[idx1].insert(idx2);

            // undirected graph
            if (word_adjtab.find(idx2) == word_adjtab.end()) {
                word_adjtab.insert(make_pair(idx2, set<size_t>()));
            }
            word_adjtab[idx2].insert(idx1);
        }
    }

    // 计算每个点的权重和
    for (map<size_t, set<size_t> >::iterator witer = word_adjtab.begin(); witer != word_adjtab.end(); ++witer) {
        size_t idx1 = witer->first;
        set<size_t> &neighbors = witer->second;
        double sum = 0;

        for (set<size_t>::iterator niter = neighbors.begin(); niter != neighbors.end(); ++niter) {
            size_t idx2 = *niter;
            sum += GetWeight(idx1, idx2);
        }
        this->m_sum_weight.insert(make_pair(idx1, sum));
    }
    return 0;
}


void TextRank::CalcWordScore(const map<size_t, set<size_t> > &word_adjtab, map<size_t, double> &idx2score) {
    idx2score.clear();

    // 所有词的分数初始化为1
    for (map<size_t, set<size_t> >::const_iterator w_it = word_adjtab.begin(); w_it != word_adjtab.end(); ++w_it)
        idx2score.insert(make_pair(w_it->first, 1.0));

    // iterate
    for (size_t i = 0; i < this->m_max_iter_num; ++i) {
        double max_delta = 0;    // 最大更新差值
        map<size_t, double> new_idx2score; // current iteration score

        for (map<size_t, set<size_t> >::const_iterator w_it = word_adjtab.begin();
             w_it != word_adjtab.end(); ++w_it) {
            size_t idx1 = w_it->first;
            double new_score = 1 - m_d;    // 1-d
            double sum_weight = 0;    // 公式后面的累加项
            const set<size_t> &neighbors = w_it->second;

            for (set<size_t>::const_iterator n_it = neighbors.begin(); n_it != neighbors.end(); ++n_it) {
                size_t idx2 = *n_it;
                double weight = GetWeight(idx2, idx1);
                sum_weight += weight / this->m_sum_weight.at(idx2) * idx2score[idx2];
            }

            new_score += this->m_d * sum_weight;
            new_idx2score.insert(make_pair(idx1, new_score));

            double delta = fabs(new_score - idx2score[idx1]);
            max_delta = max(max_delta, delta);

            //cout << "iter " << i << '\t' << m_word_vec[id1] << '\t' << new_score << '\t' << delta << endl;
        }

        idx2score = new_idx2score;
        if (max_delta < this->m_least_delta) {    // 更新值小于阈值则退出
//            cout << "stop iteration when iter = " << i << '\t' << max_delta << endl;
            break;
        }

#ifdef _DEBUG
        if (i == this->m_max_iter_num - 1)
            cout << "max_delta: " << max_delta << endl;
#endif
    }
}

void TextRank::Clear() {
    this->m_words.clear();
    this->m_word2idx.clear();
    this->m_edge2weight.clear();
    this->m_sum_weight.clear();
}
