#include "gtest/gtest.h"
#include "DataAnalyzer.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string.hpp>
#include <fstream>

namespace pt = boost::property_tree;

void GetVectorOfKeysAndValues(std::vector<std::string>& vecOfKeys,
                     std::vector<std::vector<int>>& values,
                     const std::map<std::string, std::vector<int>>& map)
{
    vecOfKeys.clear();
    values.clear();

    for(auto it = map.begin(); it != map.end(); ++it)
    {
      vecOfKeys.push_back(it->first);
      values.push_back(it->second);
    }
}

TEST(DataAnalyzertest, When_SmileyParserIsCalledWithEmptyString_Then_ItShouldReturnEmptyMapOfSmiley)
{
    DataAnalyzer dataAnalyzerObj;
    std::vector<std::string> keys;

    std::map<std::string, std::vector<int>> smileysAndPosition = dataAnalyzerObj.SmileyParser("");
    EXPECT_EQ(smileysAndPosition.size(), 0);
}

TEST(DataAnalyzertest, When_SmileyParserIsCalledWithDifferentTextBlock_Then_ItShouldReturnCorrectMapOfSmiley)
{
    DataAnalyzer dataAnalyzerObj;
    std::vector<std::string> keys;
    std::vector<std::vector<int>> values;
    std::vector<int> vecOfPositions;

    //Test string 1 :
    std::map<std::string, std::vector<int>> smileysAndPosition = dataAnalyzerObj.SmileyParser(":):):)");
    EXPECT_EQ(smileysAndPosition.size(), 1);

    GetVectorOfKeysAndValues(keys, values, smileysAndPosition);
    EXPECT_EQ(keys.size(), 1);
    EXPECT_EQ(keys.at(0), ":)");
    EXPECT_EQ(values.size(), 1);

    vecOfPositions = values.at(0);
    EXPECT_EQ(vecOfPositions.size(), 3);
    EXPECT_EQ(vecOfPositions.at(0), 0);
    EXPECT_EQ(vecOfPositions.at(1), 2);
    EXPECT_EQ(vecOfPositions.at(2), 4);

    //Test string 2 :
    smileysAndPosition = dataAnalyzerObj.SmileyParser("1 2332:) 23:):?::[-some randomText^#1:[0]:]:::-0:::");
    EXPECT_EQ(smileysAndPosition.size(), 3);

    GetVectorOfKeysAndValues(keys, values, smileysAndPosition);
    EXPECT_EQ(keys.size(), 3);
    EXPECT_EQ(keys.at(0), ":)");
    EXPECT_EQ(keys.at(1), ":[");
    EXPECT_EQ(keys.at(2), ":]");

    EXPECT_EQ(values.size(), 3);

    vecOfPositions = values.at(0);
    EXPECT_EQ(vecOfPositions.size(), 2);
    EXPECT_EQ(vecOfPositions.at(0), 6);
    EXPECT_EQ(vecOfPositions.at(1), 11);

    vecOfPositions = values.at(1);
    EXPECT_EQ(vecOfPositions.size(), 2);
    EXPECT_EQ(vecOfPositions.at(0), 16);
    EXPECT_EQ(vecOfPositions.at(1), 37);

    vecOfPositions = values.at(2);
    EXPECT_EQ(vecOfPositions.size(), 1);
    EXPECT_EQ(vecOfPositions.at(0), 41);

    //Test string 3 (UTF8 string)
    smileysAndPosition = dataAnalyzerObj.SmileyParser("\x3a\x2d\x28\x20\x73\x6f\x6d\x65\x20\x72\x61\x6e\x64\x6f\x6d\x20\x74\x65\x73\x74\x20\x69\x6e\x55\x54\x46\x3a\x5d\x20\x69\x74\x4d\x75\x53\x74\x20\x57\x30\x72\x6b\x20\x3a\x29\x3a\x29");
    EXPECT_EQ(smileysAndPosition.size(), 3);

    GetVectorOfKeysAndValues(keys, values, smileysAndPosition);
    EXPECT_EQ(keys.size(), 3);
    EXPECT_EQ(keys.at(0), ":)");
    EXPECT_EQ(keys.at(1), ":-(");
    EXPECT_EQ(keys.at(2), ":]");

    EXPECT_EQ(values.size(), 3);

    vecOfPositions = values.at(0);
    EXPECT_EQ(vecOfPositions.size(), 2);
    EXPECT_EQ(vecOfPositions.at(0), 41);
    EXPECT_EQ(vecOfPositions.at(1), 43);

    vecOfPositions = values.at(1);
    EXPECT_EQ(vecOfPositions.size(), 1);
    EXPECT_EQ(vecOfPositions.at(0), 0);

    vecOfPositions = values.at(2);
    EXPECT_EQ(vecOfPositions.size(), 1);
    EXPECT_EQ(vecOfPositions.at(0), 26);
}

