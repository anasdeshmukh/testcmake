
#include "DataAnalyzer.h"
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        cout << "usage: <testcmake_run> <option (1 or 2)>  <\"text block to be analyzed\">" << endl;
        cout << "<1 : Smiley Statistics.>" << endl;
        cout << "<2 : Most used words.>" << endl;
    }
    else
    {
        int choice = (int) atoi(argv[1]);
        DataAnalyzer obj;
        switch (choice)
        {
        case 1:
            obj.SmileyParser(argv[2]);
            break;
        case 2:
            obj.WordStatistics(argv[2]);
            break;
        default:
            cout << "usage: <testcmake_run> <option (1 or 2)>  <\"text block to be analyzed\">" << endl;
            break;
        }
    }
    return 0;
}
