#include <iostream>
#include <algorithm>
#include "help.h"
#include "parse.h"
#include "basic.h"
#include "assess.h"
using namespace std;

bool isFilp = false;
bool detFilp = false;

int rounds = 0;

void outputAllneeds()		//此输出函数使用$号作为分割表格的标志
{
	string content1, content2, content3, content4, content5, content6;
	int usNum = 0, eneNum = 0;
	for (int i = 0; i <= 11; i++)		//输出棋盘
	{
		for (int j = 0; j <= 4; j++)
		{
			if ((char(cMap[i][j]) >= 'A') && (char(cMap[i][j]) <= 'Z'))
                content1 += "13 ";
			else if ((char(cMap[i][j]) == 'j'))
                content1 += "10 ";
			else if ((char(cMap[i][j]) == 'k'))
                content1 += "11 ";
			else if ((char(cMap[i][j]) == 'l'))
                content1 += "12 ";
			else
				content1 += string(1, char(cMap[i][j]) - 48) + " ";
		}
		content1 += "\n";
	}
	content1 += "$";
	writeFile("特种兵的日记.txt", content1);
    for (unsigned int i = 0; i < allEnemyChess.size(); i++)		//输出概率表
	{
		float sum = (allEnemyChess[i]->sum());
        for (unsigned int j = 0; j < 12; j++)
		{
			content2 += to_string((allEnemyChess[i]->prob[j]) / sum) + " ";
		}
		content2 += "\n";
	}
	content2 += "$";
	writeFile("特种兵的日记.txt", content2);
    for (unsigned int i = 0; i < allEnemyChess.size(); i++)		//输出敌方棋子位置
	{
		content3 += to_string(allEnemyChess[i]->y) + " " + to_string(allEnemyChess[i]->x) + " ";		//此处按照先行后列进行输出
		content3 += "\n";
	}
	content3 += "$";
	writeFile("特种兵的日记.txt", content3);
	for (int i = 0; i <= 11; i++)		//输出双方总棋子数
	{
		for (int j = 0; j <= 4; j++)
		{
			if (IsMyChess(i, j))
				usNum += 1;
			if (IsEneChess(i, j))
				eneNum += 1;
		}
	}
	content4 += to_string(usNum) + " " + to_string(eneNum) + "\n";		//先输出我方总棋子数  再输出敌方总棋子数
	content4 += "$";
	writeFile("特种兵的日记.txt", content4);
	content5 += to_string(rounds) + "\n";		//输出回合数
	content5 += "$";
	writeFile("特种兵的日记.txt", content5);
	float ff1, ff2, ff3, ff4, ff5, ff6, ff7;
	tie(ff1, ff2, ff3, ff4, ff5, ff6, ff7, ignore) = assess::valueEstimation(cMap);
	content6 += to_string(ff1) + " " + to_string(ff2) + " " + to_string(ff3) + " " + to_string(ff4) + " " + to_string(ff5) + " " + to_string(ff6) + " " + to_string(ff7) + "\n";		//输出局面评估函数值
	content6 += "@";		//回合结束使用@进行分割
	writeFile("特种兵的日记.txt", content6);
}

