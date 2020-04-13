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

/**********************�궨��**************************/
#define BLACK 0
#define WHITE 1
#define EMPTY 2
#define MAX 99999999
#define MIN -99999999
const int n = 19;
const int m = 19;


/**********************************�Զ�����������*********************************/
struct Point { //��ṹ
	int x, y;
};


struct Step { //���ṹ
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
	int ONE = 5; //��һ 
	int BLOCKED_ONE = 1;//��һ 

	int TWO = 150;//��� 
	int MIST_TWO = 100;//���ʶ�  
	int BLOCKED_TWO = 10;//�߶� 

	int THREE = 350;
	int MIST_THREE = 180;
	int BLOCKED_THREE = 150;

	int FOUR = 900;
	int BLOCKED_FOUR = 460;

	int FIVE = 1000;
	int BLOCKED_FIVE = 500;

	int SIX = 10000;

};


//modify:��������ͷ�ļ�
class Zobrist
{
public:
	int code;//��ǰ��������ֵ
	int	White[19][19];
	int Black[19][19];
	int side;
	//���������ÿһ��������һ������������ٱ�֤��32λ�ĳ��ȣ���32bit)�������64λ����ʼ��ֵҲ����һ���������
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
		for (int i = 0;i < 19;i++)
		{
			for (int j = 0;j < 19;j++)
			{
				this->White[i][j] = this->_rand();
				this->Black[i][j] = this->_rand();
			}
		}
	}
	static int _rand()
	{
		//return Math.floor(Math.random() * 1000000000);  //�ٶ�һλ������ˡ�
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::mt19937 g1(seed);  // mt19937 is a standard mersenne_twister_engine  
		return g1();
	}
	int getGO(int x, int y, int role)//����õ��µľ����code,����Ŀǰ�����code����֪�Ļ�
	{
		//���㷨�ĵĻ��������˰�λ���������������ʣ�A^B^B = A
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

/*************************ȫ����*************************/

int Board[19][19];//�洢������Ϣ����Ԫ��ֵΪ BLACK, WHITE, EMPTY ֮һ
map<Point, Point> AB;//keyֵΪһ��Point��valueֵΪ��Point��Ӧ��alpha,betaֵ���ֱ�洢��Point.x,Point.y�У�
int virtualBoard[19][19];

Zobrist zzz;
list <ListNode>L1;


/*************************������������*************************/

//��������
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
//��֦���
void initRootAB(Point root);
void initAB(Point father, Point son);
void upDataAB(Point p, int value, int flag);
bool isCut(Point pt);

//���ü�֦��min���max��
int min_layer(int deep, const Point father, int f, int computerside);
int max_layer(int deep, const Point father, int f, int computerside);

//��ѡ�߼�,�����жϲ�����ʱ��ȡ�˷���
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


/*******************���Ľ�********************/
string type[7] = { "oxxxxx+","+xxxxxo","+xxxxx+","+xxxx+","+x+xxx+" ,"+xx+xx+","+xxx+x+" };
					

int judgeChessType(int fake_Board[19][19], int x, int y, int directionX, int directionY, int& end, int my)
{

	//my�����ҵĽ�ɫ����my������Է����ж϶Է��Ƿ�������ұ���Ҫ�µ����
	//�ж��������
	//0Ϊ��
	std::string a = "#";
	int num, marko = 0, markx = 0, mark3 = 0;
	int x1 = x;
	int y1 = y;
	bool flag = false;
	for (int i = 0; i < 38; i++)//ֱ���ж�һ����
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
			else if (fake_Board[x1][y1] == 1-my)
			{
				std::string b = "x";
				markx++;
				a = a + b;

			}
		}


		x1 = x1 + directionX;
		y1 = y1 + directionY;
	}


	for (int i = 0;i < 7;i++)
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
			end = begin-1;//ע�� ��һ���ַ��ǹ̶��ġ�#��
			return (i+1);
		}
	}
	return 0;
}
int getdir(int fake_Board[19][19], int& end,int& start_x,int& start_y,int &type1, int my)
{

	//�жϳ��ָ����͵ķ���end�����ʼλ��
	for (int i = 0; i < 19; i++)//����
	{

		//��
		type1=judgeChessType(fake_Board, i, 0, 0, 1, end, my);
		if (type1>0)
		{
			start_x = i;
			start_y = 0;
			return 1;
		}

		//��
		type1 = judgeChessType(fake_Board, 0, i, 1, 0, end, my);
		if (type1>0)
		{
			start_x = 0;
			start_y = i;
			return 2;
		}

		/*  / */
		type1 = judgeChessType(fake_Board, 0, i, 1, -1, end, my);
		if (type1>0)
		{
			start_x = 0;
			start_y = i;
			return 3;
		}

		/*  \ */
		type1 = judgeChessType(fake_Board, 0, i, 1, 1, end, my);
		if (type1>0)
		{
			start_x = 0;
			start_y = i;
			return 4;
		}

	}
	for (int i = 1; i < 19; i++)//���㣬����
	{

		/*  / */
		type1 = judgeChessType(fake_Board, i, 18, 1, -1, end, my);
		if (type1>0)
		{
			start_x = i;
			start_y = 18;
			return 3;
		}

		/*  \ */
		type1 = judgeChessType(fake_Board, i, 0, 1, 1, end, my);
		if (type1>0)
		{
			start_x = i;
			start_y = 0;
			return 4;
		}

	}
	return -1;

}
void mustDone(int fake_Board[19][19], Step myStep, int my)
{
	//ֱ�ӵ���getdir�ķ���ֵΪ�������Ż���б��ʲô�ģ�end�����Ǹ��ַ�����β��
	//+OOOO+���������end��ȡ�����һ��+��λ�ã��ұ������ⲽ����getdir�ķ���������ص��ҵ�һ��+λ�ã�����һ�����������
	//getdir(int fake_Board[19][19], int& end, int my)
	int end=0,start_X,start_Y,type;
	int num1=0, num2=0;
	int mark=getdir(fake_Board, end, start_X, start_Y, type1, my);
	if (mark == -1) return;
	//string type[7] = { "oxxxxx+","+xxxxxo","+xxxxx+","+xxxx+","+x+xxx+" ,"+xx+xx+","+xxx+x+" };
	switch (mark)
	{
	case 1:
	{
		num1 = 0;
		num2 = 1;
	};break;
	case 2:
	{
		num1 = 1;
		num2 = 0;
	};break;
	case 3:
	{
		num1 = 1;
		num2 = -1;
	};break;
	case 4:
	{
		num1 = 1;
		num2 = 1;
	};break;
	default:
		break;
	}

	switch (type1)
	{
	case 1:
	{
		myStep.first.x = start_X+(end-1)*num1;
		myStep.first.y = start_Y + (end - 1)*num2 ;
		myStep.second.x = -1;
		myStep.second.y = -1;
	};break;
	case 2:
	{
		myStep.first.x = start_X + (end - 7) * num1;
		myStep.first.y = start_Y + (end - 7)*num2;
		myStep.second.x = -1;
		myStep.second.y = -1;
		
	};break;
		case 3:
		{
			myStep.first.y = start_Y + (end - 7)*num2;
			myStep.second.x = start_X + (end - 7) * num1;
			myStep.first.x = start_X + (end - 1) * num1;
			myStep.first.y = start_Y + (end - 1) * num2;
		};break;
		case 4:
		{
			myStep.first.y = start_Y + (end - 6) * num2;
			myStep.second.x = start_X + (end - 6) * num1;
			myStep.first.x = start_X + (end - 1) * num1;
			myStep.first.y = start_Y + (end - 1) * num2;
		};break;
		case 5:
		{
			myStep.first.y = start_Y + (end - 5) * num2;
			myStep.second.x = start_X + (end - 5) * num1;
			myStep.first.x = start_X + (end - 1) * num1;
			myStep.first.y = start_Y + (end - 1) * num2;
		};break;
		case 6:
		{
			myStep.first.y = start_Y + (end - 4) * num2;
			myStep.second.x = start_X + (end - 4) * num1;
			myStep.first.x = start_X + (end - 1) * num1;
			myStep.first.y = start_Y + (end - 1) * num2;
		};break;
		case 7:
		{
			myStep.first.y = start_Y + (end - 3) * num2;
			myStep.second.x = start_X + (end - 3) * num1;
			myStep.first.x = start_X + (end - 7) * num1;
			myStep.first.y = start_Y + (end - 7) * num2;
		};break;
		default:
			break;
		}
		
	
	
}


