#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<time.h>


#include <string>
#include <random>
#include <chrono>  
#include<List>
#define BLACK 0
#define WHITE 1
#define EMPTY 2

struct Point { //点结构
	int x, y;
};
struct Step { //步结构
	Point first, second;
	int value;
};
int Board[19][19];//存储棋盘信息，其元素值为 BLACK, WHITE, EMPTY 之一


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
		//return Math.floor(Math.random() * 1000000000);  //再多一位就溢出了。
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::mt19937 g1(seed);  // mt19937 is a standard mersenne_twister_engine  
		return g1();
	}
	int getGO (int x,int y,int role)//计算得到新的局面的code,假设目前局面的code是已知的话
	{
		//该算法的的基础利用了按位异或运算的如下性质：A^B^B = A
		this->code ^= (role == 1 ? this->White[x][y] : this->Black[x][y]);
		return (this->code);
	}
	
};
Zobrist zzz;


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
std::list <ListNode>L1;

/*void change_num(int directionx, int directiony, int& num)
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
}*/
//找到当前点的对应的 起点
void change_intoStart(int current_X,int current_Y,int &Start_Hx,int &start_Hy, int& Start_Vx, int& start_Vy, int& Start_Lx, int& start_Ly, int& Start_Rx, int& start_Ry )
{
	//找到当前点的对应的 起点
	start_Hy = 0;//横线起点 纵坐标都是0
	Start_Hx = current_X;//横线起点 横坐标都是当前的横坐标

	start_Vy = current_Y;//竖线起点 纵坐标都是当前的横坐标
	Start_Vx = 0;//竖线起点 横坐标都是0

	//左倾线
	Start_Lx = current_X;
	start_Ly = current_Y;
	while (Start_Lx > 0 || start_Ly <18)
	{
		Start_Lx--;
		start_Ly++;
	}

	//右倾线
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
	//              活六，   眠五，                 眠五，        活五，         眠四，          眠四，          活四，          眠三，         眠三，            活三，
	std::string f1 = "oooooo", f2 = "xooooo+", f3 = "+ooooox", f4 = "+ooooo+", f5 = "xoooo++", f6 = "++oooox", f7 = "++oooo++", f8 = "xooo+++", f9 = "+++ooox", f10 = "+++ooo+++";
	//              眠二       眠二               活二
	std::string f11 = "xoo++++", f12 = "++++oox", f13 = "++++oo++++";
	//朦胧三 

	//change_num(directionX, directionY, num);
	for (int i = 0;i < 19;i++)//直接判断一条线
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


//假如静态调用的话，设置step.first.x=-1;step.first.y=-1;step.second.x=-1;step.second.y=-1。
void value_situation(int fake_Board[19][19], int computerside, Step step, int& value, int score_Hori[19], int score_verti[19],int score_left[38],int score_right[38])
{
	int value1 = 0;
	int num1 = 0, num2 = 0;
	//int score_Hori[19], score_verti[19], score_left[38], score_right[38];//横着，竖着,左倾，右倾
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
	for (int i = 0;i < 19;i++)//横竖，搞定
	{
		score_Hori[i] = 0;
		chess_type(fake_Board1, 0, i, 1, 0, computerside, score_Hori[i]);//横

		value += score_Hori[i];
		score_verti[i] = 0;
		chess_type(fake_Board1, i, 0, 0, 1, computerside, score_verti[i]);//竖
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
	for (int i = 1;i < 19;i++)//左倾，右倾
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
//Zobrist 算法
//已经有的一些局面存在List里面
int getZobrist(int fake_Board[19][19],Step step,int computerside, int score_Hori[19], int score_verti[19], int score_left[38], int score_right[38],int old_value)
{
	int mark=0,value=0;
	bool flag = false;
	
	mark = zzz.getGO(step.first.x,step.first.y, computerside);//第一步
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
		//在已有局面下两步棋，得到新分数
		current_X = step.first.x;
		current_X = step.first.y;
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
	//全局重新算一次
	value_situation(fake_Board, computerside, step, value, score_Hori, score_verti, score_left, score_right);//没找到的话
	ListNode a(mark, value);
	L1.push_front(a);
	return value;
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
			/*******/		printf("name 令狐冲\n");		/**只修改令狐冲，不要删除name空格****/
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

			//生成第1子落子位置step.first.x和step.first.y
			int x, y;
			x = rand() % 19; y = rand() % 19;
			while (Board[x][y] != EMPTY)
			{
				x = rand() % 19;	y = rand() % 19;
			}
			step.first.x = x;
			step.first.y = y;
			Board[step.first.x][step.first.y] = computerSide;

			//生成第2子落子位置step.second.x和step.second.y	
			x = rand() % 19;	y = rand() % 19;
			while (Board[x][y] != EMPTY)
			{
				x = rand() % 19;	y = rand() % 19;
			}
			step.second.x = x;
			step.second.y = y;
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
