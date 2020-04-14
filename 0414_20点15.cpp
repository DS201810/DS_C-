#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string>
#include<string.h>
#include<math.h>
#include<time.h>
#include<vector>
#include<map>
#include<algorithm>
#include<cstdio>
#include<iostream>
#include<chrono>
#include<random> 
#include<list>
using namespace std;

/**********************宏定义**************************/
#define BLACK 0
#define WHITE 1
#define EMPTY 2
#define MAX 99999999
#define MIN -99999999
const int n = 19;
const int m = 19;


/**********************************自定义数据类型*********************************/
struct Point { //点结构
	int x, y;
};
struct Step { //步结构
	Point first, second;
	int value;
};
class myBoard {
public:
	int comScore;
	int humScore;
	int x;
	int y;
};
struct Score {
	int ONE = 5; //活一 
	int BLOCKED_ONE = 1;//眠一 

	int TWO = 150;//活二 
	int MIST_TWO = 100;//朦胧二  
	int BLOCKED_TWO = 10;//眠二 

	int THREE = 350;
	int MIST_THREE = 180;
	int BLOCKED_THREE = 150;

	int FOUR = 900;
	int BLOCKED_FOUR = 460;

	int FIVE = 1000;
	int BLOCKED_FIVE = 500;

	int SIX = 10000;

};


//modify:引入两个头文件
class Zobrist
{
public:
	int code;//当前局面特征值
	int	White[19][19];
	int Black[19][19];
	int side;
	//上述数组的每一个都填上一个随机数，至少保证是32位的长度（即32bit)，最好是64位。初始键值也设置一个随机数。
	Zobrist()
	{
		this->chushi();
	}
	Zobrist(int computerside)
	{
		this->side = computerside;
	}
	void chushi()
	{
		for (int i = 0; i < 19; i++)
		{
			for (int j = 0; j < 19; j++)
			{
				this->White[i][j] = this->_rand();
				this->Black[i][j] = this->_rand();
			}
		}
	}
	static int _rand()
	{
		//return Math.floor(Math.random() * 1000000000);  //再多一位就溢出了。
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::mt19937 g1(seed);  // mt19937 is a standard mersenne_twister_engine  
		return g1();
	}
	int getGO(int x, int y, int role)//计算得到新的局面的code,假设目前局面的code是已知的话
	{
		//该算法的的基础利用了按位异或运算的如下性质：A^B^B = A
		this->code ^= (role == 1 ? this->White[x][y] : this->Black[x][y]);
		return (this->code);
	}

};

class ListNode
{
public:
	int code;
	int value;
	ListNode(int code, int value)
	{
		this->code = code;
		this->value = value;
	}
	ListNode()
	{

	}
};

/*************************全局量*************************/

int Board[19][19];//存储棋盘信息，其元素值为 BLACK, WHITE, EMPTY 之一
map<Point, Point> AB;//key值为一个Point，value值为该Point对应的alpha,beta值（分别存储在Point.x,Point.y中）
int virtualBoard[19][19];

Zobrist zzz;
list <ListNode>L1;


/*************************函数声明部分*************************/

//辅助函数
void copyBoard(const int Board[19][19], int virtualBoard[19][19]);
bool inside(int x, int y);
bool isAround(int virtualBoard[19][19], int x, int y);
bool operator<(const Point a, const Point b) {
	bool flag = (a.x < b.x);
	if (a.x == b.x)
		flag = (a.y < b.y);
	return flag;
}


void getVaildPoint(int virtualBoard[19][19], vector<Point>& t);
void getEmptyPoint(int virtualBoard[19][19], vector<Point>& t);
//剪枝相关
void initRootAB(Point root);
void initAB(Point father, Point son);
void upDataAB(Point p, int value, int flag);
bool isCut(Point pt);

//调用剪枝的min层和max层
int max_layer(int deep, const Point father, int f, int computerside, Step tstep, int score_Hori[19], int score_verti[19], int score_left[38], int score_right[38], int old_value);
int min_layer(int deep, const Point father, int f, int computerside, Step tstep, int score_Hori[19], int score_verti[19], int score_left[38], int score_right[38], int old_value);

//备选逻辑,正常判断不可行时采取此方案
void otherDecide(int computerside, Step& myStep);

void change_num(int directionx, int directiony, int& num);
void chess_type(int fake_Board[19][19], int x, int y, int directionX, int directionY, int computerside, int& value);
void value_situation(int fake_Board[19][19], int computerside, Step step, int& value, int score_Hori[19], int score_verti[19], int score_left[38], int score_right[38]);
int Toscore(int count, int block);
int scorePoint(int x, int y, int role, int board[19][19]);
void gen(int role, int computerside, int board[19][19], vector<Point>& answer, int time);
bool hasSix(int fake_Board[19][19], int x, int y, int directionX, int directionY);
int evalue(int fake_Board[19][19], int role);
bool isOver(int fake_Board[19][19]);



/**************************我方为白棋的时候，第一步的决策*****************************/
bool oneBlack(int& x, int& y)
{
	//判断局面是否只有一个黑棋
	int cnt = 0;
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < m; j++)
		{
			if (Board[i][j] == BLACK)
			{
				cnt++;
				x = i;
				y = j;
			}
			if (cnt > 1)
				return false;
		}
	}
	if (cnt == 1)
		return true;
}

bool initWhite(Step& myStep)
{
	//初始只有一个黑棋，白棋的位置
	int x, y;
	int xt[] = { 0,-1,0,1,1,-1,-1,1 };
	int yt[] = { -1,0,1,0,-1,-1,1,1 };
	int xx[2], yy[2];
	int cnt = 0;
	if (oneBlack(x, y))
	{
		for (int i = 0; i < 8; i++)
		{
			int tx = x + xt[i];
			int ty = y + yt[i];
			if (inside(tx, ty) && Board[tx][ty] == EMPTY)
			{
				xx[cnt] = tx;
				yy[cnt] = ty;
				cnt++;
				if (cnt == 2)
				{
					myStep.first.x = xx[0];
					myStep.first.y = yy[0];
					myStep.second.x = xx[1];
					myStep.second.y = yy[1];
					return true;
				}
			}

		}
	}
	return false;
}

//必须堵一手，第二手的抉择
int CalculateType(int num, int leftFlag, int rightFlag)
{
	switch (num)
	{
	case 0:
		return 0;
		break;
	case 1:
		if (leftFlag == 1 && rightFlag == 1) //眠
			return 90;
		else if (leftFlag == 1 || rightFlag == 1) //冲
			return 100;
		else if (leftFlag == 0 && rightFlag == 0) //活
			return 150;
		break;
	case 2:
		if (leftFlag == 1 && rightFlag == 1) //眠
			return 100;
		else if (leftFlag == 1 || rightFlag == 1) //冲
			return 100;
		else if (leftFlag == 0 && rightFlag == 0) //活
			return 200;
		break;
	case 3:
		if (leftFlag == 1 && rightFlag == 1) //眠
			return 100;
		else if (leftFlag == 1 || rightFlag == 1) //冲
			return 300;
		else if (leftFlag == 0 && rightFlag == 0) //活
			return 400;
		break;
	case 4:
		if (leftFlag == 1 && rightFlag == 1) //眠
			return 100;
		else if (leftFlag == 1 || rightFlag == 1) //冲
			return 500;
		else if (leftFlag == 0 && rightFlag == 0) //活
			return 600;
		break;
	case 5:
		if (leftFlag == 1 && rightFlag == 1) //眠
			return 100;
		else if (leftFlag == 1 || rightFlag == 1) //冲
			return 700;
		else if (leftFlag == 0 && rightFlag == 0) //活
			return 800;
		break;
	case 6:
		return 100000; //必胜情况，设定得很大，不可超越
		break;
	case 7:
		return 100000;
		break;
	case 8:
		return 100000;
		break;
	case 9:
		return 100000;
		break;
	case 10:
		return 100000;
		break;
	case 11:
		return 100000;
		break;
	}
	return 0;
}
int CalculateGrade(int board[][19], int x, int y, int myColor)
{
	int emenyColor;
	if (myColor == WHITE)
		emenyColor = BLACK;
	else if (myColor == BLACK)
		emenyColor = WHITE;
	//sum用于统计该方向颜色相同棋子的个数
	//flag用于判断该方向有没有被敌方棋子挡住
	int northSum = 0, northFlag = 0;
	int southSum = 0, southFlag = 0;
	int eastSum = 0, eastFlag = 0;
	int westSum = 0, westFlag = 0;
	int northeastSum = 0, northeastFlag = 0;
	int northwestSum = 0, northwestFlag = 0;
	int southeastSum = 0, southeastFlag = 0;
	int southwestSum = 0, southwestFlag = 0;

	int temX, temY;
	temX = x;
	temY = y - 1;
	//上
	while (temY >= 0 && board[temX][temY] == myColor)
	{
		northSum++;
		temY--;
	}
	if (temY < 0) //上面被边界或者敌方的棋子挡住了
		northFlag = 1;
	if (temY >= 0 && board[temX][temY] == emenyColor)
		northFlag = 1;

	//下
	temX = x;
	temY = y + 1;
	while (temY <= 18 && board[temX][temY] == myColor)
	{
		southSum++;
		temY++;
	}
	if (temY > 18) //上面被边界或者敌方的棋子挡住了
		southFlag = 1;
	if (temY <= 18 && board[temX][temY] == emenyColor)
		southFlag = 1;

	//左
	temX = x - 1;
	temY = y;
	while (temX >= 0 && board[temX][temY] == myColor)
	{
		eastSum++;
		temX--;
	}
	if (temX < 0) //上面被边界或者敌方的棋子挡住了
		eastFlag = 1;
	if (temX >= 0 && board[temX][temY] == emenyColor)
		eastFlag = 1;

	//右
	temX = x + 1;
	temY = y;
	while (temX <= 18 && board[temX][temY] == myColor)
	{
		westSum++;
		temX++;
	}
	if (temX > 18) //上面被边界或者敌方的棋子挡住了
		westFlag = 1;
	if (temX <= 18 && board[temX][temY] == emenyColor)
		westFlag = 1;

	//左上
	temX = x - 1;
	temY = y - 1;
	while (temX >= 0 && temY >= 0 && board[temX][temY] == myColor)
	{
		northeastSum++;
		temX--;
		temY--;
	}
	if (temX < 0) //上面被边界或者敌方的棋子挡住了
		northeastFlag = 1;
	if (temY < 0)
		northeastFlag = 1;
	if (temX >= 0 && temY >= 0 && board[temX][temY] == emenyColor)
		northeastFlag = 1;

	//右上
	temX = x + 1;
	temY = y - 1;
	while (temX <= 18 && temY >= 0 && board[temX][temY] == myColor)
	{
		northwestSum++;
		temX++;
		temY--;
	}
	if (temX > 18) //上面被边界或者敌方的棋子挡住了
		northwestFlag = 1;
	if (temY < 0)
		northwestFlag = 1;
	if (temY >= 0 && temX <= 18 && board[temX][temY] == emenyColor)
		northwestFlag = 1;

	//左下
	temX = x - 1;
	temY = y + 1;
	while (temX >= 0 && temY <= 18 && board[temX][temY] == myColor)
	{
		southeastSum++;
		temX--;
		temY++;
	}
	if (temX < 0) //上面被边界或者敌方的棋子挡住了
		southeastFlag = 1;
	if (temY > 18)
		southeastFlag = 1;
	if (temY <= 18 && temX >= 0 && board[temX][temY] == emenyColor)
		southeastFlag = 1;

	//右下
	temX = x + 1;
	temY = y + 1;
	while (temX <= 18 && temY <= 18 && board[temX][temY] == myColor)
	{
		southwestSum++;
		temX++;
		temY++;
	}
	if (temX > 18) //上面被边界或者敌方的棋子挡住了
		southwestFlag = 1;
	if (temY > 18)
		southwestFlag = 1;
	if (temY <= 18 && temX <= 18 && board[temX][temY] == emenyColor)
		southwestFlag = 1;
	int maxGrade = 0;
	int temGrade;
	//上下
	temGrade = CalculateType(northSum + southSum + 1, northFlag, southFlag);
	if (temGrade > maxGrade)
		maxGrade = temGrade;
	//左右
	temGrade = CalculateType(eastSum + westSum + 1, eastFlag, westFlag);
	if (temGrade > maxGrade)
		maxGrade = temGrade;
	//左上到右下
	temGrade = CalculateType(northeastSum + southwestSum + 1, northeastFlag, southwestFlag);
	if (temGrade > maxGrade)
		maxGrade = temGrade;
	//左下到右上
	temGrade = CalculateType(southeastSum + northwestSum + 1, southeastFlag, northwestFlag);
	if (temGrade > maxGrade)
		maxGrade = temGrade;
	return maxGrade;
}