/**********************************������������*********************************/
void change_num(int directionx, int directiony, int& num)
{
	if (directionx == 1 && directiony == 0)
	{
		num = 0;//����
	}
	else if (directionx == 0 && directiony == 1)
	{
		num = 1;//����
	}
	else if (directionx == 1 && directiony == -1)
	{
		num = 2;//����
	}
	else if (directionx == 1 && directiony == 1)
	{
		num = 3;//����
	}
	num = -1;
}

//�ҵ���ǰ��Ķ�Ӧ�� ���
void change_intoStart(int current_X, int current_Y, int& Start_Hx, int& start_Hy, int& Start_Vx, int& start_Vy, int& Start_Lx, int& start_Ly, int& Start_Rx, int& start_Ry)
{
	//�ҵ���ǰ��Ķ�Ӧ�� ���
	start_Hy = 0;//������� �����궼��0
	Start_Hx = current_X;//������� �����궼�ǵ�ǰ�ĺ�����

	start_Vy = current_Y;//������� �����궼�ǵ�ǰ�ĺ�����
	Start_Vx = 0;//������� �����궼��0

	//������
	Start_Lx = current_X;
	start_Ly = current_Y;
	while (Start_Lx > 0 || start_Ly < 18)
	{
		Start_Lx--;
		start_Ly++;
	}

	//������
	Start_Rx = current_X;
	start_Ry = current_Y;
	while (Start_Rx > 0 || start_Ry > 0)
	{
		Start_Rx--;
		start_Ry--;
	}


}

