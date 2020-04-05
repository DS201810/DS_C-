#ifndef __MAXMIN_H__
#define __MAXMIN_H__

/***************合chess.cpp删除**************************/
struct Point { //点结构
	int x, y;
};
int Board[19][19];


/*********************增加的头文件**********************/
#include <vector>
#include <map>
using namespace std;

/**********************宏定义**************************/
#define MAX 999999
#define MIN -999999
const int n = 19;
const int m = 19;

/**********************定义的全局量*******************/
map<Point, Point> AB;//每一节点对应一个α和β，Point.x,Point.y
int virtualBoard[19][19];

/**********************调用的函数（未实现）************************/
int evalue() { ; }
bool isOver() { ; }
/*************************实现的函数及定义*************************/


//辅助函数
void copyBoard(const int Board[19][19], int virtualBoard[19][19]);
bool inside(int x, int y);
bool isAround(int x, int y);
void getVaildPoint(vector<Point>& t);
//剪枝相关
void initRootAB(Point root);
void initAB(Point father, Point son);
void upDataAB(Point p, int value, int flag);
bool isCut(Point pt);

//调用剪枝的min层和max层
int min_layer(int deep, const Point father, int f);
int max_layer(int deep, const Point father, int f);

/*************************实现的函数的代码*************************/
void copyBoard(const int Board[19][19],int virtualBoard[19][19])
{
	//拷贝虚拟棋盘
	for (int i = 0;i < n;i++)
		for (int j = 0;j < n;j++)
			virtualBoard[i][j] = Board[i][j];
}

bool inside(int x,int y)
{
	//判断是否在边界内
	return x >= 0 && x < n && y >= 0 && y < m;
}
bool isAround(int x,int y)
{
	//判断是否周围有棋子
	for (int i = -1;i <=1;i++)
	{
		for (int j = -1;j <= 1;j++)
		{
			if (i != 0 && j != 0)
			{
				int tx = x + i;
				int ty = y + j;
				if (inside(tx, ty) && Board[tx][ty] != 2)
					return true;
			}
		}
	}
	return false;
}

void getVaildPoint(vector<Point> &t)
{
	for (int i = 0;i < n;i++)
	{
		for (int j = 0;j < m;j++)
		{
			if (virtualBoard[i][j] == 2 && isAround(i, j))
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
	AB[root].x = MIN;
	AB[root].y = MAX;
}
void initAB(Point father,Point son)
{
	//初始化AB
	//放在循环调用的min/max的前一步
	AB[son].x = AB[father].x;
	AB[son].y = AB[father].y;
}
void upDataAB(Point p,int value,int flag)
{
	//更新AB
	//放在获得的tmp的后面
	//flag=0代表min层更新
	//flag=1代表max层更新
	if (!flag)
	{
		if(value<AB[p].y)
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
	return pt.x >= pt.y;
	//在best赋完值之后退出
}

int min_layer(int deep,const Point father,int f)
{
	//对方落子
	//deep代表深度，farther代表之前的点，f为该点标识
	int score = evalue();
	if (isOver()||deep<=0)//缺少判断结束
	{
		return score;
	}
	vector<Point> t;
	getVaildPoint(t);
	int re = MAX;
	for (int i = 0;i < t.size();i++)
	{
		initAB(father, t[i]);//初始化AB值
		virtualBoard[t[i].x][t[i].y] = f;//落子
		int temp = max_layer(deep-1, t[i], !f);
		upDataAB(t[i],temp, f);//更新AB值
		if (temp < re) re = temp;
		if (isCut(t[i]))
			break;
		virtualBoard[t[i].x][t[i].y] = 2;//还原
	}
	return re;
}
int max_layer(int deep, const Point father,int f)
{
	//己方落子
	//deep代表深度，father代表之前的点，f为该点标识
	int score = evalue();
	if (isOver()||deep <= 0)//缺少判断结束
	{
		return score;
	}
	vector<Point> t;
	getVaildPoint(t);
	int re = MIN;
	for (int i = 0;i < t.size();i++)
	{
		initAB(father, t[i]);//初始化AB值
		virtualBoard[t[i].x][t[i].y] = f;//落子
		int temp = min_layer(deep-1, t[i], !f);
		upDataAB(t[i], temp, f);//更新AB值
		if (temp > re) re = temp;
		if (isCut(t[i]))
			break;
		virtualBoard[t[i].x][t[i].y] = 2;//还原
	}
	return re;
}

#endif