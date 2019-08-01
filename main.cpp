#include <iostream>
#include "help.h"
#include "parse.h"
#include "basic.h"
using namespace std;

bool isFilp=false;
bool detFilp=false;
bool findJunqi=false;

void FreshMap(string cInMessage, string cOutMessage="")
{
    int x1, y1;				//起点
    int x2, y2;				//落点
    char result = -1;			//碰子结果
    if (cInMessage[0] == 'G')	// GO 指令（对方要走，根据系统得到的信息更新）
    {
        if (cInMessage[3] >= 'A' && cInMessage[3] <= 'L')
        {
            y1 = cInMessage[3] - 'A';
            x1 = cInMessage[4] - '0';
            y2 = cInMessage[5] - 'A';
            x2 = cInMessage[6] - '0';
            result = cInMessage[8] - '0';		//碰子结果
            if (!findJunqi && cInMessage[10] >= 'A' && cInMessage[10] <= 'L') //对方司令战死后显示军旗位置
            {
                findJunqi=true;
                enemyChess* c = ecOp::findChess(x1, y1);
                if(c!=nullptr)
                    c->determine(siling);
                int junqiY = cInMessage[10] - 'A';
                int junqiX = cInMessage[11] - '0';
                cMap[junqiY][junqiX] = 'L';
                c = ecOp::findChess(junqiX, junqiY);
                c->determine(junqi);
            }

            enemyChess* c = ecOp::findChess(x1, y1);
            switch (result)		//根据不同结果修改棋盘
            {
            case 0:			//对方棋子被己方吃掉
            {
                c->less(ecOp::codeToType(cMap[y2][x2])); //对方棋子小于己方棋子
                cMap[y1][x1] = '0'; //对方棋子消失，己方不必改变
                ecOp::adjustDepth();
                break;
            }
            case 1:			//对方吃掉己方棋子
            {
                c->more(ecOp::codeToType(cMap[y2][x2])); //对方棋子大于己方棋子
                c->setPos(x2, y2);
                cMap[y2][x2] = cMap[y1][x1]; //对方棋子移到己方位置（1是对方）
                cMap[y1][x1] = '0';
                ecOp::adjustDepth();
                break;
            }
            case 2:			//双方棋子对死
            {
                c->equ(ecOp::codeToType(cMap[y2][x2])); //对方棋子等于己方棋子
                cMap[y1][x1] = '0';
                cMap[y2][x2] = '0';
                ecOp::adjustDepth();
                break;
            }
            case 3:			//对方移动棋子
            {
                c->setPos(x2, y2);
                cMap[y2][x2] = cMap[y1][x1];
                cMap[y1][x1] = '0';
                ecOp::adjustDepth();
                break;
            }
            }
        }
    }
    if (cInMessage[0] == 'R')	// RESULT 指令
    {
        //先根据cOutMessage记下之前移动的我方棋子
        y1 = cOutMessage[9] - 'A';
        x1 = cOutMessage[10] - '0';
        y2 = cOutMessage[11] - 'A';
        x2 = cOutMessage[12] - '0';

        //然后看看这个棋子的结果
        result = cInMessage[7] - '0'; //碰子结果
        if (!findJunqi && cInMessage[8] == ' ' && cInMessage[9] >= 'A' && cInMessage[9] <= 'L') //对方司令战死后显示军旗位置
        {
            findJunqi=true;
            enemyChess* c = ecOp::findChess(x2, y2);
            if(c!=nullptr)
                c->determine(siling);
            int junqiY = cInMessage[9] - 'A';
            int junqiX = cInMessage[10] - '0';
            cMap[junqiY][junqiX] = 'L';
            c = ecOp::findChess(junqiX, junqiY);
            c->determine(junqi);
        }
        switch (result)		//根据不同结果修改棋盘
        {
        case 0:			//己方棋子被对方吃掉
        {
            enemyChess* c = ecOp::findChess(x2, y2);
            c->more(ecOp::codeToType(cMap[y1][x1])); //对方棋子大于己方棋子
            cMap[y1][x1] = '0';
            ecOp::adjustDepth();
            break;
        }
        case 1:			//己方吃掉对方棋子
        {
            enemyChess* c = ecOp::findChess(x2, y2);
            c->less(ecOp::codeToType(cMap[y1][x1])); //对方棋子小于己方棋子
            cMap[y2][x2] = cMap[y1][x1]; //2是新位置（敌方），1是老位置
            cMap[y1][x1] = '0';
            ecOp::adjustDepth();
            break;
        }
        case 2:			//双方棋子对死
        {
            enemyChess* c = ecOp::findChess(x2, y2);
            c->equ(ecOp::codeToType(cMap[y1][x1])); //对方棋子等于己方棋子
            cMap[y1][x1] = '0';
            cMap[y2][x2] = '0';
            ecOp::adjustDepth();
            break;
        }
        case 3:			//己方移动棋子
        {
            cMap[y2][x2] = cMap[y1][x1];
            cMap[y1][x1] = '0';
            break;
        }
        }
    }
}

