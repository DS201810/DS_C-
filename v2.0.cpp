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
/*
在maxmin.h中编译时没有重载Point的运算符也可以正常编译，合在一起之后则会报错没有定义参数为(Point,Point)的<、
网上的解释是map的底层是红黑树实现的，将map用于自定义类型需要定义<运算符
*/

//modify
//如果仅依据x判断，当x值相同y值不同的点，会被认为为相同的点，在map中被覆盖
bool operator<(const Point a, const Point b) {
	bool flag = (a.x<b.x);
	if (a.x == b.x)
		flag = (a.y < b.y);
	return flag;
}

struct Step { //步结构
	Point first, second;
	int value;
};
int Board[19][19];//存储棋盘信息，其元素值为 BLACK, WHITE, EMPTY 之一

/*
myBoard类：保存坐标(x,y),及该节点对应的双方分数
（原本这个类叫做Board，但是与老师的Board[19][19]，当出现语句Board b时，编译器无法判断这个Board是类还是数组，故会报错）
*/
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
map<Point, Point> AB;//key值为一个Point，value值为该Point对应的alpha,beta值（分别存储在Point.x,Point.y中）
int virtualBoard[19][19];



/*************************函数声明部分*************************/

//辅助函数
void copyBoard(const int Board[19][19], int virtualBoard[19][19]);
bool inside(int x, int y);
bool isAround(int x, int y);
//modify:增加了参数
void getVaildPoint(int virtualBoard[19][19],vector<Point>& t);
void getEmptyPoint(int virtualBoard[19][19], vector<Point>& t);
//剪枝相关
void initRootAB(Point root);
void initAB(Point father, Point son);
void upDataAB(Point p, int value, int flag);
bool isCut(Point pt);

//调用剪枝的min层和max层
int min_layer(int deep, const Point father, int f, int computerside);
int max_layer(int deep, const Point father, int f, int computerside);

//备选逻辑
void otherDecide(int computerside, Step& myStep);

void change_num(int directionx, int directiony, int& num);
void chess_type(int fake_Board[19][19], int x, int y, int directionX, int directionY, int computerside, int& value);
void value_situation(int fake_Board[19][19], int computerside, int& value);
int Toscore(int count, int block, int empty);
int scorePoint(int x, int y, int role, int board[19][19]);
void gen(int role, int computerside, int board[19][19], vector<Point>& t);
bool hasSix(vector<int> t);
int evalue(int fake_Board[19][19], int role);
bool isOver(int fake_Board[19][19]);


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