void FreshMap(string cInMessage, string cOutMessage = "")
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
            if (enemyChess::junqiEne == nullptr && cInMessage[10] >= 'A' && cInMessage[10] <= 'L') //对方司令战死后显示军旗位置
			{
                enemyChess* c = ecOp::findChess(x1, y1);
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
        if (enemyChess::junqiEne == nullptr && cInMessage[8] == ' ' && cInMessage[9] >= 'A' && cInMessage[9] <= 'L') //对方司令战死后显示军旗位置
		{
            //由于复盘情况下，根据单回合战报无法确认司令是被我方主动吃掉的还是敌方自杀的，因此这里不确定司令
            /*enemyChess* c = ecOp::findChess(x2, y2);
            c->determine(siling);*/
			int junqiY = cInMessage[9] - 'A';
			int junqiX = cInMessage[10] - '0';
			cMap[junqiY][junqiX] = 'L';
            enemyChess* c = ecOp::findChess(junqiX, junqiY);
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

string fileOne(string pos, unsigned int begin = 0)
{
	if (pos[begin] < 'G')
	{
		int p = 'F' - pos[begin];
		pos[begin] = 'G' + p;
	}
	else
	{
		int p = 'L' - pos[begin];
		pos[begin] = 'A' + p;
	}
    if(pos[begin+1] < '2')
    {
        int p = '2' - pos[begin+1];
        pos[begin+1] = '2' + p;
    }
    else
    {
        int p = pos[begin+1] - '2';
        pos[begin+1] = '2' - p;
    }
	return pos;
}

string filp(string pos, bool isDouble = true)
{
	if (!isDouble && pos == "00")
		return pos;
	else
	{
		if (!isDouble)
			return fileOne(pos);
		else
		{
			pos = fileOne(pos, 0);
			return fileOne(pos, 2);
		}
	}
}

void move(int FirstMove, int isFirst, string i)
{
	string pos = getParentheses(i, 1);
	string result = getParentheses(i, 2);
	string junqiPos = getParentheses(i, 3);

	if (isFilp)
	{
		pos = filp(pos);
        junqiPos = filp(junqiPos, false);
	}

	if (isFirst == FirstMove) //为我方碰子结果
	{
		if (!detFilp)
		{
			detFilp = true;
			if (pos[0] < 'G') //我方在上，需要翻转
			{
				isFilp = true;
				pos = filp(pos);
                junqiPos = filp(junqiPos, false);
			}
		}
		cout << "BESTMOVE " + pos << endl;
		cout << "RESULT " + result + " " + junqiPos << endl;
		FreshMap("RESULT " + result + " " + junqiPos, "BESTMOVE " + pos);
		rounds++;
		outputAllneeds();
	}
	else //对方行动
	{
		if (!detFilp)
		{
			detFilp = true;
			if (pos[0] > 'F') //敌方在下，需要翻转
			{
				isFilp = true;
				pos = filp(pos);
                junqiPos = filp(junqiPos, false);
			}
		}
		cout << "GO " + pos + " " + result + " 00" << endl;
		FreshMap("GO " + pos + " " + result + " 00"); //因为棋谱中对方行动对应的是己方军旗位置，所以直接00
		rounds++;
		outputAllneeds();
	}
}

int main()
{
	string path;
	cin >> path;
	//询问要将先手还是后手标为我方
	int isFirst;
	cin >> isFirst;
	//如果将先手标为我方，后手即为敌方的暗棋
	string content = readFile(path);
	vector<string> allLine = split(content, "\n");
	for (string i : allLine)
	{
		if (i.substr(0, 3) == ";AA") //先手布局
		{
			if (isFirst == 1)
			{
				string array = getParentheses(i);
				InitMap("ARRAY " + array);
				init();
			}
		}
		if (i.substr(0, 3) == ";AB") //后手布局
		{
			if (isFirst == 0)
			{
				string array = getParentheses(i);
                reverse(array.begin(),array.end());
				InitMap("ARRAY " + array);
				init();
			}
		}
		if (i.substr(0, 3) == ";MA") //先手行棋
		{
			move(1, isFirst, i);
		}
		if (i.substr(0, 3) == ";MB") //后手行棋
		{
			move(0, isFirst, i);
		}
		if (i.substr(0, 3) == ";EA") //结果
		{
			if (isFirst == 1)
			{
				string result = getParentheses(i, 1);
				writeFile("特种兵的日记.txt", result);
			}
			else
			{
				string result = getParentheses(i, 2);
				writeFile("特种兵的日记.txt", result);
			}
		}
	}
    cout<<"[finish]"<<endl;
}
