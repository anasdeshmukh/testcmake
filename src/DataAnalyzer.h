#include <map>
#include <string>
#include <vector>
#include <fstream>

class DataAnalyzer
{
public:
    DataAnalyzer();
    ~DataAnalyzer();
    std::map<std::string, std::vector<int>> SmileyParser(std::string inputString);
    std::vector<std::string> WordStatistics(std::string inputString);

private:
    std::vector<std::string> UpdateMostUsedWords(const std::map<std::string, int>& wordCountMap);
    std::size_t utf8_len(const std::string& utf8_string);
    bool isBracket(char inputChar);

    void LogMostUsedWords(const std::vector<std::string>& mostUsedWords);
    void LogMostUsedWordsToTextFile(const std::vector<std::string>& mostUsedWords);
    void LogMostUsedWordsToXmlFile(const std::vector<std::string>& mostUsedWords);

    void LogSmileysAndPosition(const std::map<std::string, std::vector<int> > &smileysAndPosition);
    void LogSmileysToTextFile(const std::map<std::string, std::vector<int> > &smileysAndPosition);
    void LogSmileysToXmlFile(const std::map<std::string, std::vector<int> > &smileysAndPosition);
};