/*
value_situation需要使用的辅助函数
*/
void chess_type(int fake_Board[19][19], int x, int y, int directionX, int directionY, int computerside, int& value)
{
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

	change_num(directionX, directionY, num);
	for (int i = 0; i < 19; i++)//直接判断一条线
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
	value += count * MAX;
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

/*
这里的computerside与老师的程序中computerSide意义不同，可以将这里的理解为role
此函数的作用为：为computerside一方计算其在fake_Board棋盘下的得分，结果存在value中
*/

//modify: value初始化为0
void value_situation(int fake_Board[19][19], int computerside, int& value)
{
	value = 0;
	int new_Board[19][19][4], num1 = 0, num2 = 0;
	int score_Hori[19], score_verti[19], score_left[38], score_right[38];//横着，竖着,左倾，右倾
	for (int i = 0; i < 19; i++)//横竖，搞定
	{
		score_Hori[i] = 0;
		chess_type(fake_Board, 0, i, 1, 0, computerside, score_Hori[i]);//横

		value += score_Hori[i];
		score_verti[i] = 0;
		chess_type(fake_Board, i, 0, 0, 1, computerside, score_verti[i]);//竖
		value += score_verti[i];

		score_left[num1] = 0;
		chess_type(fake_Board, 0, i, 1, -1, computerside, score_left[num1]);//左

		value += score_left[num1];
		score_right[num2] = 0;
		chess_type(fake_Board, 0, i, 1, 1, computerside, score_right[num2]);//右

		value += score_right[num2];
		num1++;
		num2++;
	}
	for (int i = 1; i < 19; i++)//左倾，右倾
	{
		score_left[num1] = 0;
		chess_type(fake_Board, 0, i, 1, -1, computerside, score_left[num1]);//左

		value += score_left[num1];
		score_right[num2] = 0;
		chess_type(fake_Board, 0, i, 1, 1, computerside, score_right[num2]);//右

		value += score_right[num2];
		num1++;
		num2++;
	}
}

/*
count表示同色的棋子的个数，block表示两头被堵了几头，empty表示空格出现的位置
*/
int Toscore(int count, int block, int empty) {
	Score score;

	if (empty <= 0) {
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
	else if (empty == 1 || empty == count - 1) {
		//第1个是空位
		if (count >= 6) {
			return score.SIX;
		}
		if (block == 0) {
			switch (count) {
			case 2: return score.TWO / 2;
			case 3: return score.MIST_TWO;
			case 4: return score.MIST_THREE;
			case 5: return score.FOUR;
			}
		}
		if (block == 1) {
			switch (count) {
			case 2: return score.BLOCKED_TWO;
			case 3: return score.BLOCKED_THREE;
			case 4: return score.BLOCKED_FOUR;
			case 5: return score.BLOCKED_FIVE;
			}
		}
	}
	else if (empty == 2 || empty == count - 2) {
		//第二个是空位
		if (count >= 7) {
			return score.FIVE;
		}
		if (block == 0) {
			switch (count) {
			case 3: return score.MIST_TWO;
			case 4: return score.THREE;
			case 5: return score.BLOCKED_FOUR;
			case 6: return score.FOUR;
			}
		}
		if (block == 1) {
			switch (count) {
			case 3: return score.BLOCKED_THREE;
			case 4: return score.BLOCKED_FOUR;
			case 5: return score.BLOCKED_FOUR;
			case 6: return score.FOUR;
			}
		}
		if (block == 2) {
			switch (count) {
			case 4: return 0;
			case 5:return 0;
			case 6: return score.BLOCKED_FOUR;
			}
		}
	}
	else if (empty == 3 || empty == count - 3) {
		if (count >= 9) {
			return score.SIX;
		}
		if (block == 0) {
			switch (count) {
			case 4:return score.THREE;
			case 5: return score.THREE;
			case 6: return score.BLOCKED_FOUR;
			case 7: return score.FOUR;
			case 8:return score.FIVE;
			}
		}
		if (block == 1) {
			switch (count) {
			case 4:return score.BLOCKED_THREE;
			case 5:return score.THREE;
			case 6: return score.BLOCKED_FOUR;
			case 7: return score.FOUR;
			case 8:return score.BLOCKED_FIVE;
			}
		}
		if (block == 2) {
			switch (count) {
			case 4: return 0;
			case 5:return 0;
			case 6:return score.BLOCKED_THREE;
			case 7: return score.BLOCKED_FOUR;
			}
		}
	}
	else if (empty == 4 || empty == count - 4) {
		if (count >= 10) {
			return score.SIX;
		}
		if (block == 0) {
			switch (count) {
			case 5:return score.FOUR;
			case 6:return score.FOUR;
			case 7:return score.FOUR;
			case 8: return score.FOUR;
			case 9:return score.FIVE;
			}
		}
		if (block == 1) {
			switch (count) {
			case 4:return score.BLOCKED_FOUR;
			case 5:return score.BLOCKED_FOUR;
			case 6:return score.BLOCKED_FOUR;
			case 7: return score.BLOCKED_FOUR;
			case 8: return score.FOUR;
			case 9: return score.BLOCKED_FIVE;
			}
		}
		if (block == 2) {
			switch (count) {
			case 5:return score.BLOCKED_TWO;
			case 6:return score.BLOCKED_THREE;
			case 7:return score.BLOCKED_FOUR;
			case 8: return score.BLOCKED_FOUR;
			case 9:return score.BLOCKED_FIVE;
			}
		}
	}
	else if (empty == 5 || empty == count - 5) {
		if (count >= 11) return score.SIX;
		if (block == 0) {
			switch (count) {
			case 6:return score.FIVE;
			case 7:return score.FIVE;
			case 8:return score.FIVE;
			case 9:return score.FOUR * 2;
			case 10:return score.FIVE * 2;
			}
		}
		if (block == 1) {
			switch (count) {
			case 6:return score.BLOCKED_FIVE;
			case 7:return score.BLOCKED_FIVE;
			case 8:return score.BLOCKED_FIVE;
			case 9:return score.BLOCKED_FOUR * 2;
			case 10:return score.FIVE * 2;
			}
		}
		if (block == 2) {
			switch (count) {
			case 6:return score.BLOCKED_FOUR;
			case 7:return score.BLOCKED_FOUR;
			case 8:return score.BLOCKED_FOUR;
			case 9:return score.BLOCKED_FOUR * 2;
			case 10:return score.FIVE * 2;
			}
		}


	}
	else if (empty == 6 || empty == count - 6)
		return score.SIX;
	return 0;
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
			block++;
			break;
		}
		if (board[x][i] == 2) {
			if (empty == -1 && i < 18 && board[x][i + 1] == role)
			{
				empty = count;
				continue;
			}
			else break;
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
			block++;
			break;
		}
		if (board[x][i] == 2) {
			if (empty == -1 && i > 0 && board[x][i - 1] == role) {
				empty = 0;  //注意这里是0，因为是从右往左走的
				continue;
			}
			else
				break;
		}
		if (board[x][i] == role) {
			secondCount++;
			if (empty != -1)
			{
				empty++;  //注意这里，如果左边又多了己方棋子，那么empty的位置就变大了
				continue;
			}
		}
		else {
			block++;
			break;

		}
	}
	count = count + secondCount;
	scorecache[role][x][y][0] = Toscore(count, block, empty);
	result = result + scorecache[role][x][y][0];

	//方向为竖向  
	count = 1;
	block = 0;
	empty = -1;
	secondCount = 0;
	for (int i = x + 1; true; i++) {
		if (i >= 19)
		{
			block++;
			break;
		}
		if (board[i][y] == 2) {
			if (empty == -1 && i < 18 && board[x + 1][y] == role)
			{
				empty = count;
				continue;
			}
			else break;
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
			block++;
			break;
		}
		if (board[i][y] == 2) {
			if (empty == -1 && i > 0 && board[x - 1][y] == role) {
				empty = 0;  //注意这里是0，因为是从右往左走的
				continue;
			}
			else  break;
		}
		if (board[i][y] == role) {
			secondCount++;
			if (empty != -1)
			{
				empty++;  //empty的位置变大 
				continue;
			}
		}
		else {
			block++;
			break;

		}
	}
	count = count + secondCount;
	scorecache[role][x][y][1] = Toscore(count, block, empty);

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
			block++;
			break;
		}
		if (board[m][n] == 2) {
			if (empty == -1 && (m < 18 && n < 18) && board[m + 1][n + 1] == role)
			{
				empty = count;
				continue;
			}
			else break;
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
			block++;
			break;
		}
		if (board[m][n] == 2) {
			if (empty == -1 && m > 0 && n > 0 && board[m - 1][n - 1] == role) {
				empty = 0;  //注意这里是0，因为是从右往左走的
				continue;
			}
			else  break;
		}
		if (board[m][n] == role) {
			secondCount++;
			if (empty != -1)
			{
				empty++; //注意这里，如果左边又多了己方棋子，那么empty的位置就变大了
				continue;
			}
		}
		else {
			block++;
			break;

		}
	}
	count = count + secondCount;
	scorecache[role][x][y][2] = Toscore(count, block, empty);

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
			block++;
			break;
		}
		if (board[m][n] == 2) {
			if (empty == -1 && (m < 18 && n>0) && board[m + 1][n - 1] == role)
			{
				empty = count;
				continue;
			}
			else break;
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
			block++;
			break;
		}
		if (board[m][n] == 2) {
			if (empty == -1 && m > 0 && n > 0 && board[m - 1][n + 1] == role) {
				empty = 0;  //注意这里是0，因为是从右往左走的
				continue;
			}
			else  break;
		}
		if (board[m][n] == role) {
			secondCount++;
			if (empty != -1)
			{
				empty++; //注意这里，如果左边又多了己方棋子，那么empty的位置就变大了
				continue;
			}
		}
		else {
			block++;
			break;

		}
	}
	count = count + secondCount;
	scorecache[role][x][y][3] = Toscore(count, block, empty);

	result = result + scorecache[role][x][y][3];

	return result;
}

