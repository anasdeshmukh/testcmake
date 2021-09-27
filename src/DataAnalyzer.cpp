#include "DataAnalyzer.h"
#include <codecvt>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string.hpp>

//comented out : linking issues
//#include <boost/locale.hpp>

using namespace std;
namespace pt = boost::property_tree;

DataAnalyzer::DataAnalyzer()
{
}

DataAnalyzer::~DataAnalyzer() = default;


std::size_t DataAnalyzer::utf8_len(const std::string &utf8_string)
{
    return std::wstring_convert< std::codecvt_utf8<char32_t>, char32_t >{}.from_bytes(utf8_string).size();
}

std::map<string, std::vector<int> > DataAnalyzer::SmileyParser(string inputString)
{
    std::map<std::string, std::vector<int>> smileysAndPosition;
    char dash = '-';
    vector<size_t> positions;
    int len = utf8_len(inputString);
    size_t pos = inputString.find(":", 0);
    while (pos != string::npos)
    {
        size_t nextPos = pos + 1;
        if((int)nextPos < len)
        {
            string smiley;
            if(((int)(nextPos + 1) != len)
                    and (inputString.at(nextPos) == dash)
                    and (isBracket(inputString.at(nextPos + 1))))
            {
                smiley += inputString.at(pos);
                smiley += inputString.at(nextPos);
                smiley += inputString.at(nextPos + 1);
                nextPos += 1;
            }
            else if(isBracket(inputString.at(nextPos)))
            {
                smiley += inputString.at(pos);
                smiley += inputString.at(nextPos);
            }
            if(!smiley.empty())
            {
                smileysAndPosition[smiley].push_back(pos);
            }
        }
        pos = inputString.find(":", nextPos);
    }

    LogSmileysAndPosition(smileysAndPosition);
    return smileysAndPosition;
}

//Note : '{' '}' not mentioned in requirment, uncoment below code if need to consider those brackets.
bool DataAnalyzer::isBracket(char inputChar)
{
    bool retValue = false;
    if('(' == inputChar || '[' == inputChar
            || ')' == inputChar || ']' == inputChar
            /*|| '}' == inputChar || '{' == inputChar*/)
    {
        retValue = true;
    }
    return retValue;
}

std::vector<string> DataAnalyzer::WordStatistics(string inputString)
{
    std::map<std::string, int> wordCountMap;
    regex customRegX(R"([A-Za-z]+(?:['_-][^\W_]+)*)");
    for(sregex_iterator i = sregex_iterator(inputString.begin(), inputString.end(), customRegX);
        i != sregex_iterator();
        ++i)
    {
        smatch word = *i;
        string keyString = word.str();
//        std::string keyString = boost::locale::to_lower(word.str()); // UTF8 support
        wordCountMap[boost::algorithm::to_lower_copy(keyString)] += 1;
    }

    return UpdateMostUsedWords(wordCountMap);
}

std::vector<std::string> DataAnalyzer::UpdateMostUsedWords(const std::map<std::string, int>& wordCountMap)
{
    int MAX_COUNT = 10, vecSize = 0;
    vector<pair<string, int> > wordCountVec;
    vector<string> mostUsedWord;

    for (auto& it : wordCountMap)
    {
        wordCountVec.push_back(it);
    }

    // Sort vector
    sort(wordCountVec.begin(), wordCountVec.end(),
         [](pair<string, int>& a, pair<string, int>& b) { return a.second > b.second; });

    // save the sorted value into mostUsedWord
    vecSize = wordCountVec.size();
    if(vecSize > MAX_COUNT)
    {
        vecSize = MAX_COUNT;
    }
    for (int i=0; i < vecSize; i++)
    {
        mostUsedWord.push_back(wordCountVec[i].first);
    }

    LogMostUsedWords(mostUsedWord);
    return mostUsedWord;
}

void DataAnalyzer::LogSmileysAndPosition(const std::map<std::string, std::vector<int>>& smileysAndPosition)
{
    LogSmileysToTextFile(smileysAndPosition);
    LogSmileysToXmlFile(smileysAndPosition);
}

void DataAnalyzer::LogSmileysToXmlFile(const std::map<std::string, std::vector<int> >& smileysAndPosition)
{
    string filename = "../../LogFiles/Log.xml";
    string smiley, postions;
    pt::ptree tree;

    if (!std::filesystem::remove(filename))
    {
        std::cout << "file " << filename << " not found.\n";
    }

    for(auto it = smileysAndPosition.cbegin(); it != smileysAndPosition.cend(); ++it)
    {
        smiley = (*it).first;
        if(!(*it).second.empty())
        {
            tree.put("main.smiley", smiley);

            for (int x : (*it).second)
            {
                postions = std::to_string(x);
                tree.add("main.smiley.positions", postions);
            }
        }
    }
    pt::write_xml(filename, tree);
}

void DataAnalyzer::LogSmileysToTextFile(const std::map<std::string, std::vector<int> >& smileysAndPosition)
{
    string filename = "../../LogFiles/Log.txt";
    string logString;
    fstream file;
    if (!std::filesystem::remove(filename))
    {
        std::cout << "file " << filename << " not found.\n";
    }
    file.open(filename, std::fstream::in | std::fstream::out | std::fstream::app);
    if (!file )
    {
        file.open(filename,  fstream::in | fstream::out | fstream::trunc);
    }

    for (auto it = smileysAndPosition.cbegin(); it != smileysAndPosition.cend(); ++it)
    {
        logString = "smiley = " + (*it).first + " at positions : ";
        cout << "smiley = " << (*it).first << " at positions : ";
        if((*it).second.empty())
        {
            cout << "\n";
        }
        else
        {
            for(int x : (*it).second)
            {
                logString += "\t" + std::to_string(x);
                cout << x << " ";
            }
            cout << "\n";
            logString += "\n";
            file << logString;
        }
    }
    file.close();
}


void DataAnalyzer::LogMostUsedWords(const std::vector<std::string> &mostUsedWords)
{
    LogMostUsedWordsToTextFile(mostUsedWords);
    LogMostUsedWordsToXmlFile(mostUsedWords);
}


void DataAnalyzer::LogMostUsedWordsToXmlFile(const std::vector<std::string> &mostUsedWords)
{
    string filename = "../../LogFiles/Log.xml";
    string smiley, postions;
    pt::ptree tree;
    if (!std::filesystem::remove(filename))
    {
        std::cout << "file " << filename << " not found.\n";
    }

    for (string x : mostUsedWords)
    {
        tree.add("words.word", x);
    }
    pt::write_xml(filename, tree);
}


void DataAnalyzer::LogMostUsedWordsToTextFile(const std::vector<std::string> &mostUsedWords)
{
    string filename = "../../LogFiles/Log.txt";
    string logString;
    fstream file;
    if (!filesystem::remove(filename))
    {
        cout << "file " << filename << " not found.\n";
    }
    file.open(filename, std::fstream::in | std::fstream::out | std::fstream::app);
    if (!file )
    {
        file.open(filename,  fstream::in | fstream::out | fstream::trunc);
    }

    cout << "Most used words are : " << endl;
    file << "Most used words are : " << endl;
    for (string x : mostUsedWords)
    {
        logString = x;
        cout << x << endl;
        logString += "\n";
        file << logString;
    }
    file.close();
}