/*******************待改进********************/
// x表示对方棋子，o表示我方棋子，以下16种情况为必须堵的情况
//应该新增必须堵一手 "oxxxx+","+xxxxo" 已增加
//"xxx+xx", "xx+xxx", "xx++xx"  已增加
//"oxxx+x+","+x+xxxo" ,"oxx+xx+","+xx+xxo","ox+xxx+","+xxx+xo"
string type[18] = { "oxxxxx+","+xxxxxo","+xxxxx+","+xxxx+","+x+xxx+" ,"+xx+xx+","+xxx+x+", "oxxxx++","++xxxxo", "xxx+xx", "xx+xxx", "xx++xx" ,"oxxx+x+","+x+xxxo"  ,"oxx+xx+","+xx+xxo","ox+xxx+","+xxx+xo", "x+xxxx","xxxx+x","x++xxx","xxx++x" };
//type数组的长度。在judgeChessType中用到
//每次新增type，需要修改此变量以及mustDone中switch(type1)的case也需要相应更新
int type_len = 22;


int judgeChessType(int fake_Board[19][19], int x, int y, int directionX, int directionY, int& end, int my)
{
	//my代表我的角色，！my来代表对方，判断对方是否出现了我必须要堵的情况
	//判断棋的类型
	//0为黑
	// return 0表示不是必须堵的情况，return的值>0则表示必须堵
	// judgeChessType返回后，end为必须走的位置
	std::string a = "#";
	int num, marko = 0, markx = 0, mark3 = 0;
	int x1 = x;
	int y1 = y;
	bool flag = false;
	for (int i = 0; i < 38; i++)//直接判断一条线
	{

		if (x1 < 0 || y1 < 0 || x1 >= 19 || y1 >= 19)
		{
			break;
		}
		else
		{

			if (fake_Board[x1][y1] == my)
			{
				std::string b = "o";
				marko++;
				a = a + b;

			}
			else if (fake_Board[x1][y1] == EMPTY)
			{
				std::string b = "+";
				mark3++;
				a = a + b;
			}
			else if (fake_Board[x1][y1] == 1 - my)
			{
				std::string b = "x";
				markx++;
				a = a + b;

			}
		}


		x1 = x1 + directionX;
		y1 = y1 + directionY;
	}

	//  type_len为type数组的长度
	for (int i = 0; i < type_len; i++)
	{
		int count = 0;
		int begin = -1;
		while ((begin = a.find(type[i], begin + 1)) != std::string::npos)
		{
			flag = true;
			begin = begin + type[i].length();
			break;
		}
		if (flag == true)
		{
			//因为有begin = begin + type[i].length();这句话，所以现在begin的值就是必须走的位置
			//但是实际上第一个字符是固定的‘#’，所以需要-1
			end = begin - 1;
			return (i + 1);
		}
	}
	return 0;
}
int getdir(int fake_Board[19][19], int& end, int& start_x, int& start_y, int& type1, int my)
{
	//判断出现该棋型的方向，end获得起始位置，type1中记录是9种必走棋型的哪一种
	// 横向 返回1 ，竖向 返回2，/ 返回3, \ 返回4
	// 返回值为-1表示没有必须走的情况
	for (int i = 0; i < 19; i++)//横竖
	{

		//横
		type1 = judgeChessType(fake_Board, i, 0, 0, 1, end, my);
		if (type1 > 0)
		{
			start_x = i;
			start_y = 0;
			return 1;
		}

		//竖
		type1 = judgeChessType(fake_Board, 0, i, 1, 0, end, my);
		if (type1 > 0)
		{
			start_x = 0;
			start_y = i;
			return 2;
		}

		/*  / */
		type1 = judgeChessType(fake_Board, 0, i, 1, -1, end, my);
		if (type1 > 0)
		{
			start_x = 0;
			start_y = i;
			return 3;
		}

		/*  \ */
		type1 = judgeChessType(fake_Board, 0, i, 1, 1, end, my);
		if (type1 > 0)
		{
			start_x = 0;
			start_y = i;
			return 4;
		}

	}
	for (int i = 1; i < 19; i++)//左倾，右倾
	{

		/*  / */
		type1 = judgeChessType(fake_Board, i, 18, 1, -1, end, my);
		if (type1 > 0)
		{
			start_x = i;
			start_y = 18;
			return 3;
		}

		/*  \ */
		type1 = judgeChessType(fake_Board, i, 0, 1, 1, end, my);
		if (type1 > 0)
		{
			start_x = i;
			start_y = 0;
			return 4;
		}

	}
	return -1;

}
//modify:将Step修改为传引用
void mustDone(int fake_Board[19][19], Step& myStep, int my)
{
	//直接调用getdir的返回值为方向竖着还是斜着什么的，end代表那个字符串的尾部
	//+OOOO+如这种情况end获取了最后一个+的位置，我必须走这步，再getdir的方向和棋型特点找第一个+位置，我下一步必须走这个

	int end = 0, start_X, start_Y, type1;
	int num1 = 0, num2 = 0;
	int mark = getdir(fake_Board, end, start_X, start_Y, type1, my);
	//没有必须走的位置，直接返回
	if (mark == -1) return;

	//有必须走的位置，先根据mark值(直线方向横1 竖2 /3 \4)计算num1 num2
	switch (mark)
	{
	case 1:
	{
		num1 = 0;
		num2 = 1;
	}; break;
	case 2:
	{
		num1 = 1;
		num2 = 0;
	}; break;
	case 3:
	{
		num1 = 1;
		num2 = -1;
	}; break;
	case 4:
	{
		num1 = 1;
		num2 = 1;
	}; break;
	default:
		break;
	}
	//                      
	//string type[18] = { 1"oxxxxx+",2"+xxxxxo",3"+xxxxx+",4"+xxxx+",5"+x+xxx+" ,6"+xx+xx+",7"+xxx+x+",
	//8"oxxxx++",9"++xxxxo",10 "xxx+xx", 11"xx+xxx", 12"xx++xx" ,13"oxxx+x+",14"+x+xxxo"  ,15"oxx+xx+",
	//"16+xx+xxo",17"ox+xxx+",18"+xxx+xo",19 "x+xxxx",20"xxxx+x",21"x++xxx",22"xxx++x" };

	//再根据type1(是7中必走棋型的哪一种)，以及num1 num2计算必走的位置
	switch (type1)
	{
	case 1:
	{
		
		myStep.first.x = start_X + (end - 1) * num1;
		myStep.first.y = start_Y + (end - 1) * num2;
		myStep.second.x = -1;
		myStep.second.y = -1;
	}; break;
	case 2:
	{
		myStep.first.x = start_X + (end - 7) * num1;
		myStep.first.y = start_Y + (end - 7) * num2;
		myStep.second.x = -1;
		myStep.second.y = -1;

	}; break;
	case 3:
	{
		myStep.first.x = start_X + (end - 1) * num1;
		myStep.first.y = start_Y + (end - 1) * num2;
		myStep.second.x = start_X + (end - 7) * num1;
		myStep.second.y = start_Y + (end - 7) * num2;

	}; break;
	case 4:
	{
		
		myStep.first.x = start_X + (end - 6) * num1;
		myStep.first.y = start_Y + (end - 6) * num2;
		myStep.second.x = start_X + (end - 1) * num1;
		myStep.second.y = start_Y + (end - 1) * num2;
	}; break;
	case 5:
	{
		myStep.first.x = start_X + (end - 5) * num1;
		myStep.first.y = start_Y + (end - 5) * num2;
		myStep.second.x = -1;
		myStep.second.y = -1;

	}; break;
	case 6:
	{
		myStep.first.x = start_X + (end - 4) * num1;
		myStep.first.y = start_Y + (end - 4) * num2;
		myStep.second.x = -1;
		myStep.second.y = -1;

	}; break;
	case 7:
	{
		
		myStep.first.x = start_X + (end - 3) * num1;
		myStep.first.y = start_Y + (end - 3) * num2;
		myStep.second.x = -1;
		myStep.second.y = -1;
	}; break;
	case 8:
	{
		myStep.first.x = start_X + (end - 2) * num1;
		myStep.first.y = start_Y + (end - 2) * num2;
		myStep.second.x = -1;
		myStep.second.y = -1;
	}; break;
	case 9:
	{
		myStep.first.x = start_X + (end - 6) * num1;
		myStep.first.y = start_Y + (end - 6) * num2;
		myStep.second.x = -1;
		myStep.second.y = -1;
	}; break;
	case 10:
	{
		
		myStep.first.x = start_X + (end - 3) * num1;
		myStep.first.y = start_Y + (end - 3) * num2;
		myStep.second.x = -1;
		myStep.second.y = -1;
	}; break;
	case 11:
	{
		myStep.first.x = start_X + (end - 4) * num1;
		myStep.first.y = start_Y + (end - 4) * num2;
		myStep.second.x = -1;
		myStep.second.y = -1;
	}; break;
	case 12:
	{
		myStep.first.x = start_X + (end - 3) * num1;
		myStep.first.y = start_Y + (end - 3) * num2;
		myStep.second.x = start_X + (end - 4) * num1;
		myStep.second.y = start_Y + (end - 4) * num2;
	}; break;
	case 13:
	{
		
		myStep.first.x = start_X + (end - 3) * num1;
		myStep.first.y = start_Y + (end - 3) * num2;
		myStep.second.x = -1;
		myStep.second.y = -1;
	}; break;
	case 14:
	{
		
		myStep.first.x = start_X + (end - 5) * num1;
		myStep.first.y = start_Y + (end - 5) * num2;
		myStep.second.x = -1;
		myStep.second.y = -1;
	}; break;
	case 15:
	{
		myStep.first.x = start_X + (end - 4) * num1;
		myStep.first.y = start_Y + (end - 4) * num2;
		myStep.second.x = -1;
		myStep.second.y = -1;
	}; break;
	case 16:
	{
		
		myStep.first.x = start_X + (end - 4) * num1;
		myStep.first.y = start_Y + (end - 4) * num2;
		myStep.second.x = -1;
		myStep.second.y = -1;
	}; break;
	case 17:
	{
		myStep.first.x = start_X + (end - 5) * num1;
		myStep.first.y = start_Y + (end - 5) * num2;
		myStep.second.x = -1;
		myStep.second.y = -1;
	}; break;
	case 18:
	{

		myStep.first.x = start_X + (end - 3) * num1;
		myStep.first.y = start_Y + (end - 3) * num2;
		myStep.second.x = -1;
		myStep.second.y = -1;
	}; break;
	case 19:
	{

		myStep.first.x = start_X + (end - 5) * num1;
		myStep.first.y = start_Y + (end - 5) * num2;
		myStep.second.x = -1;
		myStep.second.y = -1;
	}; break;
	case 20:
	{
		myStep.first.x = start_X + (end - 2) * num1;
		myStep.first.y = start_Y + (end - 2) * num2;
		myStep.second.x = -1;
		myStep.second.y = -1;
	}; break;
	case 21:
	{
		myStep.first.x = start_X + (end - 4) * num1;
		myStep.first.y = start_Y + (end - 4) * num2;
		myStep.second.x = -1;
		myStep.second.y = -1;
	}; break;
	case 22:
	{
		myStep.first.x = start_X + (end - 2) * num1;
		myStep.first.y = start_Y + (end - 2) * num2;
		myStep.second.x = -1;
		myStep.second.y = -1;
	}; break;
	default:
	{
		myStep.first.x = -1;
		myStep.first.y = -1;
		myStep.second.x = -1;
		myStep.second.y = -1;
	}
		break;
	}



}
//与mustDone的不同就是把my换成op,同时对step也有一定修改
void mustWin(int fake_Board[19][19], Step& myStep, int op)
{
	//直接调用getdir的返回值为方向竖着还是斜着什么的，end代表那个字符串的尾部
	//+OOOO+如这种情况end获取了最后一个+的位置，我必须走这步，再getdir的方向和棋型特点找第一个+位置，我下一步必须走这个

	int end = 0, start_X, start_Y, type1;
	int num1 = 0, num2 = 0;
	int mark = getdir(fake_Board, end, start_X, start_Y, type1, op);
	//没有必须走的位置，直接返回
	if (mark == -1) return;

	//有必须走的位置，先根据mark值(直线方向横1 竖2 /3 \4)计算num1 num2
	switch (mark)
	{
	case 1:
	{
		num1 = 0;
		num2 = 1;
	}; break;
	case 2:
	{
		num1 = 1;
		num2 = 0;
	}; break;
	case 3:
	{
		num1 = 1;
		num2 = -1;
	}; break;
	case 4:
	{
		num1 = 1;
		num2 = 1;
	}; break;
	default:
		break;
	}
	//string type[18] = { 1"oxxxxx+",2"+xxxxxo",3"+xxxxx+",4"+xxxx+",5"+x+xxx+" ,6"+xx+xx+",7"+xxx+x+",
	//8"oxxxx++",9"++xxxxo",10 "xxx+xx", 11"xx+xxx", 12"xx++xx" ,13"oxxx+x+",14"+x+xxxo"  ,15"oxx+xx+",
	//"16+xx+xxo",17"ox+xxx+",18"+xxx+xo",19 "x+xxxx",20"xxxx+x",21"x++xxx",22"xxx++x" };
	//再根据type1(是7中必走棋型的哪一种)，以及num1 num2计算必走的位置
	switch (type1)
	{
	case 1:
	{
		
		myStep.first.x = start_X + (end - 1) * num1;
		myStep.first.y = start_Y + (end - 1) * num2;
		myStep.second.x = -1;
		myStep.second.y = -1;
	}; break;
	case 2:
	{
		myStep.first.x = start_X + (end - 7) * num1;
		myStep.first.y = start_Y + (end - 7) * num2;
		myStep.second.x = -1;
		myStep.second.y = -1;

	}; break;
	case 3:
	{
		myStep.first.x = start_X + (end - 1) * num1;
		myStep.first.y = start_Y + (end - 1) * num2;
		myStep.second.x = start_X + (end - 7) * num1;
		myStep.second.y = start_Y + (end - 7) * num2;

	}; break;
	case 4:
	{
		
		myStep.first.x = start_X + (end - 6) * num1;
		myStep.first.y = start_Y + (end - 6) * num2;
		myStep.second.x = start_X + (end - 1) * num1;
		myStep.second.y = start_Y + (end - 1) * num2;
	}; break;
	case 5://不同于mustDone
	{
		myStep.first.x = start_X + (end - 5) * num1;
		myStep.first.y = start_Y + (end - 5) * num2;
		myStep.second.x = start_X + (end - 1) * num1;
		myStep.second.y = start_X + (end - 1) * num2;

	}; break;
	case 6://不同于mustDone
	{
		myStep.first.x = start_X + (end - 4) * num1;
		myStep.first.y = start_Y + (end - 4) * num2;
		myStep.second.x = start_X + (end - 1) * num1;
		myStep.second.y = start_X + (end - 1) * num2;

	}; break;
	case 7://不同于mustDone
	{
		
		myStep.first.x = start_X + (end - 3) * num1;
		myStep.first.y = start_Y + (end - 3) * num2;
		myStep.second.x = start_X + (end - 1) * num1;
		myStep.second.y = start_X + (end - 1) * num2;
	}; break;
	case 8://不同于mustDone
	{
		myStep.first.x = start_X + (end - 1) * num1;
		myStep.first.y = start_Y + (end - 1) * num2;
		myStep.second.x = start_X + (end - 2) * num1;
		myStep.second.y = start_X + (end - 2) * num2;
	}; break;
	case 9://不同于mustDone
	{
		myStep.first.x = start_X + (end - 6) * num1;
		myStep.first.y = start_Y + (end - 6) * num2;
		myStep.second.x = start_X + (end - 7) * num1;
		myStep.second.y = start_X + (end - 7) * num2;
	}; break;
	case 10:
	{
		
		myStep.first.x = start_X + (end - 3) * num1;
		myStep.first.y = start_Y + (end - 3) * num2;
		myStep.second.x = -1;
		myStep.second.y = -1;
	}; break;
	case 11:
	{
		myStep.first.x = start_X + (end - 4) * num1;
		myStep.first.y = start_Y + (end - 4) * num2;
		myStep.second.x = -1;
		myStep.second.y = -1;
	}; break;
	case 12:
	{
		myStep.first.x = start_X + (end - 3) * num1;
		myStep.first.y = start_Y + (end - 3) * num2;
		myStep.second.x = start_X + (end - 4) * num1;
		myStep.second.y = start_Y + (end - 4) * num2;
	}; break;
	case 13://不同
	{
		
		myStep.first.x = start_X + (end - 3) * num1;
		myStep.first.y = start_Y + (end - 3) * num2;
		myStep.second.x = start_X + (end - 1) * num1;
		myStep.second.y = start_Y + (end - 1) * num2;
	}; break;
	case 14:
	{

		myStep.first.x = start_X + (end - 5) * num1;
		myStep.first.y = start_Y + (end - 5) * num2;
		myStep.second.x = start_Y + (end - 7) * num1;
		myStep.second.y = start_Y + (end - 7) * num2;
	}; break;
	case 15:
	{
		myStep.first.x = start_X + (end - 4) * num1;
		myStep.first.y = start_Y + (end - 4) * num2;
		myStep.second.x = start_X + (end - 1) * num1;
		myStep.second.y = start_Y + (end - 1) * num2;
	}; break;
	case 16:
	{
		// "+xx+xxo", "ox+xxx+", "+xxx+xo"
		myStep.first.x = start_X + (end - 4) * num1;
		myStep.first.y = start_Y + (end - 4) * num2;
		myStep.second.x = start_X + (end - 7) * num1;
		myStep.second.y = start_X + (end - 7) * num2;
	}; break;
	case 17:
	{
		myStep.first.x = start_X + (end - 5) * num1;
		myStep.first.y = start_Y + (end - 5) * num2;
		myStep.second.x = start_X + (end - 1) * num1;
		myStep.second.y = start_X + (end - 1) * num1;
	}; break;
	case 18:
	{

		myStep.first.x = start_X + (end - 3) * num1;
		myStep.first.y = start_Y + (end - 3) * num2;
		myStep.second.x = start_X + (end - 7) * num1;
		myStep.second.y = start_X + (end - 7) * num2;
	}; break;
	case 19:
	{
		myStep.first.x = start_X + (end - 5) * num1;
		myStep.first.y = start_Y + (end - 5) * num2;
		myStep.second.x = -1;
		myStep.second.y = -1;
	}; break;
	case 20:
	{
		myStep.first.x = start_X + (end - 2) * num1;
		myStep.first.y = start_Y + (end - 2) * num2;
		myStep.second.x = -1;
		myStep.second.y = -1;
	}; break;
	case 22:
	{
		myStep.first.x = start_X + (end - 5) * num1;
		myStep.first.y = start_Y + (end - 5) * num2;
		myStep.second.x = start_X + (end - 4) * num1;
		myStep.second.y = start_Y + (end - 4) * num2;
	}; break;
	case 21:
	{
		myStep.first.x = start_X + (end - 2) * num1;
		myStep.first.y = start_Y + (end - 2) * num2;
		myStep.second.x = start_X + (end - 3) * num1;
		myStep.second.y = start_Y + (end - 3) * num2;
	}; break;
	default:
	{
		myStep.first.x = -1;
		myStep.first.y = -1;
		myStep.second.x = -1;
		myStep.second.y = -1;
	}
		break;
	}



}


