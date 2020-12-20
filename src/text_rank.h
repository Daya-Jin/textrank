#ifndef TEXT_RANK_H_
#define TEXT_RANK_H_

#include <string>
#include <vector>
#include <map>
#include <set>

struct value_great {
    template<typename T>
    bool operator()(const std::pair<T, double> &x, const std::pair<T, double> &y) const {
        return x.second > y.second;
    }
};

class TextRank {
public:
    TextRank() {}

    TextRank(int window_length, int max_iter_num, double d, double least_delta) :
            m_window_length(window_length),
            m_max_iter_num(max_iter_num),
            m_d(d),
            m_least_delta(least_delta) {}

    ~TextRank() {}

    // @brief 抽取tf最高的词
    void ExtractHighTfWords(const std::vector<std::string> &token_vec,
                            std::vector<std::pair<std::string, double> > &keywords,
                            size_t topN);

    // @brief 抽取关键词
    void
    ExtractKeyword(const std::vector<std::string> &token_vec,
                   std::vector<std::pair<std::string, double> > &keywords,
                   size_t topN = 10);

private:
    size_t m_window_length;
    size_t m_max_iter_num;    // 最大迭代次数
    double m_d;   // 论文中的d
    double m_least_delta;

    std::vector<std::string> m_words;
    std::map<std::string, size_t> m_word2idx;

    // edge weight
    // {(word1_idx,word2_idx):weight,
    //  ...,
    //  (wordi_idx,wordj_idx):weight}
    std::map<std::pair<size_t, size_t>, double> m_edge2weight;

    // 每个词的权重和
    // {word1_idx:val1,
    //  ...,
    //  word2_idx:val2}
    std::map<size_t, double> m_sum_weight;                     // out edges weight sum of one node

    /**
     * @brief 建立词之间的关系
     * @param token_vec
     * @param word_adjtab 词邻接表
     * @return
     */
    int BuildWordRelation(const std::vector<std::string> &token_vec,
                          std::map<size_t, std::set<size_t> > &word_adjtab);

    /**
     * @brief 计算单词得分
     * @param [in] word_adjtab
     * @param [out] idx2score
     */
    void CalcWordScore(const std::map<size_t, std::set<size_t> > &word_adjtab,
                       std::map<size_t, double> &idx2score);

    /**
     * @brief 更新边的权重
     * @param i
     * @param j
     */
    void UpdateWeightMap(size_t i, size_t j);

    /**
     * @brief 获取边的权重
     * @param i word_idx
     * @param j word_idx
     * @return 边(i, j)的权值
     */
    double GetWeight(size_t i, size_t j) const;

    // @brief clear for extracting from new doc
    void Clear();
};

#endif
