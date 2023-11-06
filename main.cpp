#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

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
vector<string> SplitIntoWordsNoStop(
    const string &text,
    const set<string> &stop_word)
{
    vector<string> words;
    for (const string &word : SplitIntoWords(text))
    {
        if (stop_word.count(word) == 0)
            words.push_back(word);
    }
    return words;
}
set<string> ParseStopWords(const string &text)
{
    set<string> stop_words;
    for (const string &word : SplitIntoWords(text))
        stop_words.insert(word);
    return stop_words;
}
void AddDocument(map<string, set<int>> &word_to_document,
                 const set<string> &stop_words,
                 int document_id,
                 const string &document)
{
    for (const string &word : SplitIntoWordsNoStop(document, stop_words))
    {
        word_to_document[word].insert(document_id);
    }
}

vector<pair<int, int>> FindDocuments(map<string, set<int>> &word_to_documents,
                                     const set<string> &stop_words,
                                     const string &query)
{
    map<int, int> document_to_relevance;
    const vector<string> query_words = SplitIntoWordsNoStop(query, stop_words);
    for (const string &word : query_words)
    {
        if (word_to_documents.count(word) == 0)
            continue;
        for (const int document_id : word_to_documents[word])
        {
            ++document_to_relevance[document_id];
        }
    }

    vector<pair<int, int>> found_documents;
    for (auto [document_id, relevance] : document_to_relevance)
    {
        found_documents.push_back({document_id, relevance});
    }
    return found_documents;
}
int main(int argc, char *argv[])
{
    const string stop_words_joined = input::ReadLine();
    const set<string> stop_words = ParseStopWords(stop_words_joined);

    map<string, set<int>> word_to_documents;
    const int document_count = input::ReadLineWithNumber();
    for (int document_id = 0; document_id < document_count; document_id++)
    {
        AddDocument(word_to_documents, stop_words, document_id, input::ReadLine());
    }

    const string query = input::ReadLine();
    for (auto [document_id, relevance] : FindDocuments(word_to_documents, stop_words, query))
    {
        cout << "{ document_id = "s << document_id << ", relevance = "s << relevance << " }"s << endl;
    }
}