/**********************************评估函数部分*********************************/
void change_num(int directionx, int directiony, int& num)
{
	if (directionx == 1 && directiony == 0)
	{
		num = 0;//竖着
	}
	else if (directionx == 0 && directiony == 1)
	{
		num = 1;//横着
	}
	else if (directionx == 1 && directiony == -1)
	{
		num = 2;//左倾
	}
	else if (directionx == 1 && directiony == 1)
	{
		num = 3;//右倾
	}
	num = -1;
}

//找到当前点的对应的 起点
void change_intoStart(int current_X, int current_Y, int& Start_Hx, int& start_Hy, int& Start_Vx, int& start_Vy, int& Start_Lx, int& start_Ly, int& Start_Rx, int& start_Ry)
{
	//找到当前点的对应的 起点
	start_Hy = 0;//横线起点 纵坐标都是0
	Start_Hx = current_X;//横线起点 横坐标都是当前的横坐标

	start_Vy = current_Y;//竖线起点 纵坐标都是当前的横坐标
	Start_Vx = 0;//竖线起点 横坐标都是0

	//左倾线
	Start_Lx = current_X;
	start_Ly = current_Y;
	while (Start_Lx > 0 && start_Ly < 18)
	{
		Start_Lx--;
		start_Ly++;
	}

	//右倾线
	Start_Rx = current_X;
	start_Ry = current_Y;
	while (Start_Rx > 0 && start_Ry > 0)
	{
		Start_Rx--;
		start_Ry--;
	}


}

