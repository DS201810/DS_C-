﻿#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<time.h>
#include <string>

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


void chess_type(int fake_Board[19][19], int x, int y, int directionX, int directionY, int computerside, int& value)
{
	std::string a = "#";
	int num,marko=0,markx=0,mark3=0;
	int x1 = x;
	int y1 = y;
	bool flag = true;
	//              活六，   眠五，                 眠五，        活五，         眠四，          眠四，          活四，          眠三，         眠三，            活三，
	std::string f1 = "oooooo", f2 = "xooooo+", f3 = "+ooooox", f4 = "+ooooo+", f5 = "xoooo++", f6 = "++oooox", f7 = "++oooo++", f8 = "xooo+++", f9 = "+++ooox", f10 = "+++ooo+++";
	//              眠二       眠二               活二
	std::string f11 = "xoo++++", f12 = "++++oox", f13 = "++++oo++++";
	//朦胧三 

	change_num(directionX, directionY, num);
	for (int i = 0;i < 19;i++)//直接判断一条线
	{

		if (x1 < 0 || y1 < 0||x1>=19||y1>=19)
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
	
	/*
	if (strstr(a.c_str(), f1.c_str()) != NULL)//活六
	{
		value = 10000;
	}
	else if (strstr(a.c_str(), f4.c_str()) != NULL)//活五
	{
		value = 1000;
	}
	else if (strstr(a.c_str(), f2.c_str()) != NULL || strstr(a.c_str(), f3.c_str()) != NULL)//眠五
	{
		value = 500;
	}
	else if (strstr(a.c_str(), f7.c_str()) != NULL)//活四
	{
		value = 900;
	}
	else if (strstr(a.c_str(), f5.c_str()) != NULL || strstr(a.c_str(), f6.c_str()) != NULL)//眠四
	{
		value = 460;
	}
	else if (strstr(a.c_str(), f10.c_str()) != NULL)//活三
	{
		value = 350;
	}
	else if (strstr(a.c_str(), f8.c_str()) != NULL || strstr(a.c_str(), f9.c_str()) != NULL)//眠三
	{
		value = 150;
	}
	else if (strstr(a.c_str(), f13.c_str()) != NULL)//活二
	{
		value = 150;
	}
	else if (strstr(a.c_str(), f12.c_str()) != NULL || strstr(a.c_str(), f11.c_str()) != NULL)//眠二
	{
		value = 10;
	}
	value = 0;
	*/
}



void value_situation(int fake_Board[19][19], int computerside, Step step, int& value)
{
	int value1 = 0;
	int new_Board[19][19][4], num1 = 0,num2=0;
	int score_Hori[19],score_verti[19],score_left[38],score_right[38];//横着，竖着,左倾，右倾
	for (int i = 0;i < 19;i++)//横竖，搞定
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
	for (int i = 1;i < 19;i++)//左倾，右倾
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