/*
去掉vector<myBoard> my的分数信息，保存其对应的坐标值，并附加在p向量的尾部
*/
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

//modify: 先阶段有问题，不能按排序生成数组，暂时返回初始的
void gen(int role, int computerside, int board[19][19], vector<Point>& answer) {
	
	//临时解决方案
	getVaildPoint(board, answer);
		return;

	myBoard p;
	Score S;

	//com开头代表己方，hum开头代表敌方
	vector<myBoard> comsix;
	vector<myBoard> humsix;
	vector<myBoard> comfives;
	vector<myBoard> humfives;
	vector<myBoard> comfours;
	vector<myBoard> humfours;
	vector<myBoard> comblockedfours;
	vector<myBoard> humblockedfours;
	vector<myBoard> comtwothrees;
	vector<myBoard> humtwothrees;
	vector<myBoard> comthrees;
	vector<myBoard> humthrees;
	vector<myBoard> comtwos;
	vector<myBoard> humtwos;
	vector<myBoard> neighbors;


	int com[19][19];
	int hum[19][19];
	memset(com, 0, sizeof(com));
	memset(hum, 0, sizeof(hum));
	vector<Point> t;
	getVaildPoint(virtualBoard,t);
	
	for (int i = 0; i < t.size(); i++) {

		if (board[t[i].x][t[i].y] == 2) {
			//必须是有邻居的才行
			int cs = scorePoint(t[i].x, t[i].y, computerside, board);//己方分数 
			int hs = scorePoint(t[i].x, t[i].y, 1 - computerside, board);//对方分数 
			com[t[i].x][t[i].y] = cs;
			hum[t[i].x][t[i].y] = hs;
		}
		//此处删去对已落子处的棋子的评分

		if (board[t[i].x][t[i].y] == 2) {
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
			else if (p.comScore >= 2 * S.THREE) {
				//能成双三也行
				comtwothrees.push_back(p);
			}
			else if (p.humScore >= 2 * S.THREE) {
				humtwothrees.push_back(p);
			}
			else if (p.comScore >= S.THREE) {
				comthrees.push_back(p);
			}
			else if (p.humScore >= S.THREE) {
				humthrees.push_back(p);
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
	//如果成六，是必杀棋，直接返回
	if (!comsix.empty()){
		changemyBoardToPoint(comsix, answer);
		return ;
	}
	if (!humsix.empty()){
		changemyBoardToPoint(humsix, answer);
		return ;
	}

	if (role == computerside && !comfives.empty()) {
		changemyBoardToPoint(comfives, answer);
		return;
	}
		
	if (role == 1 - computerside && !humfives.empty()) {
		changemyBoardToPoint(comfives, answer);
		return;
	}


	// 自己能活四，则直接活四，不考虑冲四
	if (role == computerside && !comfours.empty()) {
		changemyBoardToPoint(comfours, answer);
		return;
	}
	if (role == 1 - computerside && !humfours.empty()) {
		changemyBoardToPoint(humfours, answer);
		return;
	}


	// 对面有活四冲四，自己冲四都没，则只考虑对面活四 （此时对面冲四就不用考虑了)
	if (role == computerside && !humfours.empty() && comblockedfours.empty()) {
		changemyBoardToPoint(humfours, answer);
		return;
	}
	if (role == 1 - computerside && !comfours.empty() && humblockedfours.empty()) {
		changemyBoardToPoint(comfours, answer);
		return;
	}


	// 对面有活四自己有冲四，则都考虑下
	vector<myBoard> fours;
	vector<myBoard>blockedfours;
	if (role == computerside) {
		fours.insert(fours.end(), comfours.begin(), comfours.end());
		fours.insert(fours.end(), humfours.begin(), humfours.end());

		blockedfours.insert(blockedfours.end(), comblockedfours.begin(), comblockedfours.end());
		blockedfours.insert(blockedfours.end(), humblockedfours.begin(), humblockedfours.end());
	}
	else {
		fours.insert(fours.end(), humfours.begin(), humfours.end());
		fours.insert(fours.end(), comfours.begin(), comfours.end());

		blockedfours.insert(blockedfours.end(), humblockedfours.begin(), humblockedfours.end());
		blockedfours.insert(blockedfours.end(), comblockedfours.begin(), comblockedfours.end());
	}

	if (!fours.empty())
	{
		vector<myBoard> sum;
		sum.insert(sum.end(), fours.begin(), fours.end());
		sum.insert(sum.end(), blockedfours.begin(), blockedfours.end());
		changemyBoardToPoint(sum, answer);
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

	// 只返回大于等于活三的棋
	vector<myBoard> twos;
	if (role == computerside)
	{
		twos.insert(twos.end(), comtwos.begin(), comtwos.end());
		twos.insert(twos.end(), humtwos.begin(), humtwos.end());
	}

	else {
		twos.insert(twos.end(), humtwos.begin(), humtwos.end());
		twos.insert(twos.end(), comtwos.begin(), comtwos.end());
	}
	changemyBoardToPoint(twos, answer);
	return;

}

/**********************************搜索函数部分*********************************/
/**********************调用的函数************************/
/*
给定棋盘fake_Board，判定此棋盘下role一方的得分
*/
int evalue(int fake_Board[19][19], int role) {
	int value=0;
	value_situation(fake_Board, role, value);
	return value; 
}

/*
判断给定的vector中是否有连续的六个同色棋子（八个方向均转为一个vector）
*/
bool hasSix(vector<int> t) {
	int len = t.size();
	int i = 0, count = 0;//i是t的下角标，count是目前最大的连续数
	while (i < len && t[i] == 2) i++;
	int currentside = t[i];
	count = 1;
	for (; i < len; i++) {
		if (t[i] == currentside) {
			count++;
			if (count == 6)
				return true;
		}
		else {
			currentside = t[i];
			count = 1;
		}
	}
	
	return false;
}

/*
将19条竖线，19条横线，27条\，27条/分别转化成一个vector来判断是否有连六
最多可放棋子不足6个的共20条线，不在转化范围内
*/

//有问题
bool isOver(int fake_Board[19][19]) {
	return false;
	vector<int> t;
	t.clear();
	//横排
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			t.push_back(fake_Board[i][j]);
		}
		if (hasSix(t)) 
			return true;
		else 
			t.clear();
	}
	//竖排
	for (int j = 0; j < m; j++) {
		for (int i = 0; i < n; i++) {
			t.push_back(fake_Board[i][j]);
		}
		if (hasSix(t))
			return true;
		else
			t.clear();
	}
	/*  下三角中的\ */ 
	for (int i = 13; i >= 0; i--) {
		int j = 0;
		int dx = 1, dy = 1, currentx = i, currenty = j;
		int num = 19 - i;//这一条线上最多有num个棋子
		while (num > 0) {
			t.push_back(fake_Board[currentx][currenty]);
			num--;
			currentx += dx;
			currenty += dy;
		}
		if (hasSix(t))
			return true;
		else
			t.clear();
	}
	/*  上三角中的\ */
	for (int j = 2; j <= 13; j++) {
		int i = 0;
		int dx = 1, dy = 1, currentx = i, currenty = j;
		int num = 19 - j;//这一条线上最多有num个棋子
		while (num > 0) {
			t.push_back(fake_Board[currentx][currenty]);
			num--;
			currentx += dx;
			currenty += dy;
		}
		if (hasSix(t))
			return true;
		else
			t.clear();
	}
	/*  上三角中的/ */
	for (int i = 5; i < 19; i++) {
		int j = 0;
		int dx = -1, dy = 1, currentx = i, currenty = j;
		int num = i + 1;//这一条线上最多有num个棋子
		while (num > 0) {
			t.push_back(fake_Board[currentx][currenty]);
			num--;
			currentx += dx;
			currenty += dy;
		}
		if (hasSix(t))
			return true;
		else
			t.clear();
	}
	/*  下三角中的/ */
	for (int j = 2; j <= 13; j++) {
		int i = 18;
		int dx = -1, dy = 1, currentx = i, currenty = j;
		int num = 19 - j;//这一条线上最多有num个棋子
		while (num > 0) {
			t.push_back(fake_Board[currentx][currenty]);
			num--;
			currentx += dx;
			currenty += dy;
		}
		if (hasSix(t))
			return true;
		else
			t.clear();
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

bool inside(int x, int y)
{
	//判断是否在边界内
	return x >= 0 && x < n && y >= 0 && y < m;
}
//modify:增加了范围的参数，可供后续修改
bool isAround(int virtualBoard[19][19],int x, int y)
{
	//判断是否周围有棋子
	int range = 2;
	for (int i = -range; i <= range; i++)
	{
		for (int j = -range; j <= range; j++)
		{
			if (i != 0 && j != 0)
			{
				int tx = x + i;
				int ty = y + j;
				if (inside(tx, ty) && virtualBoard[tx][ty] != 2)
					return true;
			}
		}
	}
	return false;
}

void getVaildPoint(int virtualBoard[19][19],vector<Point> &t)
{
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < m; j++)
		{
			if (virtualBoard[i][j] == EMPTY && isAround(virtualBoard,i, j))
			{
				Point pt;
				pt.x = i;
				pt.y = j;
				t.push_back(pt);
			}
		}
	}
}
void getEmptyPoint(int virtualBoard[19][19],vector<Point>& t)
{
	//获取棋盘上所有空位置
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < m; j++)
		{
			if (virtualBoard[i][j] == 2 )
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

int min_layer(int deep, const Point father, int f, int computerside)
{
	//对方落子
	//deep代表深度，farther代表之前的点，f为该点标识（值为0/1，即黑/白）
	int score = evalue(virtualBoard, WHITE);
	if (isOver(virtualBoard) || deep <= 0)
	{
		return score;
	}
	vector<Point> t;
	gen(f, computerside, virtualBoard, t);
	int re = MAX,temp;
	if (t.size() == 0|| t.size() == 1)//特殊情况处理
		return MAX;
	for (int i = 0; i < t.size()-1 ; i+=2)
	{
		initAB(father, t[i]);//初始化AB值
		initAB(father, t[i+1]);//初始化AB值
		virtualBoard[t[i].x][t[i].y] = f;//落子
		virtualBoard[t[i+1].x][t[i+1].y] = f;

		//Point confar;
		//confar.x = max(t[i].x, t[i + 1].x);
		//confar.y = min(t[i].y, t[i + 1].y);
		
		//将两层看成一层来考虑
		int temp1 = max_layer(deep - 1, t[i], !f, computerside);
		int temp2 = max_layer(deep - 1, t[i], !f, computerside);
		temp = temp1 + temp2;
		upDataAB(t[i], temp, f);//更新AB值
		upDataAB(t[i+1], temp, f);//更新AB值
		//这种方式更新后二者的AB值相同判断一个剪枝即可
		if (temp < re) re = temp;
		if (isCut(t[i]))
			break;
		virtualBoard[t[i].x][t[i].y] = 2;//还原
		virtualBoard[t[i + 1].x][t[i + 1].y] = 2;
	}
	return re;
}
int max_layer(int deep, const Point father, int f, int computerside)
{
	//己方落子
	//deep代表深度，father代表之前的点，f为该点标识
	int score = evalue(virtualBoard, BLACK);
	if (isOver(virtualBoard) || deep <= 0)//缺少判断结束
	{
		return score;
	}
	vector<Point> t;
	gen(f, computerside, virtualBoard, t);
	int re = MIN,temp;
	if (t.size() == 0 || t.size() == 1)//特殊情况处理
		return MIN;
	for (int i = 0; i < t.size() - 1; i += 2)
	{
		initAB(father, t[i]);//初始化AB值
		initAB(father, t[i + 1]);//初始化AB值
		virtualBoard[t[i].x][t[i].y] = f;//落子
		virtualBoard[t[i + 1].x][t[i + 1].y] = f;

		//Point confar;
		//confar.x = max(t[i].x, t[i + 1].x);
		//confar.y = min(t[i].y, t[i + 1].y);

		//将两层看成一层来考虑
		int temp1 = min_layer(deep - 1, t[i], !f, computerside);
		int temp2 = min_layer(deep - 1, t[i], !f, computerside);
		temp = temp1 + temp2;
		upDataAB(t[i], temp, f);//更新AB值
		upDataAB(t[i+1], temp, f);//更新AB值
		//这种方式更新后二者的AB值相同判断一个剪枝即可
		if (temp > re) re = temp;
		if (isCut(t[i]))
			break;
		virtualBoard[t[i].x][t[i].y] = 2;//还原
		virtualBoard[t[i + 1].x][t[i + 1].y] = 2;
	}
	return re;
}

/*
决策函数，决定所走的两步的坐标
computerside为己方执棋颜色，最终决策的结果保存在myStep中
*/
void otherDecide(int computerside, Step& myStep)
{
	int best = MIN,i,j;
	int max_i=0, max_j=1;
	int vBoard[19][19];
	vector<Point> v;
	getEmptyPoint(vBoard,v);
	copyBoard(Board, vBoard);
	if (v.size() == 0)
	{
		myStep.first.x = rand()%19;
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
	for (int i = 0;i < v.size()-1;i++)
	{
		for (int j = i;j < v.size();j++)
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
void decide(int computerside, Step& myStep) {
	vector<Point> v;

	copyBoard(Board, virtualBoard);
	getVaildPoint(virtualBoard,v);
	
	int best = MIN;
	int deep = 2;//没有剪枝的情况下，先搜索4层看一下计算时间
	vector<Point> vbest;

	int len = v.size();
	/*if (len <= 1)
	{
		otherDecide(computerside, myStep);
		return;
	}*/
	for (int i = 0; i < len-1; i=i+2) {
		AB.clear();
		initRootAB(v[i]);
		initRootAB(v[i + 1]);
		virtualBoard[v[i].x][v[i].y] = computerside;
		virtualBoard[v[i+1].x][v[i+1].y] = computerside;
		int tmp1 = min_layer(deep - 1, v[i], !computerside, computerside);
		int tmp2 = min_layer(deep - 1, v[i], !computerside, computerside);
		int tmp = tmp1 + tmp2;
		if (tmp == best) {
			vbest.push_back(v[i]);
			vbest.push_back(v[i+1]);
		}
		if (tmp > best) {
			best = tmp;
			vbest.clear();
			vbest.push_back(v[i]);
			vbest.push_back(v[i + 1]);
		}
		virtualBoard[v[i].x][v[i].y] = EMPTY;
		virtualBoard[v[i + 1].x][v[i + 1].y] = EMPTY;
	}
	myStep.first.x = vbest[0].x;
	myStep.first.y = vbest[0].y;
	myStep.second.x = vbest[1].x;
	myStep.second.y = vbest[1].y;
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