void chess_type(int fake_Board[19][19], int x, int y, int directionX, int directionY, int computerside, int& value)
{
	//2020_4_13初始化修改
	value = 0;
	std::string a = "#";
	int num, marko = 0, markx = 0, mark3 = 0;
	int x1 = x;
	int y1 = y;
	bool flag = true;
	//              活六，   眠五，                 眠五，        活五，         眠四，          眠四，          活四，          眠三，         眠三，            活三，
	std::string f1 = "oooooo", f2 = "xooooo+", f3 = "+ooooox", f4 = "+ooooo+", f5 = "xoooo++", f6 = "++oooox", f7 = "++oooo++", f8 = "xooo+++", f9 = "+++ooox", f10 = "+++ooo+++";
	//              眠二       眠二               活二
	std::string f11 = "xoo++++", f12 = "++++oox", f13 = "++++oo++++";
	//朦胧三 
	std::string f14 = "x++o+oo+x", f15 = "x++oo+o+x", f16 = "x+o+oo++x", f17 = "x+oo+o++x";
	//change_num(directionX, directionY, num);
	for (int i = 0; i < 38; i++)//直接判断一条线
	{

		if (x1 < 0 || y1 < 0 || x1 >= 19 || y1 >= 19)
		{
			break;
		}
		else
		{

			if (fake_Board[x1][y1] == computerside)
			{
				std::string b = "o";
				marko++;
				a = a + b;

			}
			else if (fake_Board[x1][y1] == EMPTY)
			{
				std::string b = "+";
				mark3++;
				a = a + b;
			}
			else if (fake_Board[x1][y1] == 1 - computerside)
			{
				std::string b = "x";
				markx++;
				a = a + b;

			}
		}

		/*//活六或长连 或者 眠num             或者活num
		if (marko >= 6 || (marko + mark3 >= 6 && markx > 0) || (marko + mark3 > 6 && (marko + mark3 - 6) % 2 == 0))
		{
			break;
		}*/
		x1 = x1 + directionX;
		y1 = y1 + directionY;
	}
	//判断棋子类型
	int count = 0;
	int begin = -1;
	while ((begin = a.find(f1, begin + 1)) != std::string::npos)//活六
	{
		count++;
		begin = begin + f1.length();
	}
	value += count * 10000;
	count = 0;
	begin = -1;
	while ((begin = a.find(f4, begin + 1)) != std::string::npos)//活五
	{
		count++;
		begin = begin + f1.length();
	}
	value += count * 1000;
	count = 0;
	begin = -1;
	while ((begin = a.find(f2, begin + 1)) != std::string::npos)//眠五
	{
		count++;
		begin = begin + f2.length();
	}
	value += count * 500;
	count = 0;
	begin = -1;
	while ((begin = a.find(f3, begin + 1)) != std::string::npos)//眠五
	{
		count++;
		begin = begin + f3.length();
	}
	value += count * 500;
	count = 0;
	begin = -1;
	while ((begin = a.find(f7, begin + 1)) != std::string::npos)//活四
	{
		count++;
		begin = begin + f7.length();
	}
	value += count * 900;
	count = 0;
	begin = -1;
	while ((begin = a.find(f5, begin + 1)) != std::string::npos)//眠四
	{
		count++;
		begin = begin + f5.length();
	}
	value += count * 460;
	count = 0;
	begin = -1;
	while ((begin = a.find(f6, begin + 1)) != std::string::npos)//眠四
	{
		count++;
		begin = begin + f6.length();
	}
	value += count * 460;
	count = 0;
	begin = -1;
	while ((begin = a.find(f10, begin + 1)) != std::string::npos)//活三
	{
		count++;
		begin = begin + f10.length();
	}
	value += count * 350;
	//朦胧三
	count = 0;
	begin = -1;
	while ((begin = a.find(f14, begin + 1)) != std::string::npos)//活三
	{
		count++;
		begin = begin + f14.length();
	}
	value += count * 180;
	count = 0;
	begin = -1;
	while ((begin = a.find(f15, begin + 1)) != std::string::npos)//活三
	{
		count++;
		begin = begin + f15.length();
	}
	value += count * 180;
	count = 0;
	begin = -1;
	while ((begin = a.find(f16, begin + 1)) != std::string::npos)//活三
	{
		count++;
		begin = begin + f16.length();
	}
	value += count * 180;
	count = 0;
	begin = -1;
	while ((begin = a.find(f17, begin + 1)) != std::string::npos)//活三
	{
		count++;
		begin = begin + f17.length();
	}
	value += count * 180;

	count = 0;
	begin = -1;
	while ((begin = a.find(f8, begin + 1)) != std::string::npos)//眠三
	{
		count++;
		begin = begin + f8.length();
	}
	value += count * 150;
	count = 0;
	begin = -1;
	while ((begin = a.find(f9, begin + 1)) != std::string::npos)//眠三
	{
		count++;
		begin = begin + f9.length();
	}
	value += count * 150;
	count = 0;
	begin = -1;
	while ((begin = a.find(f13, begin + 1)) != std::string::npos)//活二
	{
		count++;
		begin = begin + f13.length();
	}
	value += count * 150;
	count = 0;
	begin = -1;
	while ((begin = a.find(f11, begin + 1)) != std::string::npos)//眠二
	{
		count++;
		begin = begin + f11.length();
	}
	value += count * 10;
	count = 0;
	begin = -1;
	while ((begin = a.find(f12, begin + 1)) != std::string::npos)//眠二
	{
		count++;
		begin = begin + f12.length();
	}
	value += count * 10;

}


//2020_4_13:value没有初始化(已修改)
//假如静态调用的话，设置step.first.x=-1;step.first.y=-1;step.second.x=-1;step.second.y=-1。
void value_situation(int fake_Board[19][19], int computerside, Step step, int& value, int score_Hori[19], int score_verti[19], int score_left[38], int score_right[38])
{
	value = 0;
	int num1 = 0, num2 = 0;
	//int score_Hori[19], score_verti[19], score_left[38], score_right[38];//横着，竖着,左倾，右倾
	int fake_Board1[19][19];
	for (int i = 0; i < 19; i++)
	{
		for (int j = 0; j < 19; j++)
		{
			fake_Board1[i][j] = fake_Board[i][j];
		}
	}
	if (step.first.x >= 0 && step.first.x >= 0 && step.first.y >= 0 && step.second.y >= 0 && step.first.x < 19 && step.second.x < 19 && step.first.y < 19 && step.second.y < 19)
	{
		fake_Board1[step.first.x][step.first.y] = computerside;
		fake_Board1[step.second.x][step.second.y] = computerside;
	}
	for (int i = 0; i < 19; i++)//横竖，搞定
	{
		score_Hori[i] = 0;
		chess_type(fake_Board1, i, 0, 0, 1, computerside, score_Hori[i]);//横

		value += score_Hori[i];
		score_verti[i] = 0;
		chess_type(fake_Board1, 0, i, 1, 0, computerside, score_verti[i]);//竖
		value += score_verti[i];

		score_left[num1] = 0;
		chess_type(fake_Board1, 0, i, 1, -1, computerside, score_left[num1]);//左

		value += score_left[num1];
		score_right[num2] = 0;
		chess_type(fake_Board1, 0, i, 1, 1, computerside, score_right[num2]);//右

		value += score_right[num2];
		num1++;
		num2++;
	}
	for (int i = 1; i < 19; i++)//左倾，右倾
	{
		score_left[num1] = 0;
		chess_type(fake_Board1, i, 18, 1, -1, computerside, score_left[num1]);//左

		value += score_left[num1];
		score_right[num2] = 0;
		chess_type(fake_Board1, i, 0, 1, 1, computerside, score_right[num2]);//右

		value += score_right[num2];
		num1++;
		num2++;
	}
}
//2020_4_13:小问题current_Y = step.first.y; Y写成了X
//Zobrist 算法
//已经有的一些局面存在List里面
int getZobrist(int fake_Board[19][19], Step step, int computerside, int score_Hori[19], int score_verti[19], int score_left[38], int score_right[38], int old_value)
{
	int mark = 0, value = 0;
	bool flag = false;
	int ccccc = zzz.code;
	mark = zzz.getGO(step.first.x, step.first.y, computerside);//第一步
	/*
	if (L1.empty() == true);
	else
	{
		std::list<ListNode>::iterator it;
		for (it = L1.begin();it != L1.end();++it)
		{
			if (it->code == mark)
			{
				flag = true;
				value = it->value;
				break;
			}
		}
	}
	if (flag != true)//没找到第一步之后的局面
	{
		value_situation(fake_Board, computerside, step, value, score_Hori, score_verti, score_left, score_right);
		ListNode a(mark, value);
		L1.push_front(a);
	}*/
	mark = zzz.getGO(step.second.x, step.second.y, computerside);//第二步
	std::list<ListNode>::iterator it1;
	for (it1 = L1.begin(); it1 != L1.end(); ++it1)
	{
		if (it1->code == mark)
		{
			return it1->value;
		}
	}
	if (old_value != -1)
	{
		int current_X, current_Y, Start_Hx, start_Hy, Start_Vx, start_Vy, Start_Lx, start_Ly, Start_Rx, start_Ry;
		int valueH1, valueV1, valueL1, valueR1, valueH2, valueV2, valueL2, valueR2;
		int cha = 0;
		//在已有局面下两步棋，得到新分数
		current_X = step.first.x;
		current_Y = step.first.y;
		change_intoStart(current_X, current_Y, Start_Hx, start_Hy, Start_Vx, start_Vy, Start_Lx, start_Ly, Start_Rx, start_Ry);
		chess_type(fake_Board, Start_Hx, start_Hy, 0, 1, computerside, valueH1);
		chess_type(fake_Board, Start_Vx, start_Vy, 1, 0, computerside, valueV1);
		chess_type(fake_Board, Start_Lx, start_Ly, 1, -1, computerside, valueL1);
		chess_type(fake_Board, Start_Rx, start_Ry, 1, 1, computerside, valueR1);
		cha += score_Hori[Start_Hx]; cha += score_verti[Start_Vx];
		score_Hori[Start_Hx] = valueH1;
		score_verti[Start_Vx] = valueV1;
		if (Start_Lx == 0)
		{
			cha += score_left[start_Ly];
			score_left[start_Ly] = valueL1;
		}
		else
		{
			cha += score_left[Start_Lx + 18];
			score_left[Start_Lx + 18] = valueL1;
		}
		if (Start_Rx == 0)
		{
			cha += score_right[start_Ry];
			score_right[start_Ry] = valueR1;
		}
		else
		{
			cha += score_right[Start_Rx + 18];
			score_right[Start_Rx + 18] = valueR1;
		}

		//printf("%d\n", cha);
		current_X = step.second.x;
		current_X = step.second.y;
		chess_type(fake_Board, Start_Hx, start_Hy, 0, 1, computerside, valueH2);
		chess_type(fake_Board, Start_Vx, start_Vy, 1, 0, computerside, valueV2);
		chess_type(fake_Board, Start_Lx, start_Ly, 1, -1, computerside, valueL2);
		chess_type(fake_Board, Start_Rx, start_Ry, 1, 1, computerside, valueR2);

		cha += score_Hori[Start_Hx];
		cha += score_verti[Start_Vx];
		score_Hori[Start_Hx] = valueH2;
		score_verti[Start_Vx] = valueV2;
		if (Start_Lx == 0)
		{
			cha += score_left[start_Ly];
			score_left[start_Ly] = valueL2;
		}
		else
		{
			cha += score_left[Start_Lx + 18];
			score_left[Start_Lx + 18] = valueL2;
		}
		if (Start_Rx == 0)
		{
			cha += score_right[start_Ry];
			score_right[start_Ry] = valueR2;
		}
		else
		{
			cha += score_right[Start_Rx + 18];
			score_right[Start_Rx + 18] = valueR2;
		}
		value = old_value - cha + valueH1 + valueH2 + valueL1 + valueL2 + valueR1 + valueR2 + valueV1 + valueV2;
		return value;
	}
	//全局重新算一次
	value_situation(fake_Board, computerside, step, value, score_Hori, score_verti, score_left, score_right);//没找到的话
	ListNode a(mark, value);
	L1.push_front(a);
	zzz.code = ccccc;
	return value;
}



