#pragma once
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
using namespace std;

ofstream *ofile=nullptr;

void writeFile(string path, string content)
{
        if (ofile == nullptr)
        {
                ofile = new ofstream;
                ofile->open(path, std::ios::app);
        }
        (*ofile) << content << endl;   //标题写入文件
}

string readFile(string file)
{
        ifstream infile;
        infile.open(file.data());   //将文件流对象与文件连接起来
        string content;
        while (getline(infile, content));
        infile.close();             //关闭文件输入流
        return content;
}

void string_replace(std::string &strBig, const std::string &strsrc, const std::string &strdst)
{
    std::string::size_type pos = 0;
    std::string::size_type srclen = strsrc.size();
    std::string::size_type dstlen = strdst.size();

    while( (pos=strBig.find(strsrc, pos)) != std::string::npos )
    {
        strBig.replace( pos, srclen, strdst );
        pos += dstlen;
    }
}

vector<string> split(const string &s, const string &seperator)
{
    vector<string> result;
    typedef string::size_type string_size;
    string_size i = 0;

    while(i != s.size())
    {
      //找到字符串中首个不等于分隔符的字母；
      int flag = 0;
      while(i != s.size() && flag == 0)
      {
        flag = 1;
        for(string_size x = 0; x < seperator.size(); ++x)
        if(s[i] == seperator[x])
        {
          ++i;
          flag = 0;
          break;
        }
      }

      //找到又一个分隔符，将两个分隔符之间的字符串取出；
      flag = 0;
      string_size j = i;
      while(j != s.size() && flag == 0)
      {
        for(string_size x = 0; x < seperator.size(); ++x)
        if(s[j] == seperator[x])
        {
          flag = 1;
          break;
        }
        if(flag == 0)
            ++j;
      }

      if(i != j)
      {
        result.push_back(s.substr(i, j-i));
        i = j;
      }
    }
    return result;
}