TEST(DataAnalyzertest, When_WordStatisticsIsCalledWithEmptyString_Then_ItShouldReturnEmptyVector)
{
    DataAnalyzer dataAnalyzerObj;
    std::vector<std::string> mostUsedWords = dataAnalyzerObj.WordStatistics("");
    EXPECT_EQ(mostUsedWords.size(), 0);
}

TEST(DataAnalyzertest, When_WordStatisticsIsCalledWithDifferentTextBlock_Then_ItShouldReturnCorrectVector)
{
    DataAnalyzer dataAnalyzerObj;
    std::vector<std::string> mostUsedWords = dataAnalyzerObj.WordStatistics("code random text code, % 4 testing TEXT code block 4 ");
    EXPECT_EQ(mostUsedWords.size(), 5);
    EXPECT_EQ(mostUsedWords.at(0), "code");
    EXPECT_EQ(mostUsedWords.at(1), "text");
    EXPECT_EQ(mostUsedWords.at(2), "block");
    EXPECT_EQ(mostUsedWords.at(3), "random");
    EXPECT_EQ(mostUsedWords.at(4), "testing");

    mostUsedWords = dataAnalyzerObj.WordStatistics("12#$% 54^7 8() :) :)        :");
    EXPECT_EQ(mostUsedWords.size(), 0);

    mostUsedWords = dataAnalyzerObj.WordStatistics("\x42\x4d\x57\x20\x42\x4d\x57\x20\x42\x4d\x57");
    EXPECT_EQ(mostUsedWords.size(), 1);
    EXPECT_EQ(mostUsedWords.at(0), "bmw");
}

TEST(DataAnalyzertest, When_WordStatisticsIsCalledWithTextBlock_Then_VeriyXmlLogFiles)
{
    DataAnalyzer dataAnalyzerObj;
    std::vector<std::string> expectedVector;
    std::vector<std::string> mostUsedWords = dataAnalyzerObj.WordStatistics("code random text code, % 4 testing TEXT code block 4 ");
    EXPECT_EQ(mostUsedWords.size(), 5);
    EXPECT_EQ(mostUsedWords.at(0), "code");
    EXPECT_EQ(mostUsedWords.at(1), "text");
    EXPECT_EQ(mostUsedWords.at(2), "block");
    EXPECT_EQ(mostUsedWords.at(3), "random");
    EXPECT_EQ(mostUsedWords.at(4), "testing");

    boost::property_tree::ptree pt;
    read_xml("../../LogFiles/Log.xml", pt);

    for(boost::property_tree::ptree::value_type &v : pt.get_child("words"))
    {
        expectedVector.push_back(v.second.data());
    }

    EXPECT_EQ(expectedVector.at(0), "code");
    EXPECT_EQ(expectedVector.at(1), "text");
    EXPECT_EQ(expectedVector.at(2), "block");
    EXPECT_EQ(expectedVector.at(3), "random");
    EXPECT_EQ(expectedVector.at(4), "testing");
}


TEST(DataAnalyzertest, When_WordStatisticsIsCalledWithTextBlock_Then_VeriyTextLogFiles)
{
    DataAnalyzer dataAnalyzerObj;
    int indexZero = 0;
    std::vector<std::string> expectedVector;
    std::vector<std::string> mostUsedWords = dataAnalyzerObj.WordStatistics("BMW, bmw is automobile company. company ComPanY**company$%#3");
    EXPECT_EQ(mostUsedWords.size(), 4);
    EXPECT_EQ(mostUsedWords.at(0), "company");
    EXPECT_EQ(mostUsedWords.at(1), "bmw");
    EXPECT_EQ(mostUsedWords.at(2), "automobile");
    EXPECT_EQ(mostUsedWords.at(3), "is");

    boost::property_tree::ptree pt;
    read_xml("../../LogFiles/Log.xml", pt);
    std::string word;
    std::ifstream MyReadFile("../../LogFiles/Log.txt");
    while (getline (MyReadFile, word))
    {
        if(0 == indexZero)
        {
            indexZero++;
        }
        else
        {
            expectedVector.push_back(word);
        }
    }

    EXPECT_EQ(expectedVector.size(), 4);
    EXPECT_EQ(expectedVector.at(0), "company");
    EXPECT_EQ(expectedVector.at(1), "bmw");
    EXPECT_EQ(expectedVector.at(2), "automobile");
    EXPECT_EQ(expectedVector.at(3), "is");
}