void changemyBoardToPoint(vector<myBoard> my, vector<Point>& p) {

	vector<Point> tvec;

	Point tp;

	for (int i = 0; i < my.size(); i++) {

		tp.x = my[i].x;

		tp.y = my[i].y;

		tvec.push_back(tp);

	}

	p.insert(p.end(), tvec.begin(), tvec.end());

}

/*
role为当前需要评估的角色, computerside为己方执棋颜色, board为局面
为role生成可以落子的位置，存储位置在answer向量中，顺序为成六、活五、活四、双三眠四活三活二
*/
int Toscore(int count, int block) {

	Score score;




	if (count >= 6) return score.SIX;

	if (block == 0) {

		switch (count) {

		case 1: return score.ONE;

		case 2: return score.TWO;

		case 3: return score.THREE;

		case 4: return score.FOUR;

		case 5: return score.FIVE;

		}

	}

	if (block == 1) {

		switch (count) {

		case 1: return score.BLOCKED_ONE;

		case 2: return score.BLOCKED_TWO;

		case 3: return score.BLOCKED_THREE;

		case 4: return score.BLOCKED_FOUR;

		case 5: return score.BLOCKED_FIVE;

		}

	}




}


/*
给定棋盘board[19][19], 判断位置(x,y)处，角色role的分数
*/
int scorePoint(int x, int y, int role, int board[19][19]) {

	myBoard b;

	int scorecache[2][19][19][4];

	int result = 0;

	int count = 0;

	int block = 0;

	int empty = 0;

	int secondCount = 0;  //另一个方向的count

	//方向为横向 

	count = 1;

	block = 0;

	empty = -1;

	secondCount = 0;

	for (int i = y + 1; true; i++) {

		if (i >= 19) {


			break;

		}

		if (board[x][i] == 2) {


			break;

		}

		if (board[x][i] == role) {

			count++;

			continue;

		}

		else {

			block++;

			break;

		}

	}

	for (int i = y - 1; true; i--) {

		if (i < 0) {

			break;

		}

		if (board[x][i] == 2) {

			break;

		}

		if (board[x][i] == role) {

			secondCount++;
			continue;



		}

		else {

			block++;

			break;



		}

	}

	count = count + secondCount;

	scorecache[role][x][y][0] = Toscore(count, block);

	result = result + scorecache[role][x][y][0];



	//方向为竖向  

	count = 1;

	block = 0;

	empty = -1;

	secondCount = 0;

	for (int i = x + 1; true; i++) {

		if (i >= 19)

		{


			break;

		}

		if (board[i][y] == 2) {

			break;

		}

		if (board[i][y] == role) {

			count++;

			continue;

		}

		else {

			block++;

			break;

		}

	}

	for (int i = x - 1; true; i--) {

		if (i < 0) {



			break;

		}

		if (board[i][y] == 2) {

			break;

		}

		if (board[i][y] == role) {

			secondCount++;


			continue;



		}

		else {

			block++;

			break;



		}

	}

	count = count + secondCount;

	scorecache[role][x][y][1] = Toscore(count, block);



	result = result + scorecache[role][x][y][1];



	//方向为↘向  

	count = 1;

	block = 0;

	empty = -1;

	secondCount = 0;

	for (int i = 1; true; i++) {

		int m = x + i;

		int n = y + i;

		if (m >= 19 || n >= 19)

		{



			break;

		}

		if (board[m][n] == 2) {

			break;

		}

		if (board[m][n] == role) {

			count++;

			continue;

		}

		else {

			block++;

			break;

		}

	}

	for (int i = 1; true; i++) {

		int m = x - i;

		int n = y - i;

		if (m < 0 || n < 0) {



			break;

		}

		if (board[m][n] == 2) {

			break;

		}

		if (board[m][n] == role) {

			secondCount++;


			continue;



		}

		else {

			block++;

			break;



		}

	}

	count = count + secondCount;

	scorecache[role][x][y][2] = Toscore(count, block);



	result = result + scorecache[role][x][y][2];

	//方向为↙向  

	count = 1;

	block = 0;

	empty = -1;

	secondCount = 0;

	for (int i = 1; true; i++) {

		int m = x + i;

		int n = y - i;

		if (m >= 19 || n >= 19 || m < 0 || n < 0)

		{


			break;

		}

		if (board[m][n] == 2) {

			break;

		}

		if (board[m][n] == role) {

			count++;

			continue;

		}

		else {

			block++;

			break;

		}

	}

	for (int i = 1; true; i++) {

		int m = x - i;

		int n = y + i;

		if (m < 0 || n >= 19) {



			break;

		}

		if (board[m][n] == 2) {

			break;

		}

		if (board[m][n] == role) {

			secondCount++;


			continue;


		}

		else {

			block++;

			break;



		}

	}

	count = count + secondCount;

	scorecache[role][x][y][3] = Toscore(count, block);



	result = result + scorecache[role][x][y][3];



	return result;

}


