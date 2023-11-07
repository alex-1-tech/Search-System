#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <algorithm>
#include <execution>
#include <math.h>

using namespace std;

namespace input
{
    string ReadLine()
    {
        string s;
        getline(cin, s);
        return s;
    }
    int ReadLineWithNumber()
    {
        int result;
        cin >> result;
        ReadLine();
        return result;
    }
}
vector<string> SplitIntoWords(const string &text)
{
    vector<string> words;
    string word;
    for (const char c : text)
    {
        if (c == ' ')
        {
            words.push_back(word);
            word = "";
        }
        else
            word += c;
    }
    words.push_back(word);
    return words;
}

class SearchServer
{
private:
    struct Document
    {
        int document_id;
        double relevance;
    };
    struct Query
    {
        vector<string> plus_words;
        vector<string> minus_words;
    };
    map<string, map<int, double>> word_to_document_freqs_;
    set<string> stop_words_;
    int document_count_ = 0;
    const int MAX_RESULT_DOCUMENT_COUNT = 5;

    Query ParseQuery(const string &query) const
    {
        Query query_words;
        for (auto word : SplitIntoWordsNoStop(query))
        {
            if (word[0] == '-')
                query_words.minus_words.push_back(word.substr(1));
            else
                query_words.plus_words.push_back(word);
        }
        return query_words;
    }

    vector<Document> FindAllDocuments(const string &query) const
    {
        map<int, double> document_to_relevance;

        const Query query_words = ParseQuery(query);
        for (const string &word : query_words.plus_words)
        {
            if (word_to_document_freqs_.count(word) == 0)
                continue;
            for (const auto [document_id, team_freq] : word_to_document_freqs_.at(word))
            {
                document_to_relevance[document_id] += ComputeTfIdfWord(word, team_freq);
            }
        }

        for (const string &word : query_words.minus_words)
        {
            if (word_to_document_freqs_.count(word) == 0)
                continue;
            for (const auto [document_id, team_freq] : word_to_document_freqs_.at(word))
            {
                document_to_relevance[document_id] = 0;
            }
        }

        vector<Document> found_documents;
        for (auto [document_id, relevance] : document_to_relevance)
        {
            if (relevance != 0)
                found_documents.push_back({document_id, relevance});
        }
        return found_documents;
    }
    double ComputeTfIdfWord(const string& word, double team_freq) const
    {
        return log(document_count_ / (double)word_to_document_freqs_.at(word).size()) * team_freq;
    }
    vector<string> SplitIntoWordsNoStop(const string &text) const
    {
        vector<string> words;
        for (const string &word : SplitIntoWords(text))
        {
            if (stop_words_.count(word) == 0)
                words.push_back(word);
        }
        return words;
    }

public:
    void SetStopWords(const string &text)
    {
        for (const string &word : SplitIntoWords(text))
            stop_words_.insert(word);
    }
    void AddDocument(int document_id, const string &document)
    {
        document_count_ += 1;
        vector<string> words = SplitIntoWordsNoStop(document);
        for (const string &word : words)
        {
            word_to_document_freqs_[word][document_id] += (1.0 / (double)words.size());
        }
    }
    
    vector<Document> FindTopDocuments(const string &query) const
    {
        auto found_documents = FindAllDocuments(query);

        sort(execution::par, found_documents.begin(), found_documents.end(),
             [](Document lhs, Document rhs)
             {
                 return lhs.relevance > rhs.relevance;
             });
        if (found_documents.size() > MAX_RESULT_DOCUMENT_COUNT)
        {
            found_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
        return found_documents;
    }
};
SearchServer CreateSearchServer()
{
    SearchServer search_server;
    const string stop_words_joined = input::ReadLine();

    search_server.SetStopWords(stop_words_joined);
    const int document_count = input::ReadLineWithNumber();
    for (int document_id = 0; document_id < document_count; document_id++)
    {
        search_server.AddDocument(document_id, input::ReadLine());
    }
    return search_server;
}

int main(int argc, char *argv[])
{
    const SearchServer search_server = CreateSearchServer();

    const string query = input::ReadLine();
    for (auto [document_id, relevance] : search_server.FindTopDocuments(query))
    {
        cout << "{ document_id = "s << document_id << ", relevance = "s << relevance << " }"s << endl;
    }
}