void init()
{
    ecOp::search_depth = minDepth;
    for (int i = 0; i <= 5; i++)
    {
        for (int j = 0; j <= 4; j++)
        {
            if (cMap[i][j] == 'X')
                allEnemyChess.push_back(new enemyChess(j, i));
        }
    }
    enemyChess::junqiEne = nullptr;
}

string fileOne(string pos,unsigned int begin=0)
{
    if(pos[begin]<'G')
    {
        int p='F'-pos[begin];
        pos[begin]='G'+p;
    }
    else
    {
        int p='L'-pos[begin];
        pos[begin]='A'+p;
    }
    return pos;
}

string filp(string pos,bool isDouble=true)
{
    if(!isDouble && pos=="00")
        return pos;
    else
    {
        if(!isDouble)
            return fileOne(pos);
        else
        {
            pos=fileOne(pos,0);
            return fileOne(pos,2);
        }
    }
}

void move(int FirstMove, int isFirst,string i)
{
    string pos = getParentheses(i,1);
    string result = getParentheses(i,2);
    string junqiPos = getParentheses(i,3);

    if(isFilp)
    {
        pos=filp(pos);
        junqiPos=filp(pos,false);
    }

    if(isFirst==FirstMove) //为我方碰子结果
    {
        if(!detFilp)
        {
            detFilp=true;
            if(pos[0]<'G') //我方在上，需要翻转
            {
                isFilp=true;
                pos=filp(pos);
                junqiPos=filp(pos,false);
            }
        }
        cout<<"BESTMOVE "+pos<<endl;
        cout<<"RESULT "+result+" "+junqiPos<<endl;
        FreshMap("RESULT "+result+" "+junqiPos,"BESTMOVE "+pos);
        //fix:保存点
    }
    else //对方行动
    {
        if(!detFilp)
        {
            detFilp=true;
            if(pos[0]>'F') //敌方在下，需要翻转
            {
                isFilp=true;
                pos=filp(pos);
                junqiPos=filp(pos,false);
            }
        }
        cout<<"GO "+pos+" "+result+" 00"<<endl;
        FreshMap("GO "+pos+" "+result+" 00"); //因为棋谱中对方行动对应的是己方军旗位置，所以直接00
        //fix:保存点
    }
}

int main()
{
    string path;
    cin>>path;
    //询问要将先手还是后手标为我方
    int isFirst;
    cin>>isFirst;
    //如果将先手标为我方，后手即为敌方的暗棋
    string content = readFile(path);
    vector<string> allLine = split(content,"\n");
    for(string i : allLine)
    {
        if(i.substr(0,3)==";AA") //先手布局
        {
            if(isFirst==1)
            {
                string array = getParentheses(i);
                InitMap("ARRAY "+array);
                init();
            }
        }
        if(i.substr(0,3)==";AB") //后手布局
        {
            if(isFirst==0)
            {
                string array = getParentheses(i);
                InitMap("ARRAY "+array);
                init();
            }
        }
        if(i.substr(0,3)==";MA") //先手行棋
        {
            move(1,isFirst,i);
        }
        if(i.substr(0,3)==";MB") //后手行棋
        {
            move(0,isFirst,i);
        }
        if(i.substr(0,3)==";EA") //结果
        {
            if(isFirst==1)
            {
                string result = getParentheses(i,1);
                //fix:保存
            }
            else
            {
                string result = getParentheses(i,2);
                //fix:保存
            }
        }
    }
}