//外部调用time为0，内部调用time为1
void gen(int role, int computerside, int board[19][19], vector<Point>& answer, int time) {

	/*
	role为当前需要评估的角色, computerside为己方执棋颜色, board为局面
	为role生成可以落子的位置，存储位置在answer向量中，顺序为成六、活五、活四、双三眠四活三活二
	*/

	myBoard p;
	Score S;

	//com开头代表己方，hum开头代表敌方
	vector<myBoard> comsix;
	vector<myBoard> humsix;
	vector<myBoard> comfives;
	vector<myBoard> humfives;
	vector<myBoard> comblockedfives;

	vector<myBoard> humblockedfives;

	vector<myBoard> comfours;

	vector<myBoard> humfours;

	vector<myBoard> comblockedfours;

	vector<myBoard> humblockedfours;

	vector<myBoard> comtwothrees;

	vector<myBoard> humtwothrees;

	vector<myBoard> comthrees;

	vector<myBoard> humthrees;

	vector<myBoard> comblockedthrees;

	vector<myBoard> humblockedthrees;

	vector<myBoard> comtwos;

	vector<myBoard> humtwos;

	vector<myBoard> neighbors;





	int com[19][19];
	int hum[19][19];

	memset(com, 0, sizeof(com));
	memset(hum, 0, sizeof(hum));

	vector<Point> t;
	getVaildPoint(Board, t);


	for (int i = 0; i < t.size(); i++) {

		if (board[t[i].x][t[i].y] == 2) {
			int cs = scorePoint(t[i].x, t[i].y, computerside, board);//己方分数 

			int hs = scorePoint(t[i].x, t[i].y, 1 - computerside, board);//对方分数 

			com[t[i].x][t[i].y] = cs;

			hum[t[i].x][t[i].y] = hs;

			p.humScore = hum[t[i].x][t[i].y];

			p.comScore = com[t[i].x][t[i].y];

			p.x = t[i].x;

			p.y = t[i].y;


			if (p.comScore >= S.SIX) {

				comsix.push_back(p);

			}

			else if (p.humScore >= S.SIX)

			{

				humsix.push_back(p);

			}

			else if (p.comScore >= S.FIVE) {//电脑能不能连成5

				comfives.push_back(p);

			}

			else if (p.humScore >= S.FIVE) {//玩家能不能连成5

				humfives.push_back(p);

			}

			else if (p.comScore >= S.FOUR) {

				comfours.push_back(p);

			}

			else if (p.humScore >= S.FOUR) {

				humfours.push_back(p);

			}

			else if (p.comScore >= S.BLOCKED_FOUR) {

				comblockedfours.push_back(p);

			}

			else if (p.humScore >= S.BLOCKED_FOUR) {

				humblockedfours.push_back(p);

			}

			else if (p.comScore >= S.THREE) {

				comthrees.push_back(p);

			}

			else if (p.humScore >= S.THREE) {

				humthrees.push_back(p);


			}
			else if (p.comScore >= S.BLOCKED_THREE) {
				comblockedthrees.push_back(p);
			}
			else if (p.humScore >= S.BLOCKED_THREE) {
				humblockedthrees.push_back(p);
			}

			else if (p.comScore >= S.TWO) {

				comtwos.push_back(p);

			}

			else if (p.humScore >= S.TWO) {

				humtwos.push_back(p);

			}

			else neighbors.push_back(p);

		}

	}

	answer.clear();
	int BBoard[19][19];

	//如果成六，是必杀棋，直接返回

	if (!comsix.empty())
	{

		changemyBoardToPoint(comsix, answer);

		return;

	}

	if (!humsix.empty()) {

		changemyBoardToPoint(humsix, answer);

		return;

	}

	//对方有活五或者冲五,自己活五冲五都没有，先考虑对方
	if (role == computerside) {
		if (!humfives.empty())
		{
			if (humfives.size() == 1 && time == 0) {

				for (int i = 0; i < 19; i++)
					for (int j = 0; j < 19; j++) {
						BBoard[i][j] = board[i][j];
					}
				BBoard[humfives[0].x][humfives[0].y] = role;

				vector<Point> answer_;
				gen(role, computerside, BBoard, answer_, 1);
				vector<Point> plus;
				changemyBoardToPoint(humfives, plus);

				answer.insert(answer.end(), plus.begin(), plus.end());
				answer.insert(answer.end(), answer_.begin(), answer_.end());
				return;

			}
			else {

				changemyBoardToPoint(humfives, answer);

				return;
			}
		}

		else if (!humblockedfives.empty())
		{
			if (humblockedfives.size() == 1 && time == 0) {
				for (int i = 0; i < 19; i++)
					for (int j = 0; j < 19; j++) {
						BBoard[i][j] = board[i][j];
					}
				BBoard[humblockedfives[0].x][humblockedfives[0].y] = role;

				vector<Point>answer_;
				gen(role, computerside, BBoard, answer_, 1);
				vector<Point> plus;
				changemyBoardToPoint(humblockedfives, plus);

				answer.insert(answer.end(), plus.begin(), plus.end());
				answer.insert(answer.end(), answer_.begin(), answer_.end());
				return;

			}
			changemyBoardToPoint(humblockedfives, answer);
			return;
		}
	}
	if (role == 1 - computerside && humfives.empty()) {
		if (!comfives.empty())
		{
			changemyBoardToPoint(comfives, answer);
			return;
		}
		else if (!comblockedfives.empty())
		{
			changemyBoardToPoint(comblockedfives, answer);

			return;
		}
	}


	// 对面有活四，自己冲四都没，则考虑对面活四 

	if (role == computerside) {
		if (!humfours.empty())

		{
			if (humfours.size() == 1 && time == 0) {

				for (int i = 0; i < 19; i++)
					for (int j = 0; j < 19; j++) {
						BBoard[i][j] = board[i][j];
					}
				BBoard[humfours[0].x][humfours[0].y] = role;

				vector<Point>answer_;
				gen(role, computerside, BBoard, answer_, 1);
				vector<Point> plus;
				changemyBoardToPoint(humfours, plus);

				answer.insert(answer.end(), plus.begin(), plus.end());
				answer.insert(answer.end(), answer_.begin(), answer_.end());
				return;

			}
			changemyBoardToPoint(humfours, answer);

			return;
		}
		else if (!humblockedfours.empty()) {
			if (humblockedfours.size() == 1 && time == 0) {

				for (int i = 0; i < 19; i++)
					for (int j = 0; j < 19; j++) {
						BBoard[i][j] = board[i][j];
					}

				BBoard[humblockedfours[0].x][humblockedfours[0].y] = role;

				vector<Point>answer_;
				gen(role, computerside, BBoard, answer_, 1);
				vector<Point> plus;
				changemyBoardToPoint(humblockedfours, plus);

				answer.insert(answer.end(), plus.begin(), plus.end());
				answer.insert(answer.end(), answer_.begin(), answer_.end());
				return;

			}
			changemyBoardToPoint(humblockedfours, answer);
			return;
		}

	}

	if (role == 1 - computerside) {
		if (!comfours.empty())
		{
			changemyBoardToPoint(comfours, answer);

			return;
		}
		else if (!comblockedfours.empty())
		{
			changemyBoardToPoint(comblockedfours, answer);
			return;
		}
	}

	if (role == computerside && !comfives.empty()) {
		if (comfives.size() == 1 && time == 0) {

			for (int i = 0; i < 19; i++)
				for (int j = 0; j < 19; j++) {
					BBoard[i][j] = board[i][j];
				}

			BBoard[comfives[0].x][comfives[0].y] = role;
			vector<Point>answer_;
			gen(role, computerside, BBoard, answer_, 1);
			vector<Point> plus;
			changemyBoardToPoint(comfives, plus);

			answer.insert(answer.end(), plus.begin(), plus.end());
			answer.insert(answer.end(), answer_.begin(), answer_.end());
			return;

		}

		changemyBoardToPoint(comfives, answer);

		return;

	}
	if (role == computerside && !comblockedfives.empty()) {
		if (comblockedfives.size() == 1 && time == 0) {

			for (int i = 0; i < 19; i++)
				for (int j = 0; j < 19; j++) {
					BBoard[i][j] = board[i][j];
				}

			BBoard[comblockedfives[0].x][comblockedfives[0].y] = role;

			vector<Point>answer_;
			gen(role, computerside, BBoard, answer_, 1);
			vector<Point> plus;
			changemyBoardToPoint(comblockedfives, plus);

			answer.insert(answer.end(), plus.begin(), plus.end());
			answer.insert(answer.end(), answer_.begin(), answer_.end());
			return;

		}

		changemyBoardToPoint(comblockedfives, answer);
		return;
	}



	if (role == 1 - computerside && !humfives.empty()) {

		changemyBoardToPoint(humfives, answer);

		return;

	}
	if (role == 1 - computerside && !humblockedfives.empty())
	{
		changemyBoardToPoint(humblockedfives, answer);

		return;

	}




	// 自己能活四，则直接活四，不考虑冲四

	if (role == computerside && !comfours.empty()) {
		if (comfours.size() == 1 && time == 0) {

			for (int i = 0; i < 19; i++)
				for (int j = 0; j < 19; j++) {
					BBoard[i][j] = board[i][j];
				}
			BBoard[comfours[0].x][comfours[0].y] = role;

			vector<Point>answer_;
			gen(role, computerside, BBoard, answer_, 1);
			vector<Point> plus;
			changemyBoardToPoint(comfours, plus);

			answer.insert(answer.end(), plus.begin(), plus.end());
			answer.insert(answer.end(), answer_.begin(), answer_.end());
			return;

		}

		changemyBoardToPoint(comfours, answer);

		return;

	}

	if (role == 1 - computerside && !humfours.empty()) {

		changemyBoardToPoint(humfours, answer);

		return;

	}



	if (role == computerside && !comblockedfours.empty()) {
		if (comblockedfours.size() == 1 && time == 0) {
			//cout << "...................h" << endl;
			for (int m = 0; m < 19; m++)
				for (int n = 0; n < 19; n++)
					BBoard[m][n] = board[m][n];
			BBoard[comblockedfours[0].x][comblockedfours[0].y] = role;
			vector<Point>answer_;
			gen(role, computerside, BBoard, answer_, 1);
			vector<Point> plus;
			changemyBoardToPoint(comblockedfours, plus);

			answer.insert(answer.end(), plus.begin(), plus.end());
			answer.insert(answer.end(), answer_.begin(), answer_.end());
			return;
		}

		changemyBoardToPoint(comblockedfours, answer);

		return;

	}
	if (role == 1 - computerside && !humblockedfours.empty()) {
		if (humblockedfours.size() == 1 && time == 0) {

			for (int i = 0; i < 19; i++)
				for (int j = 0; j < 19; j++) {
					BBoard[i][j] = Board[i][j];
				}
			BBoard[humblockedfours[0].x][humblockedfours[0].y] = role;

			vector<Point>answer_;
			gen(role, computerside, BBoard, answer_, 1);
			vector<Point> plus;
			changemyBoardToPoint(humblockedfours, plus);

			answer.insert(answer.end(), plus.begin(), plus.end());
			answer.insert(answer.end(), answer_.begin(), answer_.end());
			return;

		}

		changemyBoardToPoint(humblockedfours, answer);

		return;

	}

	if (role == computerside && !comthrees.empty()) {
		if (comthrees.size() == 1 && time == 0) {

			for (int i = 0; i < 19; i++)
				for (int j = 0; j < 19; j++) {
					BBoard[i][j] = Board[i][j];
				}
			BBoard[comthrees[0].x][comthrees[0].y] = role;

			vector<Point>answer_;
			gen(role, computerside, BBoard, answer_, 1);
			vector<Point> plus;
			changemyBoardToPoint(comthrees, plus);

			answer.insert(answer.end(), plus.begin(), plus.end());
			answer.insert(answer.end(), answer_.begin(), answer_.end());
			return;

		}
		changemyBoardToPoint(comthrees, answer);
		return;
	}
	if (role == 1 - computerside && !humthrees.empty()) {
		changemyBoardToPoint(humthrees, answer);
		return;
	}
	if (role == computerside && !comblockedthrees.empty()) {
		if (comblockedthrees.size() == 1 && time == 0) {

			for (int i = 0; i < 19; i++)
				for (int j = 0; j < 19; j++) {
					BBoard[i][j] = Board[i][j];
				}
			BBoard[comblockedthrees[0].x][comblockedthrees[0].y] = role;

			vector<Point>answer_;
			gen(role, computerside, BBoard, answer_, 1);
			vector<Point> plus;
			changemyBoardToPoint(comblockedthrees, plus);

			answer.insert(answer.end(), plus.begin(), plus.end());
			answer.insert(answer.end(), answer_.begin(), answer_.end());
			return;

		}
		changemyBoardToPoint(comblockedthrees, answer);
		return;

	}
	if (role == 1 - computerside && !humblockedthrees.empty()) {
		changemyBoardToPoint(humblockedthrees, answer);
		return;

	}



	vector<myBoard> result;

	if (role == computerside) {

		result.insert(result.end(), comtwothrees.begin(), comtwothrees.end());

		result.insert(result.end(), humtwothrees.begin(), humtwothrees.end());

		result.insert(result.end(), comblockedfours.begin(), comblockedfours.end());

		result.insert(result.end(), humblockedfours.begin(), humblockedfours.end());

		result.insert(result.end(), comthrees.begin(), comthrees.end());

		result.insert(result.end(), humthrees.begin(), humthrees.end());

	}



	if (role == 1 - computerside) {

		result.insert(result.end(), humtwothrees.begin(), humtwothrees.end());

		result.insert(result.end(), comtwothrees.begin(), comtwothrees.end());

		result.insert(result.end(), humblockedfours.begin(), humblockedfours.end());

		result.insert(result.end(), comblockedfours.begin(), comblockedfours.end());

		result.insert(result.end(), humthrees.begin(), humthrees.end());

		result.insert(result.end(), comthrees.begin(), comthrees.end());

	}



	//双三很特殊，因为能形成双三的不一定比一个活三强

	if (!comtwothrees.empty() || !humtwothrees.empty()) {

		changemyBoardToPoint(result, answer);

		return;

	}






	if (role == computerside && !comtwos.empty())

	{


		changemyBoardToPoint(comtwos, answer);

		return;

	}



	else if (role == 1 - computerside && !humtwos.empty()) {



		changemyBoardToPoint(humtwos, answer);


		return;

	}
	changemyBoardToPoint(neighbors, answer);

	return;




}

/**********************************************************搜索函数部分**********************************8**************************/
/**********************调用的函数************************/
/*
给定棋盘fake_Board，判定此棋盘下role一方的得分
*/
int evalue(int fake_Board[19][19], int role) {
	int value = 0;
	Step tstep;
	tstep.first.x = -1;
	tstep.first.y = -1;
	tstep.second.x = -1;
	tstep.second.y = -1;
	int score_Hori[19];
	int score_verti[19];
	int score_left[38];
	int score_right[38];
	value_situation(fake_Board, role, tstep, value, score_Hori, score_verti, score_left, score_right);
	return value;
}
/*
判断给定的fake_Board中，以(x,y)为起点的线是否有连续的六个同色棋子
*/
bool hasSix(int fake_Board[19][19], int x, int y, int directionX, int directionY) {

	std::string a = "#";
	int num, marko = 0, markx = 0, mark3 = 0;
	int x1 = x;
	int y1 = y;
	//              活六，   
	std::string f1 = "oooooo", f2 = "xxxxxx";


	for (int i = 0; i < 19; i++)//直接判断一条线
	{

		if (x1 < 0 || y1 < 0 || x1 >= 19 || y1 >= 19)
		{
			break;
		}
		else
		{

			if (fake_Board[x1][y1] == WHITE)
			{
				std::string b = "o";
				marko++;
				a = a + b;

			}
			else if (fake_Board[x1][y1] == EMPTY)
			{
				std::string b = "+";
				mark3++;
				a = a + b;
			}
			else if (fake_Board[x1][y1] == BLACK)
			{
				std::string b = "x";
				markx++;
				a = a + b;

			}
		}


		x1 = x1 + directionX;
		y1 = y1 + directionY;
	}

	//判断是否白子有6连
	int count = 0;
	int begin = -1;
	while ((begin = a.find(f1, begin + 1)) != std::string::npos)
	{
		count++;
		begin = begin + f1.length();
	}
	if (count > 0)
		return true;

	//判断是否黑子有6连
	count = 0;
	begin = -1;
	while ((begin = a.find(f2, begin + 1)) != std::string::npos)
	{
		count++;
		begin = begin + f2.length();
	}
	if (count > 0)
		return true;

	return false;
}
/*
分别判断19条竖线，19条横线，38条\，38条/中是否有连续且同色的六子
*/
bool isOver(int fake_Board[19][19]) {

	for (int i = 0; i < 19; i++)//横竖
	{

		//横
		if (hasSix(fake_Board, 0, i, 1, 0)) return true;

		//竖
		if (hasSix(fake_Board, i, 0, 0, 1)) return true;

		/*  / */
		if (hasSix(fake_Board, 0, i, 1, -1)) return true;

		/*  \ */
		if (hasSix(fake_Board, 0, i, 1, 1)) return true;

	}
	for (int i = 1; i < 19; i++)//左倾，右倾
	{

		/*  / */
		if (hasSix(fake_Board, i, 18, 1, -1)) return true;

		/*  \ */
		if (hasSix(fake_Board, i, 0, 1, 1)) return true;

	}
	return false;

}

