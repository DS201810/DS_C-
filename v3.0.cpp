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

/*

��maxmin.h�б���ʱû������Point�������Ҳ�����������룬����һ��֮����ᱨ��û�ж������Ϊ(Point,Point)��<��

���ϵĽ�����map�ĵײ��Ǻ����ʵ�ֵģ���map�����Զ���������Ҫ����<�����

*/



//modify

//���������x�жϣ���xֵ��ͬyֵ��ͬ�ĵ㣬�ᱻ��ΪΪ��ͬ�ĵ㣬��map�б�����

bool operator<(const Point a, const Point b) {

	bool flag = (a.x < b.x);

	if (a.x == b.x)

		flag = (a.y < b.y);

	return flag;

}



struct Step { //���ṹ

	Point first, second;

	int value;

};

int Board[19][19];//�洢������Ϣ����Ԫ��ֵΪ BLACK, WHITE, EMPTY ֮һ



/*

myBoard�ࣺ��������(x,y),���ýڵ��Ӧ��˫������

��ԭ����������Board����������ʦ��Board[19][19]�����������Board bʱ���������޷��ж����Board���໹�����飬�ʻᱨ��

*/

class myBoard {

public:

	int comScore;

	int humScore;

	int x;

	int y;

};

struct Score {

	int ONE = 10; //��һ 

	int BLOCKED_ONE = 1;//��һ 



	int TWO = 100;//��� 

	int MIST_TWO = 50;//���ʶ�  

	int BLOCKED_TWO = 10;//�߶� 



	int THREE = 1000;

	int MIST_THREE = 500;

	int BLOCKED_THREE = 100;



	int FOUR = 100000;

	int BLOCKED_FOUR = 10000;



	int FIVE = 1000000;

	int BLOCKED_FIVE = 100000;



	int SIX = 10000000;



};

map<Point, Point> AB;//keyֵΪһ��Point��valueֵΪ��Point��Ӧ��alpha,betaֵ���ֱ�洢��Point.x,Point.y�У�

int virtualBoard[19][19];







/*************************������������*************************/



//��������

void copyBoard(const int Board[19][19], int virtualBoard[19][19]);

bool inside(int x, int y);

bool isAround(int x, int y);

//modify:�����˲���

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



//��ѡ�߼�

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



/*

value_situation��Ҫʹ�õĸ�������

*/

void chess_type(int fake_Board[19][19], int x, int y, int directionX, int directionY, int computerside, int& value)