void chess_type(int fake_Board[19][19], int x, int y, int directionX, int directionY, int computerside, int& value)
{
	std::string a = "#";
	int num, marko = 0, markx = 0, mark3 = 0;
	int x1 = x;
	int y1 = y;
	bool flag = true;
	//              ������   ���壬                 ���壬        ���壬         ���ģ�          ���ģ�          ���ģ�          ������         ������            ������
	std::string f1 = "oooooo", f2 = "xooooo+", f3 = "+ooooox", f4 = "+ooooo+", f5 = "xoooo++", f6 = "++oooox", f7 = "++oooo++", f8 = "xooo+++", f9 = "+++ooox", f10 = "+++ooo+++";
	//              �߶�       �߶�               ���
	std::string f11 = "xoo++++", f12 = "++++oox", f13 = "++++oo++++";
	//������ 
	std::string f14 = "x++o+oo+x", f15 = "x++oo+o+x", f16 = "x+o+oo++x", f17 = "x+oo+o++x";
	//change_num(directionX, directionY, num);
	for (int i = 0;i < 38;i++)//ֱ���ж�һ����
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

		/*//�������� ���� ��num             ���߻�num
		if (marko >= 6 || (marko + mark3 >= 6 && markx > 0) || (marko + mark3 > 6 && (marko + mark3 - 6) % 2 == 0))
		{
			break;
		}*/
		x1 = x1 + directionX;
		y1 = y1 + directionY;
	}
	//�ж���������
	int count = 0;
	int begin = -1;
	while ((begin = a.find(f1, begin + 1)) != std::string::npos)//����
	{
		count++;
		begin = begin + f1.length();
	}
	value += count * 10000;
	count = 0;
	begin = -1;
	while ((begin = a.find(f4, begin + 1)) != std::string::npos)//����
	{
		count++;
		begin = begin + f1.length();
	}
	value += count * 1000;
	count = 0;
	begin = -1;
	while ((begin = a.find(f2, begin + 1)) != std::string::npos)//����
	{
		count++;
		begin = begin + f2.length();
	}
	value += count * 500;
	count = 0;
	begin = -1;
	while ((begin = a.find(f3, begin + 1)) != std::string::npos)//����
	{
		count++;
		begin = begin + f3.length();
	}
	value += count * 500;
	count = 0;
	begin = -1;
	while ((begin = a.find(f7, begin + 1)) != std::string::npos)//����
	{
		count++;
		begin = begin + f7.length();
	}
	value += count * 900;
	count = 0;
	begin = -1;
	while ((begin = a.find(f5, begin + 1)) != std::string::npos)//����
	{
		count++;
		begin = begin + f5.length();
	}
	value += count * 460;
	count = 0;
	begin = -1;
	while ((begin = a.find(f6, begin + 1)) != std::string::npos)//����
	{
		count++;
		begin = begin + f6.length();
	}
	value += count * 460;
	count = 0;
	begin = -1;
	while ((begin = a.find(f10, begin + 1)) != std::string::npos)//����
	{
		count++;
		begin = begin + f10.length();
	}
	value += count * 350;
	//������
	count = 0;
	begin = -1;
	while ((begin = a.find(f14, begin + 1)) != std::string::npos)//����
	{
		count++;
		begin = begin + f14.length();
	}
	value += count * 180;
	count = 0;
	begin = -1;
	while ((begin = a.find(f15, begin + 1)) != std::string::npos)//����
	{
		count++;
		begin = begin + f15.length();
	}
	value += count * 180;
	count = 0;
	begin = -1;
	while ((begin = a.find(f16, begin + 1)) != std::string::npos)//����
	{
		count++;
		begin = begin + f16.length();
	}
	value += count * 180;
	count = 0;
	begin = -1;
	while ((begin = a.find(f17, begin + 1)) != std::string::npos)//����
	{
		count++;
		begin = begin + f17.length();
	}
	value += count * 180;

	count = 0;
	begin = -1;
	while ((begin = a.find(f8, begin + 1)) != std::string::npos)//����
	{
		count++;
		begin = begin + f8.length();
	}
	value += count * 150;
	count = 0;
	begin = -1;
	while ((begin = a.find(f9, begin + 1)) != std::string::npos)//����
	{
		count++;
		begin = begin + f9.length();
	}
	value += count * 150;
	count = 0;
	begin = -1;
	while ((begin = a.find(f13, begin + 1)) != std::string::npos)//���
	{
		count++;
		begin = begin + f13.length();
	}
	value += count * 150;
	count = 0;
	begin = -1;
	while ((begin = a.find(f11, begin + 1)) != std::string::npos)//�߶�
	{
		count++;
		begin = begin + f11.length();
	}
	value += count * 10;
	count = 0;
	begin = -1;
	while ((begin = a.find(f12, begin + 1)) != std::string::npos)//�߶�
	{
		count++;
		begin = begin + f12.length();
	}
	value += count * 10;

}