/********************实现的函数的代码**********************/
void copyBoard(const int board[19][19], int virtualboard[19][19])
{
	//拷贝虚拟棋盘
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			virtualboard[i][j] = board[i][j];
}

//调用关系：gen -> genValidPoint -> isAround  -> inside。inside只在isAround中调用
//由于gen是经常调用的，所以inside也是经常调用的。04_14 09:24，删去isAround中的inside函数，减小栈溢出的可能(不知道效果)

bool inside(int x, int y)
{
	//判断是否在边界内
	return x >= 0 && x < n && y >= 0 && y < m;
}

//modify:增加了范围的参数，可供后续修改
bool isAround(int virtualBoard[19][19], int x, int y)
{
	//判断是否周围有棋子
	int range = 1;
	for (int i = -range; i <= range; i++)
	{
		for (int j = -range; j <= range; j++)
		{
			if (i != 0 && j != 0)
			{
				int tx = x + i;
				int ty = y + j;
				if (tx >= 0 && tx < n && ty >= 0 && ty < m && virtualBoard[tx][ty] != EMPTY)
					return true;
			}
		}
	}
	return false;
}

int initAroundCom(int x, int y, int computerside)
{
	//判断是否周围有棋子
	int range = 1;
	int cnt = 0;
	for (int i = -range; i <= range; i++)
	{
		for (int j = -range; j <= range; j++)
		{
			if (i != 0 && j != 0)
			{
				int tx = x + i;
				int ty = y + j;
				if (tx >= 0 && tx < n && ty >= 0 && ty < m && Board[tx][ty] == computerside)
					cnt++;
			}
		}
	}
	return cnt;
}
void getVaildPoint(int virtualBoard[19][19], vector<Point>& t)
{
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < m; j++)
		{
			if (virtualBoard[i][j] == EMPTY && isAround(virtualBoard, i, j))
			{
				Point pt;
				pt.x = i;
				pt.y = j;
				t.push_back(pt);
			}
		}
	}
}
/*在otherDecide中使用下面这个函数*/
void getEmptyPoint(int virtualBoard[19][19], vector<Point>& t)
{
	//获取棋盘上所有空位置
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < m; j++)
		{
			if (virtualBoard[i][j] == 2)
			{
				Point pt;
				pt.x = i;
				pt.y = j;
				t.push_back(pt);
			}
		}
	}
}

void initRootAB(Point root)
{
	//需要调用根的时候调用
	Point t;
	t.x = MIN;
	t.y = MAX;
	AB[root] = t;
}
void initAB(Point father, Point son)
{
	//初始化AB
	//放在循环调用的min/max的前一步
	Point t;
	t.x = AB[father].x;
	t.y = AB[father].y;
	AB[son] = t;

}
void upDataAB(Point p, int value, int flag)
{
	//更新AB
	//放在获得的tmp的后面
	//flag=0代表min层更新
	//flag=1代表max层更新
	if (!flag)
	{
		if (value < AB[p].y)
			AB[p].y = value;
	}
	else
	{
		if (value > AB[p].x)
			AB[p].x = value;
	}
}
bool isCut(Point pt)
{
	//在更新后进行判断
	return AB[pt].x >= AB[pt].y;
	//在best赋完值之后退出
}

/*
所做修改：增加了computerside参数
*/

void copyLine(int score_Hori[19], int score_verti[19], int score_left[38], int score_right[38], int bscore_Hori[19], int bscore_verti[19], int bscore_left[38], int bscore_right[38])
{
	int i;
	for (i = 0; i < 19; i++)
	{
		bscore_Hori[i] = score_Hori[i];
		bscore_verti[i] = score_verti[i];
		bscore_left[i] = score_left[i];
		bscore_right[i] = score_right[i];
	}
	for (; i < 38; i++)
	{
		bscore_left[i] = score_left[i];
		bscore_right[i] = score_right[i];
	}
}
void initLineArray(int bscore_Hori[19], int bscore_verti[19], int bscore_left[38], int bscore_right[38])
{
	int i;
	for (i = 0; i < 19; i++)
	{
		bscore_Hori[i] = 0;
		bscore_verti[i] = 0;
		bscore_left[i] = 0;
		bscore_right[i] = 0;
	}
	for (; i < 38; i++)
	{
		bscore_left[i] = 0;
		bscore_right[i] = 0;
	}
}

void initStep(Step& tstep)
{
	tstep.first.x = -1;
	tstep.first.y = -1;
	tstep.second.x = -1;
	tstep.second.y = -1;
}


int getValue(int f, Step tstep, int computerside, int score_Hori[19], int score_verti[19], int score_left[38], int score_right[38], int old_value)
{


	if (tstep.first.x != -1)
	{
		virtualBoard[tstep.first.x][tstep.first.y] = EMPTY;
		virtualBoard[tstep.second.x][tstep.second.y] = EMPTY;
	}
	int value = getZobrist(virtualBoard, tstep, computerside, score_Hori, score_verti, score_left, score_right, old_value);
	if (old_value != -1 && value == 0)
	{
		int value1 = initAroundCom(tstep.first.x, tstep.first.y, computerside);
		int value2 = initAroundCom(tstep.second.x, tstep.second.y, computerside);

		int value3 = initAroundCom(tstep.first.x, tstep.first.y, !computerside);
		int value4 = initAroundCom(tstep.second.x, tstep.second.y, !computerside);

		value = max(0, abs((value1 - value3) * (value2 - value4)));
	}
	if (tstep.first.x != -1)
	{
		virtualBoard[tstep.first.x][tstep.first.y] = !f;
		virtualBoard[tstep.second.x][tstep.second.y] = !f;
	}return value;
}



int min_layer(int deep, const Point father, int f, int computerside, Step tstep, int score_Hori[19], int score_verti[19], int score_left[38], int score_right[38], int old_value)
{
	//对方落子
	//deep代表深度，farther代表之前的点，f为该点标识（值为0/1，即黑/白。对于min层而言,f总为!computerside）

	//initStep(tstep);

	int score = getValue(f, tstep, computerside, score_Hori, score_verti, score_left, score_right, old_value);// = evalue(virtualBoard, f);

	if (isOver(virtualBoard) || deep <= 0)
	{
		return score;
	}
	vector<Point> t;
	gen(f, computerside, virtualBoard, t, 0);
	int re = MAX, temp;
	if (t.size() == 0)//特殊情况处理
		return MAX;
	for (int i = 0; i < t.size(); i++)
	{
		initAB(father, t[i]);//初始化AB值
		virtualBoard[t[i].x][t[i].y] = f;//落子

		vector<Point> t2;
		gen(f, computerside, virtualBoard, t2, 0);
		for (int j = 0; j < t2.size(); j++)
		{
			initAB(father, t2[j]);//初始化AB值
			virtualBoard[t2[j].x][t2[j].y] = f;

			//评估中使用
			int bscore_Hori[19];
			int bscore_verti[19];
			int bscore_left[38];
			int bscore_right[38];
			tstep.first = t[i];
			tstep.second = t2[j];
			copyLine(score_Hori, score_verti, score_left, score_right, bscore_Hori, bscore_verti, bscore_left, bscore_right);

			//将两层看成一层来考虑
			int temp1 = max_layer(deep - 1, t[i], !f, computerside, tstep, score_Hori, score_verti, score_left, score_right, score);
			int temp2 = max_layer(deep - 1, t[i], !f, computerside, tstep, score_Hori, score_verti, score_left, score_right, score);
			temp = temp1 + temp2;
			upDataAB(t[i], temp, f);//更新AB值
			upDataAB(t2[j], temp, f);//更新AB值
			//这种方式更新后二者的AB值相同判断一个剪枝即可
			if (temp < re) re = temp;
			if (isCut(t[i]))
				return re;

			virtualBoard[t2[j].x][t2[j].y] = EMPTY;
			copyLine(bscore_Hori, bscore_verti, bscore_left, bscore_right, score_Hori, score_verti, score_left, score_right);
		}
		virtualBoard[t[i].x][t[i].y] = EMPTY;//还原
	}
	return re;
}
int max_layer(int deep, const Point father, int f, int computerside, Step tstep, int score_Hori[19], int score_verti[19], int score_left[38], int score_right[38], int old_value)
{
	//己方落子
	//deep代表深度，father代表之前的点，f为该点标识(对于max层而言,f总为computerside)
	int score = getValue(f, tstep, computerside, score_Hori, score_verti, score_left, score_right, old_value);
	if (isOver(virtualBoard) || deep <= 0)//缺少判断结束
	{
		return score;
	}
	vector<Point> t;
	gen(f, computerside, virtualBoard, t, 1);
	int re = MIN, temp;//temp用于保存临时分值
	if (t.size() == 0)//特殊情况处理
		return MIN;
	for (int i = 0; i < t.size(); i++)
	{
		initAB(father, t[i]);//初始化AB值
		virtualBoard[t[i].x][t[i].y] = f;//第一步落子

		vector<Point> t2;
		gen(f, computerside, virtualBoard, t2, 0);
		for (int j = 0; j < t2.size(); j++)
		{
			initAB(father, t2[j]);//初始化AB值
			virtualBoard[t2[j].x][t2[j].y] = f;//第二步落子

			//评估中使用
			int bscore_Hori[19];
			int bscore_verti[19];
			int bscore_left[38];
			int bscore_right[38];
			tstep.first = t[i];
			tstep.second = t2[j];
			copyLine(score_Hori, score_verti, score_left, score_right, bscore_Hori, bscore_verti, bscore_left, bscore_right);

			//将两层看成一层来考虑
			int temp1 = min_layer(deep - 1, t[i], !f, computerside, tstep, score_Hori, score_verti, score_left, score_right, score);
			int temp2 = min_layer(deep - 1, t[i], !f, computerside, tstep, score_Hori, score_verti, score_left, score_right, score);
			temp = temp1 + temp2;
			upDataAB(t[i], temp, f);//更新AB值
			upDataAB(t2[j], temp, f);//更新AB值
			//这种方式更新后t[i] t2[j]二者的AB值相同判断一个剪枝即可
			if (temp > re) re = temp;
			if (isCut(t[i]))
				return re;
			virtualBoard[t2[j].x][t2[j].y] = EMPTY;
			copyLine(bscore_Hori, bscore_verti, bscore_left, bscore_right, score_Hori, score_verti, score_left, score_right);
		}
		virtualBoard[t[i].x][t[i].y] = EMPTY;//还原
	}

	return re;
}