{

	std::string a = "#";

	int num, marko = 0, markx = 0, mark3 = 0;

	int x1 = x;

	int y1 = y;

	bool flag = true;

	//              ������   ���壬                 ���壬        ���壬         ���ģ�          ���ģ�          ���ģ�          ������         ������            ������

	std::string f1 = "oooooo", f2 = "xooooo+", f3 = "+ooooox", f4 = "+ooooo+", f5 = "xoooo++",
		f6 = "++oooox", f7 = "++oooo++", f8 = "xooo+++", f9 = "+++ooox", f10 = "+++ooo+++";

	//              �߶�       �߶�               ���

	std::string f11 = "xoo++++", f12 = "++++oox", f13 = "++++oo++++";

	//������ 



	change_num(directionX, directionY, num);

	for (int i = 0; i < 19; i++)//ֱ���ж�һ����

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

	//�ж���������

	int count = 0;

	int begin = -1;

	while ((begin = a.find(f1, begin + 1)) != std::string::npos)//����

	{

		count++;

		begin = begin + f1.length();

	}

	value += count * MAX;

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



/*

�����computerside����ʦ�ĳ�����computerSide���岻ͬ�����Խ���������Ϊrole

�˺���������Ϊ��Ϊcomputersideһ����������fake_Board�����µĵ÷֣��������value��

*/



//modify: value��ʼ��Ϊ0

void value_situation(int fake_Board[19][19], int computerside, int& value)

{

	value = 0;

	int new_Board[19][19][4], num1 = 0, num2 = 0;

	int score_Hori[19], score_verti[19], score_left[38], score_right[38];//���ţ�����,���㣬����

	for (int i = 0; i < 19; i++)//�������㶨

	{

		score_Hori[i] = 0;

		chess_type(fake_Board, 0, i, 1, 0, computerside, score_Hori[i]);//��



		value += score_Hori[i];

		score_verti[i] = 0;

		chess_type(fake_Board, i, 0, 0, 1, computerside, score_verti[i]);//��

		value += score_verti[i];



		score_left[num1] = 0;

		chess_type(fake_Board, 0, i, 1, -1, computerside, score_left[num1]);//��



		value += score_left[num1];

		score_right[num2] = 0;

		chess_type(fake_Board, 0, i, 1, 1, computerside, score_right[num2]);//��



		value += score_right[num2];

		num1++;

		num2++;

	}

	for (int i = 1; i < 19; i++)//���㣬����

	{

		score_left[num1] = 0;

		chess_type(fake_Board, 0, i, 1, -1, computerside, score_left[num1]);//��



		value += score_left[num1];

		score_right[num2] = 0;

		chess_type(fake_Board, 0, i, 1, 1, computerside, score_right[num2]);//��



		value += score_right[num2];

		num1++;

		num2++;

	}

}



/*

count��ʾͬɫ�����ӵĸ�����block��ʾ��ͷ�����˼�ͷ��empty��ʾ�ո���ֵ�λ��

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

		//��1���ǿ�λ

		if (count >= 7) {

			return score.SIX;

		}

		if (block == 0) {

			switch (count) {

			case 2: return score.TWO / 2;

			case 3: return score.MIST_TWO;

			case 4: return score.MIST_THREE;

			case 5: return score.FOUR;

			case 6: return score.FIVE;

			}

		}
		if (block == 2) {
			if (count < 5)
				return 0;

			switch (count) {
			case 5:return score.BLOCKED_FOUR;

			case 6:return score.BLOCKED_FIVE;
			}
		}

		if (block == 1) {

			switch (count) {

			case 2: return score.ONE;

			case 3: return score.MIST_TWO;

			case 4: return score.MIST_THREE;

			case 5: return score.BLOCKED_FOUR;

			case 6:return  score.BLOCKED_FIVE;

			}

		}

	}

	else if (empty == 2 || empty == count - 2) {

		//�ڶ����ǿ�λ

		if (count >= 7) {

			return score.FIVE;

		}

		if (block == 0) {

			switch (count) {

			case 3: return score.MIST_TWO;

			case 4: return score.TWO*2;

			case 5: return score.THREE;

			case 6: return score.FOUR;

			}

		}

		if (block == 1) {

			switch (count) {

			case 3: return score.BLOCKED_TWO;

			case 4: return score.TWO;

			case 5: return score.BLOCKED_THREE;

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

			case 5: return score.THREE+score.TWO;

			case 6: return score.THREE*2;

			case 7: return score.FOUR;

			case 8:return score.FIVE;

			}

		}

		if (block == 1) {

			switch (count) {

			case 4:return score.BLOCKED_THREE;

			case 5:return score.MIST_THREE;

			case 6: return score.THREE;

			case 7: return score.FOUR;

			case 8:return score.BLOCKED_FIVE;

			}

		}

		if (block == 2) {

			switch (count) {

			case 4: return 0;

			case 5:return score.BLOCKED_THREE;

			case 6:return score.BLOCKED_THREE*2;

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

				empty = 0;  //ע��������0����Ϊ�Ǵ��������ߵ�

				continue;

			}

			else

				break;

		}

		if (board[x][i] == role) {

			secondCount++;

			if (empty != -1)

			{

				empty++;  //ע������������ֶ��˼������ӣ���ôempty��λ�þͱ����

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



	//����Ϊ����  

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

				empty = 0;  //ע��������0����Ϊ�Ǵ��������ߵ�

				continue;

			}

			else  break;

		}

		if (board[i][y] == role) {

			secondCount++;

			if (empty != -1)

			{

				empty++;  //empty��λ�ñ�� 

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

				empty = 0;  //ע��������0����Ϊ�Ǵ��������ߵ�

				continue;

			}

			else  break;

		}

		if (board[m][n] == role) {

			secondCount++;

			if (empty != -1)

			{

				empty++; //ע������������ֶ��˼������ӣ���ôempty��λ�þͱ����

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

				empty = 0;  //ע��������0����Ϊ�Ǵ��������ߵ�

				continue;

			}

			else  break;

		}

		if (board[m][n] == role) {

			secondCount++;

			if (empty != -1)

			{

				empty++; //ע������������ֶ��˼������ӣ���ôempty��λ�þͱ����

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

ȥ��vector<myBoard> my�ķ�����Ϣ���������Ӧ������ֵ����������p������β��

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

roleΪ��ǰ��Ҫ�����Ľ�ɫ, computersideΪ����ִ����ɫ, boardΪ����

Ϊrole���ɿ������ӵ�λ�ã��洢λ����answer�����У�˳��Ϊ���������塢���ġ�˫�����Ļ������

*/



void gen(int role, int computerside, int board[19][19], vector<Point>& answer) {






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

	vector<myBoard> comtwos;

	vector<myBoard> humtwos;

	vector<myBoard> neighbors;





	int com[19][19];

	int hum[19][19];

	memset(com, 0, sizeof(com));

	memset(hum, 0, sizeof(hum));

	vector<Point> t;

	getVaildPoint(virtualBoard, t);



	for (int i = 0; i < t.size(); i++) {



		if (board[t[i].x][t[i].y] == 2) {

			//���������ھӵĲ���

			int cs = scorePoint(t[i].x, t[i].y, computerside, board);//�������� 

			int hs = scorePoint(t[i].x, t[i].y, 1 - computerside, board);//�Է����� 

			com[t[i].x][t[i].y] = cs;

			hum[t[i].x][t[i].y] = hs;

		}

		//�˴�ɾȥ�������Ӵ������ӵ�����



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

			else if (p.comScore >= S.FIVE) {//�����ܲ�������5

				comfives.push_back(p);

			}

			else if (p.humScore >= S.FIVE) {//����ܲ�������5

				humfives.push_back(p);

			}
			else if (p.comScore >= S.BLOCKED_FIVE)
			{
				comblockedfives.push_back(p);
			}
			else if (p.humScore >= S.BLOCKED_FIVE) {

				humblockedfives.push_back(p);
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

				//�ܳ�˫��Ҳ��

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

	//����������Ǳ�ɱ�壬ֱ�ӷ���

	if (!comsix.empty()) {

		changemyBoardToPoint(comsix, answer);

		return;

	}

	if (!humsix.empty()) {

		changemyBoardToPoint(humsix, answer);

		return;

	}

	//�Է��л�����߳���,�Լ�������嶼û�У��ȿ��ǶԷ�
	if (role == computerside && comfives.empty() && comblockedfives.empty()) {
		if(!humfives.empty())
		{changemyBoardToPoint(humfives, answer); }

	    else if (!humblockedfives.empty())
		changemyBoardToPoint(humblockedfives, answer);


		return;

	}
	if (role ==1-computerside  && humfives.empty() && humblockedfives.empty()) {
		if(!comfives.empty())
		changemyBoardToPoint(comfives, answer);
		else if(!comblockedfives.empty())
		changemyBoardToPoint(comblockedfives, answer);

		return;
	}

	if (role == computerside && !comfives.empty()) {

		changemyBoardToPoint(comfives, answer);

		return;

	}
	if (role == computerside && !comblockedfives.empty()) {
		changemyBoardToPoint(comblockedfives, answer);
	}



	if (role == 1 - computerside && !humfives.empty()) {

		changemyBoardToPoint(humfives, answer);

		return;

	}
	if (role = 1 - computerside && !humblockedfives.empty())
	{
		changemyBoardToPoint(humblockedfives, answer);

		return;

	}





	// �Լ��ܻ��ģ���ֱ�ӻ��ģ������ǳ���

	if (role == computerside && !comfours.empty()) {

		changemyBoardToPoint(comfours, answer);

		return;

	}

	if (role == 1 - computerside && !humfours.empty()) {

		changemyBoardToPoint(humfours, answer);

		return;

	}





	// �����л��ĳ��ģ��Լ����Ķ�û�����Ƕ������ 

	if (role == computerside && !humfours.empty()&&comfours.empty() && comblockedfours.empty()) {

		changemyBoardToPoint(humfours, answer);

		return;

	}

	if (role == 1 - computerside && !comfours.empty() &&humfours.empty()&&humblockedfours.empty()) {

		changemyBoardToPoint(comfours, answer);

		return;

	}





	// �����л����Լ��г��ģ��򶼿�����

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



	//˫�������⣬��Ϊ���γ�˫���Ĳ�һ����һ������ǿ

	if (!comtwothrees.empty() || !humtwothrees.empty()) {

		changemyBoardToPoint(result, answer);

		return;

	}




	vector<myBoard> twos;

	if (role == computerside)

	{

		twos.insert(twos.end(), comtwos.begin(), comtwos.end());

		twos.insert(twos.end(), humtwos.begin(), humtwos.end());
		changemyBoardToPoint(twos, answer);

	return;

	}



	else {

		twos.insert(twos.end(), humtwos.begin(), humtwos.end());

		twos.insert(twos.end(), comtwos.begin(), comtwos.end());
		changemyBoardToPoint(twos, answer);

	return;

	}

	
  changemyBoardToPoint(neighbors,answer);


}



/**********************************������������*********************************/

/**********************���õĺ���************************/

/*

��������fake_Board���ж���������roleһ���ĵ÷�

*/

int evalue(int fake_Board[19][19], int role) {

	int value = 0;

	value_situation(fake_Board, role, value);

	return value;

}



/*

�жϸ�����vector���Ƿ�������������ͬɫ���ӣ��˸������תΪһ��vector��

*/

bool hasSix(vector<int> t) {

	int len = t.size();

	int i = 0, count = 0;//i��t���½Ǳ꣬count��Ŀǰ����������

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

��19�����ߣ�19�����ߣ�27��\��27��/�ֱ�ת����һ��vector���ж��Ƿ�������

���ɷ����Ӳ���6���Ĺ�20���ߣ�����ת����Χ��

*/



//������

bool isOver(int fake_Board[19][19]) {

	return false;

	vector<int> t;

	t.clear();

	//����

	for (int i = 0; i < n; i++) {

		for (int j = 0; j < m; j++) {

			t.push_back(fake_Board[i][j]);

		}

		if (hasSix(t))

			return true;

		else

			t.clear();

	}

	//����

	for (int j = 0; j < m; j++) {

		for (int i = 0; i < n; i++) {

			t.push_back(fake_Board[i][j]);

		}

		if (hasSix(t))

			return true;

		else

			t.clear();

	}

	/*  �������е�\ */

	for (int i = 13; i >= 0; i--) {

		int j = 0;

		int dx = 1, dy = 1, currentx = i, currenty = j;

		int num = 19 - i;//��һ�����������num������

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

	/*  �������е�\ */

	for (int j = 2; j <= 13; j++) {

		int i = 0;

		int dx = 1, dy = 1, currentx = i, currenty = j;

		int num = 19 - j;//��һ�����������num������

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

	/*  �������е�/ */

	for (int i = 5; i < 19; i++) {

		int j = 0;

		int dx = -1, dy = 1, currentx = i, currenty = j;

		int num = i + 1;//��һ�����������num������

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

	/*  �������е�/ */

	for (int j = 2; j <= 13; j++) {

		int i = 18;

		int dx = -1, dy = 1, currentx = i, currenty = j;

		int num = 19 - j;//��һ�����������num������

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

			if (!(i == 0 && j == 0))

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



void getVaildPoint(int virtualBoard[19][19], vector<Point> &t)

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

	int score = evalue(virtualBoard, WHITE);

	if (isOver(virtualBoard) || deep <= 0)

	{

		return score;

	}

	vector<Point> t;

	gen(f, computerside, virtualBoard, t);

	int re = MAX, temp;

	if (t.size() == 0 || t.size() == 1)//�����������

		return MAX;

	for (int i = 0; i < t.size() - 1; i += 2)

	{

		initAB(father, t[i]);//��ʼ��ABֵ

		initAB(father, t[i + 1]);//��ʼ��ABֵ

		virtualBoard[t[i].x][t[i].y] = f;//����

		virtualBoard[t[i + 1].x][t[i + 1].y] = f;



		//Point confar;

		//confar.x = max(t[i].x, t[i + 1].x);

		//confar.y = min(t[i].y, t[i + 1].y);



		//�����㿴��һ��������

		int temp1 = max_layer(deep - 1, t[i], !f, computerside);

		int temp2 = max_layer(deep - 1, t[i], !f, computerside);

		temp = temp1 + temp2;

		upDataAB(t[i], temp, f);//����ABֵ

		upDataAB(t[i + 1], temp, f);//����ABֵ

		//���ַ�ʽ���º���ߵ�ABֵ��ͬ�ж�һ����֦����

		if (temp < re) re = temp;

		if (isCut(t[i]))

			break;

		virtualBoard[t[i].x][t[i].y] = 2;//��ԭ

		virtualBoard[t[i + 1].x][t[i + 1].y] = 2;

	}

	return re;

}

int max_layer(int deep, const Point father, int f, int computerside)

{

	//��������

	//deep������ȣ�father����֮ǰ�ĵ㣬fΪ�õ��ʶ

	int score = evalue(virtualBoard, BLACK);

	if (isOver(virtualBoard) || deep <= 0)//ȱ���жϽ���

	{

		return score;

	}

	vector<Point> t;

	gen(f, computerside, virtualBoard, t);

	int re = MIN, temp;

	if (t.size() == 0 || t.size() == 1)//�����������

		return MIN;

	for (int i = 0; i < t.size() - 1; i += 2)

	{

		initAB(father, t[i]);//��ʼ��ABֵ

		initAB(father, t[i + 1]);//��ʼ��ABֵ

		virtualBoard[t[i].x][t[i].y] = f;//����

		virtualBoard[t[i + 1].x][t[i + 1].y] = f;



		//Point confar;

		//confar.x = max(t[i].x, t[i + 1].x);

		//confar.y = min(t[i].y, t[i + 1].y);



		//�����㿴��һ��������

		int temp1 = min_layer(deep - 1, t[i], !f, computerside);

		int temp2 = min_layer(deep - 1, t[i], !f, computerside);

		temp = temp1 + temp2;

		upDataAB(t[i], temp, f);//����ABֵ

		upDataAB(t[i + 1], temp, f);//����ABֵ

		//���ַ�ʽ���º���ߵ�ABֵ��ͬ�ж�һ����֦����

		if (temp > re) re = temp;

		if (isCut(t[i]))

			break;

		virtualBoard[t[i].x][t[i].y] = 2;//��ԭ

		virtualBoard[t[i + 1].x][t[i + 1].y] = 2;

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

void decide(int computerside, Step& myStep) {

	vector<Point> v;



	copyBoard(Board, virtualBoard);

	getVaildPoint(virtualBoard, v);



	int best = MIN;

	int deep = 2;//û�м�֦������£�������4�㿴һ�¼���ʱ��

	vector<Point> vbest;



	int len = v.size();

	/*if (len <= 1)

	{

		otherDecide(computerside, myStep);

		return;

	}*/

	for (int i = 0; i < len - 1; i = i + 2) {

		AB.clear();

		initRootAB(v[i]);

		initRootAB(v[i + 1]);

		virtualBoard[v[i].x][v[i].y] = computerside;

		virtualBoard[v[i + 1].x][v[i + 1].y] = computerside;

		int tmp1 = min_layer(deep - 1, v[i], !computerside, computerside);

		int tmp2 = min_layer(deep - 1, v[i], !computerside, computerside);

		int tmp = tmp1 + tmp2;

		if (tmp == best) {

			vbest.push_back(v[i]);

			vbest.push_back(v[i + 1]);

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
