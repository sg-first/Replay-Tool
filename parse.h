#pragma once
#include <string>
using namespace std;

string getParentheses(string &s, int num=1)
{
    int nowNum=0;
    string result;
    bool recording=false;
    for(char c : s)
    {
        if(recording)
        {
            result+=string(1,c);
        }
        if(c=='[')
        {
            nowNum++;
            if(nowNum==num)
                recording=true;
        }
        if(c==']')
        {
            if(nowNum==num)
                break;
        }
    }
    return result;
}