/*
决策函数，决定所走的两步的坐标
computerside为己方执棋颜色，最终决策的结果保存在myStep中
*/
void otherDecide(int computerside, Step& myStep)
{
	int best = MIN, i, j;
	int max_i = 0, max_j = 1;
	int vBoard[19][19];
	vector<Point> v;
	getEmptyPoint(vBoard, v);
	copyBoard(Board, vBoard);
	//极端情况
	if (v.size() == 0)
	{
		myStep.first.x = rand() % 19;
		myStep.first.y = rand() % 19;
		myStep.second.x = rand() % 19;
		myStep.second.y = rand() % 19;
		return;
	}
	if (v.size() == 1)
	{
		myStep.first.x = v[0].x;
		myStep.first.y = v[0].y;
		myStep.second.x = v[0].x;
		myStep.second.y = v[0].y;
		return;
	}
	for (int i = 0; i < v.size() - 1; i++)
	{
		for (int j = i; j < v.size(); j++)
		{
			vBoard[v[i].x][v[i].y] = computerside;
			vBoard[v[j].x][v[j].y] = computerside;
			int score = evalue(vBoard, WHITE);
			if (score > best)
			{
				best = score;
				max_i = i;
				max_j = j;
			}
		}
	}
	myStep.first.x = v[max_i].x;
	myStep.first.y = v[max_i].y;
	myStep.second.x = v[max_j].x;
	myStep.second.y = v[max_j].y;
}
void chooseBest(vector<Point> vbest, Step& myStep, int computerside)
{
	//此函数在vbest中选择两个Point，这两个位置使得静态评分最高
	int re = 0, index = 0;// re=result，用来保存最高的分值
	int fakeBoard[19][19];
	copyBoard(Board, fakeBoard);
	for (int i = 0; i < vbest.size() - 1; i += 2)
	{
		fakeBoard[vbest[i].x][vbest[i].y] = computerside;
		fakeBoard[vbest[i + 1].x][vbest[i + 1].y] = computerside;

		int va = evalue(fakeBoard, computerside);
		if (va > re)
		{
			index = i;
			re = va;
		}
		fakeBoard[vbest[i].x][vbest[i].y] = EMPTY;
		fakeBoard[vbest[i + 1].x][vbest[i + 1].y] = EMPTY;
	}
	myStep.first = vbest[index];
	myStep.second = vbest[index + 1];
	myStep.value = re;
}
void decide(int computerside, Step& myStep) {
	if (computerside == WHITE) {
		if (initWhite(myStep))
			return;
	}


	vector<Point> v;
	copyBoard(Board, virtualBoard);

	//在判断是否必须堵之前，必须判断是否存在一定赢的位置
	//这个判断采取四重循环
	//调用的是简易版中的函数CalculateGrade，评估每个(i,j) (m,n)的组合的得分
	//四重循环结束之后myMaxScore的分数>=100000则表示存在必赢的情况
	myStep.first.x = -1;
	myStep.first.y = -1;
	myStep.second.x = -1;
	myStep.second.y = -1;


	mustWin(virtualBoard, myStep, 1 - computerside);//我必赢的情况，就是对方必须堵的情况
	bool sf = (myStep.first.x != -1) && (myStep.first.y != -1);// sf = step.first
	bool sc = (myStep.second.x != -1) && (myStep.second.y != -1);// sc = step.second
	if (sc) {//我必赢的两步已确定
		return;//myStep已在mustDone中赋值，此处不需要操作
	}
	if (sf && !sc) {//在这个If中继续选择下一个落子点
		virtualBoard[myStep.first.x][myStep.first.y] = computerside;//这个位置已经确定要下了
		int myMaxScore = 0;
		int tx = 0, ty = 0;
		for (int i = 0; i < 19; i++) //确定分值最大的第二个点
			for (int j = 0; j < 19; j++)
			{
				if (virtualBoard[i][j] == EMPTY)
				{
					virtualBoard[i][j] = computerside;
					double temMyScore1 = evalue(virtualBoard, computerside);
					if (temMyScore1 > myMaxScore) {
						tx = i;
						ty = j;
						myMaxScore = temMyScore1;
					}
					virtualBoard[i][j] = EMPTY; //还原这个点
				}
			}
		myStep.second.x = tx;
		myStep.second.y = ty;
		return;
	}


	//不存在必赢的情况，则判断是否存在必须堵的情况
	//初始化
	myStep.first.x = -1;
	myStep.first.y = -1;
	myStep.second.x = -1;
	myStep.second.y = -1;


	mustDone(virtualBoard, myStep, computerside);
	sf = (myStep.first.x != -1) && (myStep.first.y != -1);// sf = step.first
	sc = (myStep.second.x != -1) && (myStep.second.y != -1);// sc = step.second
	if (sc) {//如果是必须两手堵的情况，则直接两手堵
		return;//myStep已在mustDone中赋值，此处不需要操作
	}
	if (sf && !sc) {//必须堵一手的情况
		int myMaxScore = 0;
		int tx = 0, ty = 0;
		for (int i = 0; i < 19; i++) //确定分值最大的第二个点
			for (int j = 0; j < 19; j++)
			{
				if (virtualBoard[i][j] == EMPTY)
				{
					virtualBoard[i][j] = computerside;
					double temMyScore1 = evalue(virtualBoard, computerside);
					if (temMyScore1 > myMaxScore) {
						tx = i;
						ty = j;
						myMaxScore = temMyScore1;
					}
					virtualBoard[i][j] = EMPTY; //还原这个点
				}
			}
		myStep.second.x = tx;
		myStep.second.y = ty;
		return;
	}

	//必须两手堵  必须堵一手的情况都不满足才会运行到此处
	//getVaildPoint(virtualBoard,v);
	//问题
	gen(computerside, computerside, virtualBoard, v, 0);

	int best = MIN;
	int deep = 2;//没有剪枝的情况下，先搜索4层看一下计算时间
	vector<Point> vbest;

	int len = v.size();
	/*if (len <= 1)
	{
		otherDecide(computerside, myStep);
		return;
	}*/
	Step ttstep;
	initStep(ttstep);
	int score_Hori[19];
	int score_verti[19];
	int score_left[38];
	int score_right[38];
	initLineArray(score_Hori, score_verti, score_left, score_right);
	int score = getValue(-1, ttstep, computerside, score_Hori, score_verti, score_left, score_right, -1);

	for (int i = 0; i < len; i++) {


		AB.clear();
		initRootAB(v[i]);
		virtualBoard[v[i].x][v[i].y] = computerside;

		vector<Point> vt;
		gen(computerside, computerside, virtualBoard, vt, 0);
		int len2 = vt.size();

		for (int j = 0; j < len2; j++)
		{
			initRootAB(vt[j]);
			virtualBoard[vt[j].x][vt[j].y] = computerside;

			//评估中使用
			int bscore_Hori[19];
			int bscore_verti[19];
			int bscore_left[38];
			int bscore_right[38];
			ttstep.first = v[i];
			ttstep.second = vt[j];
			copyLine(score_Hori, score_verti, score_left, score_right, bscore_Hori, bscore_verti, bscore_left, bscore_right);

			int tmp1 = min_layer(deep - 1, v[i], !computerside, computerside, ttstep, score_Hori, score_verti, score_left, score_right, score);
			int tmp2 = min_layer(deep - 1, v[i], !computerside, computerside, ttstep, score_Hori, score_verti, score_left, score_right, score);
			int tmp = tmp1 + tmp2;
			if (tmp == best) {
				vbest.push_back(v[i]);
				vbest.push_back(vt[j]);
			}
			if (tmp > best) {
				best = tmp;
				vbest.clear();
				vbest.push_back(v[i]);
				vbest.push_back(vt[j]);
			}

			virtualBoard[vt[j].x][vt[j].y] = EMPTY;
			copyLine(bscore_Hori, bscore_verti, bscore_left, bscore_right, score_Hori, score_verti, score_left, score_right);
		}
		virtualBoard[v[i].x][v[i].y] = EMPTY;
	}
	//保证vbest的size大于等于2,否则下面这个函数会报错
	chooseBest(vbest, myStep, computerside);
	//myStep.first = vbest[0];
	//myStep.second = vbest[1];
}

int main()
{
	Step step;//临时步结构
	char message[256];//通信消息缓冲
	int computerSide;//己方执棋颜色
	int start = 0;//对局开始标记
	srand(int(time(0)));
	//此处放置初始化代码
	//...

	while (1)	//程序主循环
	{
		fflush(stdout);//不要删除此语句，否则程序会出问题
		scanf("%s", message);//获取平台命令消息
		//分析命令
		if (strcmp(message, "name?") == 0)//向对战平台发送队名
		{
			fflush(stdin);
			/***********将"令狐冲"改为你的队名，不超过6个汉字或12个英文字母，否则无成绩************/
			/*******/		printf("name 交啥啥都队\n");		/**只修改令狐冲，不要删除name空格****/
			/***********将"令狐冲"改为你的队名，不超过6个汉字或12个英文字母，否则无成绩************/
		}
		else if (strcmp(message, "new") == 0)//建立新棋局
		{
			int i, j;
			scanf("%s", message);//获取己方执棋颜色
			fflush(stdin);
			if (strcmp(message, "black") == 0)	computerSide = BLACK;  //执黑
			else  computerSide = WHITE;   //执白

			for (i = 0; i < 19; ++i)   //初始化棋局
				for (j = 0; j < 19; ++j)
					Board[i][j] = EMPTY;
			start = 1;

			if (computerSide == BLACK)
			{
				/**********生成第一手着法，并保存在step结构中，落子坐标为(step.first.x,step.first.y)**********/
				/****************************在下方填充代码，并替换我的示例代码******************************/


				step.first.x = 9;
				step.first.y = 9;


				/******************************在上面填充第一步行棋代码*******************************************/

				Board[step.first.x][step.first.y] = computerSide;//处理己方行棋
				printf("move %c%c@@\n", step.first.x + 'A', step.first.y + 'A');//输出着法
			}
		}
		else if (strcmp(message, "move") == 0)//行棋,本程序核心
		{
			scanf("%s", message);//获取对手行棋着法
			fflush(stdin);
			step.first.x = message[0] - 'A';		step.first.y = message[1] - 'A';
			step.second.x = message[2] - 'A';		step.second.y = message[3] - 'A';
			//处理对手行棋
			Board[step.first.x][step.first.y] = 1 - computerSide;
			if (!(step.second.x == -1 && step.second.y == -1)) Board[step.second.x][step.second.y] = 1 - computerSide;

			/**********************************************************************************************************/
			/***生成落子的坐标，保存在step结构中，第1子下在(step.first.x,step.first.y)，第2子下在(step.first.x,step.first.y)*****/
			/**************************************在下方填充代码，并替换我的示例代码*****************************************/
			Step myStep;
			decide(computerSide, myStep);
			step.first.x = myStep.first.x;
			step.first.y = myStep.first.y;
			Board[step.first.x][step.first.y] = computerSide;

			step.second.x = myStep.second.x;
			step.second.y = myStep.second.y;
			Board[step.second.x][step.second.y] = computerSide;

			/*****************************************在上面填充代码******************************************************/
			/**********************************************************************************************************/

			printf("move %c%c%c%c\n", step.first.x + 'A', step.first.y + 'A', step.second.x + 'A', step.second.y + 'A');//输出着法
		}
		else if (strcmp(message, "error") == 0)//着法错误
		{
			fflush(stdin);
		}
		else if (strcmp(message, "end") == 0)//对局结束
		{
			fflush(stdin);
			start = 0;
		}
		else if (strcmp(message, "quit") == 0)//退出引擎
		{
			fflush(stdin);
			printf("Quit!\n");
			break;
		}
	}
	return 0;
}