//2020_4_13:valueû�г�ʼ��(���޸�)
//���羲̬���õĻ�������step.first.x=-1;step.first.y=-1;step.second.x=-1;step.second.y=-1��
void value_situation(int fake_Board[19][19], int computerside, Step step, int& value, int score_Hori[19], int score_verti[19], int score_left[38], int score_right[38])
{
	value = 0;
	int num1 = 0, num2 = 0;
	//int score_Hori[19], score_verti[19], score_left[38], score_right[38];//���ţ�����,���㣬����
	int fake_Board1[19][19];
	for (int i = 0;i < 19;i++)
	{
		for (int j = 0;j < 19;j++)
		{
			fake_Board1[i][j] = fake_Board[i][j];
		}
	}
	if (step.first.x >= 0 && step.first.x >= 0 && step.first.y >= 0 && step.second.y >= 0 && step.first.x < 19 && step.second.x < 19 && step.first.y < 19 && step.second.y < 19)
	{
		fake_Board1[step.first.x][step.first.y] = computerside;
		fake_Board1[step.second.x][step.second.y] = computerside;
	}
	for (int i = 0;i < 19;i++)//�������㶨
	{
		score_Hori[i] = 0;
		chess_type(fake_Board1, i, 0, 0, 1, computerside, score_Hori[i]);//��

		value += score_Hori[i];
		score_verti[i] = 0;
		chess_type(fake_Board1, 0, i, 1, 0, computerside, score_verti[i]);//��
		value += score_verti[i];

		score_left[num1] = 0;
		chess_type(fake_Board1, 0, i, 1, -1, computerside, score_left[num1]);//��

		value += score_left[num1];
		score_right[num2] = 0;
		chess_type(fake_Board1, 0, i, 1, 1, computerside, score_right[num2]);//��

		value += score_right[num2];
		num1++;
		num2++;
	}
	for (int i = 1;i < 19;i++)//���㣬����
	{
		score_left[num1] = 0;
		chess_type(fake_Board1, i, 18, 1, -1, computerside, score_left[num1]);//��

		value += score_left[num1];
		score_right[num2] = 0;
		chess_type(fake_Board1, i, 0, 1, 1, computerside, score_right[num2]);//��

		value += score_right[num2];
		num1++;
		num2++;
	}
}
//2020_4_13:С����current_Y = step.first.y; Yд����X
//Zobrist �㷨
//�Ѿ��е�һЩ�������List����
int getZobrist(int fake_Board[19][19], Step step, int computerside, int score_Hori[19], int score_verti[19], int score_left[38], int score_right[38], int old_value)
{
	int mark = 0, value = 0;
	bool flag = false;

	mark = zzz.getGO(step.first.x, step.first.y, computerside);//��һ��
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
	if (flag != true)//û�ҵ���һ��֮��ľ���
	{
		value_situation(fake_Board, computerside, step, value, score_Hori, score_verti, score_left, score_right);
		ListNode a(mark, value);
		L1.push_front(a);
	}*/
	mark = zzz.getGO(step.second.x, step.second.y, computerside);//�ڶ���
	std::list<ListNode>::iterator it1;
	for (it1 = L1.begin();it1 != L1.end();++it1)
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
		//�����о����������壬�õ��·���
		current_X = step.first.x;
		current_Y = step.first.y;
		change_intoStart(current_X, current_Y, Start_Hx, start_Hy, Start_Vx, start_Vy, Start_Lx, start_Ly, Start_Rx, start_Ry);
		chess_type(fake_Board, Start_Hx, start_Hy, 0, 1, computerside, valueH1);
		chess_type(fake_Board, Start_Vx, start_Vy, 1, 0, computerside, valueV1);
		chess_type(fake_Board, Start_Lx, start_Ly, 1, -1, computerside, valueL1);
		chess_type(fake_Board, Start_Hx, start_Hy, 0, 1, computerside, valueR1);
		cha += score_Hori[Start_Hx];cha += score_verti[Start_Vx];
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


		current_X = step.second.x;
		current_X = step.second.y;
		chess_type(fake_Board, Start_Hx, start_Hy, 0, 1, computerside, valueH2);
		chess_type(fake_Board, Start_Vx, start_Vy, 1, 0, computerside, valueV2);
		chess_type(fake_Board, Start_Lx, start_Ly, 1, -1, computerside, valueL2);
		chess_type(fake_Board, Start_Hx, start_Hy, 0, 1, computerside, valueR2);

		cha += score_Hori[Start_Hx];cha += score_verti[Start_Vx];
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
	//ȫ��������һ��
	value_situation(fake_Board, computerside, step, value, score_Hori, score_verti, score_left, score_right);//û�ҵ��Ļ�
	ListNode a(mark, value);
	L1.push_front(a);
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
roleΪ��ǰ��Ҫ�����Ľ�ɫ, computersideΪ����ִ����ɫ, boardΪ����
Ϊrole���ɿ������ӵ�λ�ã��洢λ����answer�����У�˳��Ϊ���������塢���ġ�˫�����Ļ������
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

��������board[19][19], �ж�λ��(x,y)������ɫrole�ķ���

*/

int scorePoint(int x, int y, int role, int board[19][19]) {

	myBoard b;

	int scorecache[2][19][19][4];

	int result = 0;

	int count = 0;

	int block = 0;

	int empty = 0;

	int secondCount = 0;  //��һ�������count

	//����Ϊ���� 

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



	//����Ϊ����  

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



	//����Ϊ�K��  

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

	//����Ϊ�L��  

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








//�ⲿ����timeΪ0
void gen(int role, int computerside, int board[19][19], vector<Point>& answer, int time) {

	/*
	roleΪ��ǰ��Ҫ�����Ľ�ɫ, computersideΪ����ִ����ɫ, boardΪ����
	Ϊrole���ɿ������ӵ�λ�ã��洢λ����answer�����У�˳��Ϊ���������塢���ġ�˫�����Ļ������
	*/


	myBoard p;
	Score S;



	//com��ͷ��������hum��ͷ����з�
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
			int cs = scorePoint(t[i].x, t[i].y, computerside, board);//�������� 

			int hs = scorePoint(t[i].x, t[i].y, 1 - computerside, board);//�Է����� 

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

			else if (p.comScore >= S.FIVE) {//�����ܲ�������5

				comfives.push_back(p);

			}

			else if (p.humScore >= S.FIVE) {//����ܲ�������5

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

	//����������Ǳ�ɱ�壬ֱ�ӷ���

	if (!comsix.empty())
	{

		changemyBoardToPoint(comsix, answer);

		return;

	}

	if (!humsix.empty()) {

		changemyBoardToPoint(humsix, answer);

		return;

	}

	//�Է��л�����߳���,�Լ�������嶼û�У��ȿ��ǶԷ�
	if (role == computerside) {
		if (!humfives.empty())
		{
			if (humfives.size() == 1 && time == 0) {

				for (int i = 0;i < 19;i++)
					for (int j = 0;j < 19;j++) {
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
				for (int i = 0;i < 19;i++)
					for (int j = 0;j < 19;j++) {
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


	// �����л��ģ��Լ����Ķ�û�����Ƕ������ 

	if (role == computerside) {
		if (!humfours.empty())

		{
			if (humfours.size() == 1 && time == 0) {

				for (int i = 0;i < 19;i++)
					for (int j = 0;j < 19;j++) {
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

				for (int i = 0;i < 19;i++)
					for (int j = 0;j < 19;j++) {
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

			for (int i = 0;i < 19;i++)
				for (int j = 0;j < 19;j++) {
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

			for (int i = 0;i < 19;i++)
				for (int j = 0;j < 19;j++) {
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




	// �Լ��ܻ��ģ���ֱ�ӻ��ģ������ǳ���

	if (role == computerside && !comfours.empty()) {
		if (comfours.size() == 1 && time == 0) {

			for (int i = 0;i < 19;i++)
				for (int j = 0;j < 19;j++) {
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
			cout << "...................h" << endl;
			for (int m = 0;m < 19;m++)
				for (int n = 0;n < 19;n++)
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

			for (int i = 0;i < 19;i++)
				for (int j = 0;j < 19;j++) {
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

			for (int i = 0;i < 19;i++)
				for (int j = 0;j < 19;j++) {
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

			for (int i = 0;i < 19;i++)
				for (int j = 0;j < 19;j++) {
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



	//˫�������⣬��Ϊ���γ�˫���Ĳ�һ����һ������ǿ

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

/**********************************������������*********************************/
/**********************���õĺ���************************/
/*
��������fake_Board���ж���������roleһ���ĵ÷�
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
�жϸ�����fake_Board�У���(x,y)Ϊ�������Ƿ�������������ͬɫ����
*/
bool hasSix(int fake_Board[19][19], int x, int y, int directionX, int directionY) {

	std::string a = "#";
	int num, marko = 0, markx = 0, mark3 = 0;
	int x1 = x;
	int y1 = y;
	//              ������   
	std::string f1 = "oooooo", f2 = "xxxxxx";


	for (int i = 0; i < 19; i++)//ֱ���ж�һ����
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

	//�ж��Ƿ������6��
	int count = 0;
	int begin = -1;
	while ((begin = a.find(f1, begin + 1)) != std::string::npos)
	{
		count++;
		begin = begin + f1.length();
	}
	if (count > 0)
		return true;

	//�ж��Ƿ������6��
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
�ֱ��ж�19�����ߣ�19�����ߣ�38��\��38��/���Ƿ���������ͬɫ������
*/
bool isOver(int fake_Board[19][19]) {

	for (int i = 0; i < 19; i++)//����
	{

		//��
		if (hasSix(fake_Board, 0, i, 1, 0)) return true;

		//��
		if (hasSix(fake_Board, i, 0, 0, 1)) return true;

		/*  / */
		if (hasSix(fake_Board, 0, i, 1, -1)) return true;

		/*  \ */
		if (hasSix(fake_Board, 0, i, 1, 1)) return true;

	}
	for (int i = 1; i < 19; i++)//���㣬����
	{

		/*  / */
		if (hasSix(fake_Board, i, 18, 1, -1)) return true;

		/*  \ */
		if (hasSix(fake_Board, i, 0, 1, 1)) return true;

	}
	return false;

}

/********************ʵ�ֵĺ����Ĵ���**********************/
void copyBoard(const int board[19][19], int virtualboard[19][19])
{
	//������������
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			virtualboard[i][j] = board[i][j];
}

bool inside(int x, int y)
{
	//�ж��Ƿ��ڱ߽���
	return x >= 0 && x < n && y >= 0 && y < m;
}
//modify:�����˷�Χ�Ĳ������ɹ������޸�
bool isAround(int virtualBoard[19][19], int x, int y)
{
	//�ж��Ƿ���Χ������
	int range = 2;
	for (int i = -range; i <= range; i++)
	{
		for (int j = -range; j <= range; j++)
		{
			if (i != 0 && j != 0)
			{
				int tx = x + i;
				int ty = y + j;
				if (inside(tx, ty) && virtualBoard[tx][ty] != EMPTY)
					return true;
			}
		}
	}
	return false;
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
void getEmptyPoint(int virtualBoard[19][19], vector<Point>& t)
{
	//��ȡ���������п�λ��
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
	//��Ҫ���ø���ʱ�����
	Point t;
	t.x = MIN;
	t.y = MAX;
	AB[root] = t;
}
void initAB(Point father, Point son)
{
	//��ʼ��AB
	//����ѭ�����õ�min/max��ǰһ��
	Point t;
	t.x = AB[father].x;
	t.y = AB[father].y;
	AB[son] = t;

}
void upDataAB(Point p, int value, int flag)
{
	//����AB
	//���ڻ�õ�tmp�ĺ���
	//flag=0����min�����
	//flag=1����max�����
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
	//�ڸ��º�����ж�
	return AB[pt].x >= AB[pt].y;
	//��best����ֵ֮���˳�
}

/*
�����޸ģ�������computerside����
*/

int min_layer(int deep, const Point father, int f, int computerside)
{
	//�Է�����
	//deep������ȣ�farther����֮ǰ�ĵ㣬fΪ�õ��ʶ��ֵΪ0/1������/�ף�
	int score = evalue(virtualBoard, f);
	if (isOver(virtualBoard) || deep <= 0)
	{
		return score;
	}
	vector<Point> t;
	gen(f, computerside, virtualBoard, t, 0);
	int re = MAX, temp;
	if (t.size() == 0 || t.size() == 1)//�����������
		return MAX;
	for (int i = 0; i < t.size(); i++)
	{
		initAB(father, t[i]);//��ʼ��ABֵ
		virtualBoard[t[i].x][t[i].y] = f;//����

		vector<Point> t2;
		gen(f, computerside, virtualBoard, t2, 0);
		for (int j = 0; j < t2.size(); j++)
		{
			initAB(father, t2[j]);//��ʼ��ABֵ
			virtualBoard[t2[j].x][t2[j].y] = f;
			//�����㿴��һ��������
			int temp1 = max_layer(deep - 1, t[i], !f, computerside);
			int temp2 = max_layer(deep - 1, t[i], !f, computerside);
			temp = temp1 + temp2;
			upDataAB(t[i], temp, f);//����ABֵ
			upDataAB(t2[j], temp, f);//����ABֵ
			//���ַ�ʽ���º���ߵ�ABֵ��ͬ�ж�һ����֦����
			if (temp < re) re = temp;
			if (isCut(t[i]))
				break;
			virtualBoard[t2[j].x][t2[j].y] = EMPTY;
		}
		virtualBoard[t[i].x][t[i].y] = EMPTY;//��ԭ
	}
	return re;
}
int max_layer(int deep, const Point father, int f, int computerside)
{
	//��������
	//deep������ȣ�father����֮ǰ�ĵ㣬fΪ�õ��ʶ
	int score = evalue(virtualBoard, f);
	if (isOver(virtualBoard) || deep <= 0)//ȱ���жϽ���
	{
		return score;
	}
	vector<Point> t;
	gen(f, computerside, virtualBoard, t, 1);
	int re = MIN, temp;
	if (t.size() == 0 || t.size() == 1)//�����������
		return MIN;
	for (int i = 0; i < t.size(); i++)
	{
		initAB(father, t[i]);//��ʼ��ABֵ
		virtualBoard[t[i].x][t[i].y] = f;//����

		vector<Point> t2;
		gen(f, computerside, virtualBoard, t2, 0);
		for (int j = 0; j < t2.size(); j++)
		{
			initAB(father, t2[j]);//��ʼ��ABֵ
			virtualBoard[t2[j].x][t2[j].y] = f;
			//�����㿴��һ��������
			int temp1 = min_layer(deep - 1, t[i], !f, computerside);
			int temp2 = min_layer(deep - 1, t[i], !f, computerside);
			temp = temp1 + temp2;
			upDataAB(t[i], temp, f);//����ABֵ
			upDataAB(t2[j], temp, f);//����ABֵ
			//���ַ�ʽ���º���ߵ�ABֵ��ͬ�ж�һ����֦����
			if (temp > re) re = temp;
			if (isCut(t[i]))
				break;
			virtualBoard[t2[j].x][t2[j].y] = EMPTY;
		}
		virtualBoard[t[i].x][t[i].y] = EMPTY;//��ԭ
	}

	return re;
}

/*
���ߺ������������ߵ�����������
computersideΪ����ִ����ɫ�����վ��ߵĽ��������myStep��
*/
void otherDecide(int computerside, Step& myStep)
{
	int best = MIN, i, j;
	int max_i = 0, max_j = 1;
	int vBoard[19][19];
	vector<Point> v;
	getEmptyPoint(vBoard, v);
	copyBoard(Board, vBoard);
	//�������
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
	for (int i = 0;i < v.size() - 1;i++)
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

void chooseBest(vector<Point> vbest, Step& myStep, int computerside)
{
	int re = 0, index = 0;
	int fakeBoard[19][19];
	copyBoard(Board, fakeBoard);
	for (int i = 0;i < vbest.size() - 1;i += 2)
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
	vector<Point> v;

	copyBoard(Board, virtualBoard);
	//getVaildPoint(virtualBoard,v);
	//����
	gen(computerside, computerside, virtualBoard, v, 0);

	int best = MIN;
	int deep = 2;//û�м�֦������£�������4�㿴һ�¼���ʱ��
	vector<Point> vbest;

	int len = v.size();
	/*if (len <= 1)
	{
		otherDecide(computerside, myStep);
		return;
	}*/

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
			int tmp1 = min_layer(deep - 1, v[i], !computerside, computerside);
			int tmp2 = min_layer(deep - 1, v[i], !computerside, computerside);
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
		}
		virtualBoard[v[i].x][v[i].y] = EMPTY;
	}
	//��֤vbest��size���ڵ���2
	chooseBest(vbest, myStep, computerside);

}

int main()
{
	Step step;//��ʱ���ṹ
	char message[256];//ͨ����Ϣ����
	int computerSide;//����ִ����ɫ
	int start = 0;//�Ծֿ�ʼ���
	srand(int(time(0)));
	//�˴����ó�ʼ������
	//...

	while (1)	//������ѭ��
	{
		fflush(stdout);//��Ҫɾ������䣬�������������
		scanf("%s", message);//��ȡƽ̨������Ϣ
		//��������
		if (strcmp(message, "name?") == 0)//���սƽ̨���Ͷ���
		{
			fflush(stdin);
			/***********��"�����"��Ϊ��Ķ�����������6�����ֻ�12��Ӣ����ĸ�������޳ɼ�************/
			/*******/		printf("name ��ɶɶ����\n");		/**ֻ�޸�����壬��Ҫɾ��name�ո�****/
			/***********��"�����"��Ϊ��Ķ�����������6�����ֻ�12��Ӣ����ĸ�������޳ɼ�************/
		}
		else if (strcmp(message, "new") == 0)//���������
		{
			int i, j;
			scanf("%s", message);//��ȡ����ִ����ɫ
			fflush(stdin);
			if (strcmp(message, "black") == 0)	computerSide = BLACK;  //ִ��
			else  computerSide = WHITE;   //ִ��

			for (i = 0; i < 19; ++i)   //��ʼ�����
				for (j = 0; j < 19; ++j)
					Board[i][j] = EMPTY;
			start = 1;

			if (computerSide == BLACK)
			{
				/**********���ɵ�һ���ŷ�����������step�ṹ�У���������Ϊ(step.first.x,step.first.y)**********/
				/****************************���·������룬���滻�ҵ�ʾ������******************************/


				step.first.x = 9;
				step.first.y = 9;


				/******************************����������һ���������*******************************************/

				Board[step.first.x][step.first.y] = computerSide;//����������
				printf("move %c%c@@\n", step.first.x + 'A', step.first.y + 'A');//����ŷ�
			}
		}
		else if (strcmp(message, "move") == 0)//����,���������
		{
			scanf("%s", message);//��ȡ���������ŷ�
			fflush(stdin);
			step.first.x = message[0] - 'A';		step.first.y = message[1] - 'A';
			step.second.x = message[2] - 'A';		step.second.y = message[3] - 'A';
			//�����������
			Board[step.first.x][step.first.y] = 1 - computerSide;
			if (!(step.second.x == -1 && step.second.y == -1)) Board[step.second.x][step.second.y] = 1 - computerSide;

			/**********************************************************************************************************/
			/***�������ӵ����꣬������step�ṹ�У���1������(step.first.x,step.first.y)����2������(step.first.x,step.first.y)*****/
			/**************************************���·������룬���滻�ҵ�ʾ������*****************************************/
			Step myStep;
			decide(computerSide, myStep);
			step.first.x = myStep.first.x;
			step.first.y = myStep.first.y;
			Board[step.first.x][step.first.y] = computerSide;

			step.second.x = myStep.second.x;
			step.second.y = myStep.second.y;
			Board[step.second.x][step.second.y] = computerSide;

			/*****************************************������������******************************************************/
			/**********************************************************************************************************/

			printf("move %c%c%c%c\n", step.first.x + 'A', step.first.y + 'A', step.second.x + 'A', step.second.y + 'A');//����ŷ�
		}
		else if (strcmp(message, "error") == 0)//�ŷ�����
		{
			fflush(stdin);
		}
		else if (strcmp(message, "end") == 0)//�Ծֽ���
		{
			fflush(stdin);
			start = 0;
		}
		else if (strcmp(message, "quit") == 0)//�˳�����
		{
			fflush(stdin);
			printf("Quit!\n");
			break;
		}
	}
	return 0;
}