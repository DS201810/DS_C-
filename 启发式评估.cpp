#include<iostream>
#include<vector>
#include<stdlib.h>

using namespace std;

class Board{
	public:
	int comScore;
	int humScore;
    int x;
    int y;
    
};


struct Score{
  int ONE=5; //��һ 
  int BLOCKED_ONE=1;//��һ 

  int TWO=150;//��� 
  int MIST_TWO=100;//���ʶ�  
  int BLOCKED_TWO=10;//�߶� 

  int THREE= 350;
  int MIST_THREE=180; 
 int BLOCKED_THREE=150;
 
  int FOUR=900;
  int BLOCKED_FOUR= 460;

  int FIVE=1000;
  int BLOCKED_FIVE:500; 
  
  int SIX=10000; 
  
};

bool  hasNeighbor (int x,int y,int distance,int count,int board[19][19]) {
//�ж��Ƿ����ھ� 
    int len = 19;

    int  startX = x-distance;

    int endX = x+distance;

    int startY = y-distance;

    int endY = y+distance;

    for(int i=startX;i<=endX;i++) {

      if(i<0||i>=len) continue;

      for(int j=startY;j<=endY;j++) {

        if(j<0||j>=len) continue;

        if(i==x && j==y) continue;

        if(board[i][j] != 2) {

          count --;

          if(count <= 0) return true;
        }
     }
  }

    return false;

  } 
  
int Toscore(int count,int block,int empty){
	Score score;
   

  if(empty <= 0) {

    if(count >= 6) return score.SIX;

    if(block == 0) {

      switch(count) {

        case 1: return score.ONE;

        case 2: return score.TWO;

        case 3: return score.THREE;

        case 4: return score.FOUR;
        
        case 5: return score.FIVE;
        

      }

    }



    if(block == 1) {

      switch(count) {

        case 1: return score.BLOCKED_ONE;

        case 2: return score.BLOCKED_TWO;

        case 3: return score.BLOCKED_THREE;

        case 4: return score.BLOCKED_FOUR;
        
        case 5: return score.BLOCKED_FIVE;

      }

    }



  } else if(empty == 1 || empty == count-1) {

    //��1���ǿ�λ

    if(count >= 6) {

      return score.SIX;

    }

    if(block == 0) {

      switch(count) {

        case 2: return score.TWO/2;

        case 3: return score.MIST_TWO;

        case 4: return score.MIST_THREE;

        case 5: return score.FOUR;
        

      }

    }



    if(block == 1) {

      switch(count) {

        case 2: return score.BLOCKED_TWO;

        case 3: return score.BLOCKED_THREE;

        case 4: return score.BLOCKED_FOUR;

        case 5: return score.BLOCKED_FIVE;

      }

    }

  } else if(empty == 2 || empty == count-2) {

    //�ڶ����ǿ�λ

    if(count >= 7) {

      return score.FIVE;

    }

    if(block ==0) {

      switch(count) {

        case 3: return score.MIST_TWO;

        case 4: return score.THREE;

        case 5: return score.BLOCKED_FOUR;

        case 6: return score.FOUR;

      }

    }



    if(block == 1) {

      switch(count) {

        case 3: return score.BLOCKED_THREE;

        case 4: return score.BLOCKED_FOUR;

        case 5: return score.BLOCKED_FOUR;

        case 6: return score.FOUR;

      }

    }



    if(block == 2) {

      switch(count) {

        case 4: return 0; 

        case 5:return 0;

        case 6: return score.BLOCKED_FOUR;

      }

    }

  } else if(empty ==3 || empty == count-3) {

    if(count >= 9) {

      return score.SIX;

    }

    if(block == 0) {

      switch(count) {

        case 4:return score.THREE;

        case 5: return score.THREE;

        case 6: return score.BLOCKED_FOUR;

        case 7: return score.FOUR;
        
        case 8:return score.FIVE;
      }

    }



    if(block == 1) {

      switch(count) {

        case 4:return score.BLOCKED_THREE;

        case 5:return score.THREE;

        case 6: return score.BLOCKED_FOUR;

        case 7: return score.FOUR;
        
        case 8:return score.BLOCKED_FIVE;

      }

    }



    if(block == 2) {

      switch(count) {

        case 4: return 0;

        case 5:return 0;

        case 6:return score.BLOCKED_THREE;

        case 7: return score.BLOCKED_FOUR;

      }

    }

  } else if(empty == 4 || empty == count-4) {

    if(count >= 10) {

      return score.SIX;

    }

    if(block == 0) {

      switch(count) {

        case 5:return score.FOUR;

        case 6:return score.FOUR; 

        case 7:return score.FOUR;

        case 8: return score.FOUR;
        
        case 9:return score.FIVE; 

      }

    }



    if(block == 1) {

      switch(count) {

        case 4:return score.BLOCKED_FOUR;

        case 5:return score.BLOCKED_FOUR;

        case 6:return score.BLOCKED_FOUR;

        case 7: return score.BLOCKED_FOUR;

        case 8: return score.FOUR;
        
        case 9: return score.BLOCKED_FIVE; 

      }

    }



    if(block == 2) {

      switch(count) {

        case 5:return score.BLOCKED_TWO;

        case 6:return score.BLOCKED_THREE;

        case 7:return score.BLOCKED_FOUR;

        case 8: return score.BLOCKED_FOUR;
        
        case 9:return score.BLOCKED_FIVE;
      }

    }

  } else if(empty == 5 || empty == count-5) {
    if(count>=11) return score.SIX;
    if(block==0){
    	switch(count){
    		case 6:return score.FIVE;
    		case 7:return score.FIVE;
    		case 8:return score.FIVE;
    		case 9:return score.FOUR*2;
    		case 10:return score.FIVE*2;
		}
	}
	if(block==1){
    	switch(count){
    		case 6:return score.BLOCKED_FIVE;
    		case 7:return score.BLOCKED_FIVE;
    		case 8:return score.BLOCKED_FIVE;
    		case 9:return score.BLOCKED_FOUR*2;
    		case 10:return score.FIVE*2;
		}
	}
	if(block==2){
			switch(count){
    		case 6:return score.BLOCKED_FOUR;
    		case 7:return score.BLOCKED_FOUR;
    		case 8:return score.BLOCKED_FOUR;
    		case 9:return score.BLOCKED_FOUR*2;
    		case 10:return score.FIVE*2;
		}
	}
	 

  } else if(empty == 6 || empty == count-6)
  return score.SIX; 



  return 0;
	
}

int scorePoint(int x,int y,int role,int board[19][19]){
	Board b;
	int scorecache[2][19][19][4];
	int result=0;
	int count = 0;
	int block = 0;
    int empty=0;
    int secondCount = 0;  //��һ�������count
    //����Ϊ���� 
    count = 1;
    block = 0;
    empty = -1;
    secondCount=0;
    for(int i=y+1;true;i++){
    	if(i>=19)
    	{block++;
    	break;
		}
	    if(board[x][i]==2){
	    	if(empty==-1&&i<18&&board[x][i+1]==role)
	    	{empty=count;
	    	continue;
			}
			else break;
		}
		if(board[x][i]==role){
			count++;
			continue;
		}
		else{
			block++;
			break;
		}
	}
	for(int i=y-1;true;i--){
		if(i<0){
			block++;
			break;
		}
		if(board[x][i]==2){
			if(empty == -1 && i>0 && board[x][i-1] == role) {
            empty = 0;  //ע��������0����Ϊ�Ǵ��������ߵ�
			 continue;
			  } else  break;  
		}
		if(board[x][i]==role){
			secondCount++;
		if	(empty != -1 ) 
		{empty ++;  //ע������������ֶ��˼������ӣ���ôempty��λ�þͱ����
        continue;}}
        else {
         block ++;
		 break;

      }
		}
		count=count+secondCount;
		scorecache[role][x][y][0]=Toscore(count,block,empty);
	
    result=result+scorecache[role][x][y][0];
    
  //����Ϊ����  
    count = 1;
    block = 0;
    empty = -1;
    secondCount=0;
    for(int i=x+1;true;i++){
    	if(i>=19)
    	{block++;
    	break;
		}
	    if(board[i][y]==2){
	    	if(empty==-1&&i<18&&board[x+1][y]==role)
	    	{empty=count;
	    	continue;
			}
			else break;
		}
		if(board[i][y]==role){
			count++;
			continue;
		}
		else{
			block++;
			break;
		}
	}
	for(int i=x-1;true;i--){
		if(i<0){
			block++;
			break;
		}
		if(board[i][y]==2){
			if(empty == -1 && i>0 && board[x-1][y] == role) {
            empty = 0;  //ע��������0����Ϊ�Ǵ��������ߵ�
			 continue;
			  } else  break;  
		}
		if(board[i][y]==role){
			secondCount++;
		if	(empty != -1 )
		{ empty ++;  //empty��λ�ñ�� 
        continue;}}
        else {
         block ++;
		 break;

      }
		}
		count=count+secondCount;
		scorecache[role][x][y][1]=Toscore(count,block,empty);
	
    result=result+scorecache[role][x][y][1];

//����Ϊ�K��  
    count = 1;
    block = 0;
    empty = -1;
    secondCount=0;
    for(int i=1;true;i++){
    	int m=x+i;
		int n=y+i; 
    	if(m>=19||n>=19)
    	{block++;
    	break;
		}
	    if(board[m][n]==2){
	    	if(empty==-1&&(m<18&&n<18)&&board[m+1][n+1]==role)
	    	{empty=count;
	    	continue;
			}
			else break;
		}
		if(board[m][n]==role){
			count++;
			continue;
		}
		else{
			block++;
			break;
		}
	}
	for(int i=1;true;i++){
		int m=x-i;
		int n=y-i;
		if(m<0||n<0){
			block++;
			break;
		}
		if(board[m][n]==2){
			if(empty == -1 && m>0&&n>0 && board[m-1][n-1] == role) {
            empty = 0;  //ע��������0����Ϊ�Ǵ��������ߵ�
			 continue;
			  } else  break;  
		}
		if(board[m][n]==role){
			secondCount++;
		if	(empty != -1 )
		{empty ++; //ע������������ֶ��˼������ӣ���ôempty��λ�þͱ����
        continue;}}
        else {
         block ++;
		 break;

      }
		}
		count=count+secondCount;
		scorecache[role][x][y][2]=Toscore(count,block,empty);
	
    result=result+scorecache[role][x][y][2];
//����Ϊ�L��  
    count = 1;
    block = 0;
    empty = -1;
    secondCount=0;
    for(int i=1;true;i++){
    	int m=x+i;
		int n=y-i; 
    	if(m>=19||n>=19||m<0||n<0)
    	{block++;
    	break;
		}
	    if(board[m][n]==2){
	    	if(empty==-1&&(m<18&&n>0)&&board[m+1][n-1]==role)
	    	{empty=count;
	    	continue;
			}
			else break;
		}
		if(board[m][n]==role){
			count++;
			continue;
		}
		else{
			block++;
			break;
		}
	}
	for(int i=1;true;i++){
		int m=x-i;
		int n=y+i;
		if(m<0||n>=19){
			block++;
			break;
		}
		if(board[m][n]==2){
			if(empty == -1 && m>0&&n>0 && board[m-1][n+1] == role) {
            empty = 0;  //ע��������0����Ϊ�Ǵ��������ߵ�
			 continue;
			  } else  break;  
		}
		if(board[m][n]==role){
			secondCount++;
		if	(empty != -1 )
		{ empty ++; //ע������������ֶ��˼������ӣ���ôempty��λ�þͱ����
        continue;}}
        else {
         block ++;
		 break;

      }
		}
		count=count+secondCount;
		scorecache[role][x][y][3]=Toscore(count,block,empty);
	
    result=result+scorecache[role][x][y][3];
    
	return result;
} 


Board* gen(int role,int computerside,int board[19][19]){
	//roleΪ��ǰ��Ҫ�����Ľ�ɫ
	//computersideΪ����ִ����ɫ
	//boardΪ����
	
	 
	Board p; 
	Score S;
    
    vector<Board> comsix;
    vector<Board> humsix;
    vector<Board> comfives;
    vector<Board> humfives;
    vector<Board> comfours;
    vector<Board> humfours;
    vector<Board> comblockedfours ;
    vector<Board> humblockedfours ;
    vector<Board> comtwothrees;
    vector<Board> humtwothrees;
    vector<Board> comthrees ;
    vector<Board> humthrees ;
    vector<Board> comtwos ;
    vector<Board> humtwos;
    vector<Board> neighbors;
    

    int com[19][19];
    int hum[19][19];
   	for(int i=0;i<19;i++)
	for(int j=0;j<19;j++){
		
		 if(board[i][j] == 2) {

         //���������ھӵĲ���

            int cs = scorePoint(i, j, computerside,board);//�������� 

            int hs = scorePoint( i, j, 1-computerside,board);//�Է����� 

            com[i][j] = cs;

            hum[i][j] = hs;

          

        } else if (board[i][j] ==computerside) { // �Լ�����֣����˴�λ�÷���Ϊ0

          com[i][j] = scorePoint( i, j,computerside,board);

          hum[i][j] = 0; 

        } else if (board[i][j] ==1-computerside) { // �Եз���֣�����λ�÷���Ϊ0

          hum[i][j] = scorePoint(i, j, 1-computerside,board);

          com[i][j] = 0;}

		if(board[i][j]==2){
			p.humScore=hum[i][j];
			p.comScore=com[i][j];
			p.x=i;
			p.y=j;

	if(p.comScore>=S.SIX){
		comsix.push_back(p);
		
	}else if(p.humScore>=S.SIX)
	{
		humsix.push_back(p);
	}
	else if(p.comScore >= S.FIVE) {//�����ܲ�������5

            comfives.push_back(p);

          } else if(p.humScore >= S.FIVE) {//����ܲ�������5

            humfives.push_back(p);

          } else if(p.comScore >= S.FOUR) {

            comfours.push_back(p);

          } else if(p.humScore >= S.FOUR) {

            humfours.push_back(p);

          } else if(p.comScore >= S.BLOCKED_FOUR) {

            comblockedfours.push_back(p);

          } else if(p.humScore >= S.BLOCKED_FOUR) {

            humblockedfours.push_back(p);

          } else if(p.comScore >= 2*S.THREE) {

            //�ܳ�˫��Ҳ��

            comtwothrees.push_back(p);

          } else if(p.humScore >= 2*S.THREE) {

            humtwothrees.push_back(p);

          } else if(p.comScore >= S.THREE) {

            comthrees.push_back(p);

          } else if(p.humScore>= S.THREE) {

            humthrees.push_back(p);

          } else if(p.comScore >= S.TWO) {

            comtwos.push_back(p);

          } else if(p.humScore >= S.TWO) {

            humtwos.push_back(p);

          } else neighbors.push_back(p);

        }

      }

 //����������Ǳ�ɱ�壬ֱ�ӷ���
    if(!comsix.empty())
	{
    	return &comsix[0];
	}

    if(role==computerside&&!comfives.empty()) return &comfives[0];
    if(role==1-computerside&&!humfives.empty()) return &humfives[0];
	

    

    // �Լ��ܻ��ģ���ֱ�ӻ��ģ������ǳ���

    if (role ==computerside && !comfours.empty()) return &comfours[0];

    if (role ==1-computerside && !humfours.empty()) return &humfours[0];



    // �����л��ĳ��ģ��Լ����Ķ�û����ֻ���Ƕ������ ����ʱ������ľͲ��ÿ�����)

    

    if (role ==computerside && !humfours.empty() && comblockedfours.empty()) return &humfours[0];

    if (role ==1-computerside && !comfours.empty() && humblockedfours.empty()) return &comfours[0];



    // �����л����Լ��г��ģ��򶼿�����
    vector<Board> fours;
    vector<Board>blockedfours;
    if(role==computerside){
    	
    	fours.insert(fours.end(),comfours.begin(),comfours.end());
		fours.insert(fours.end(),humfours.begin(),humfours.end());
    	
    	blockedfours.insert(blockedfours.end(), comblockedfours.begin(),comblockedfours.end());
		blockedfours.insert(blockedfours.end(),humblockedfours.begin(),humblockedfours.end());
	}
	else{
    		
    	fours.insert(fours.end(),humfours.begin(),humfours.end());
		fours.insert(fours.end(),comfours.begin(),comfours.end());
    	
    	blockedfours.insert(blockedfours.end(),humblockedfours.begin(),humblockedfours.end());
		blockedfours.insert(blockedfours.end(),comblockedfours.begin(),comblockedfours.end());
	}
   
    if (!fours.empty())
	{vector<Board> sum;
    sum.insert(sum.end(),fours.begin(),fours.end());
    sum.insert(sum.end(),blockedfours.begin(),blockedfours.end());
	return &sum[0];
	 } 



    vector<Board> result;

    if (role == computerside) {
    	
      result.insert(result.end(),comtwothrees.begin(),comtwothrees.end());
      result.insert(result.end(),humtwothrees.begin(),humtwothrees.end());
     
        result.insert(result.end(),comblockedfours.begin(),comblockedfours.end());
        
       result.insert(result.end(),humblockedfours.begin(),humblockedfours.end());
      
       result.insert(result.end(),comthrees.begin(),comthrees.end());

        result.insert(result.end(),humthrees.begin(),humthrees.end());

    }

    if (role ==1-computerside) {

     	 result.insert(result.end(),humtwothrees.begin(),humtwothrees.end());
      result.insert(result.end(),comtwothrees.begin(),comtwothrees.end());
     
        result.insert(result.end(),humblockedfours.begin(),humblockedfours.end());
        
       result.insert(result.end(),comblockedfours.begin(),comblockedfours.end());
      
       result.insert(result.end(),humthrees.begin(),humthrees.end());

        result.insert(result.end(),comthrees.begin(),comthrees.end());

    }


    //˫�������⣬��Ϊ���γ�˫���Ĳ�һ����һ������ǿ

    if(!comtwothrees.empty() || !humtwothrees.empty()) {

      return &result[0];

    }





    // ֻ���ش��ڵ��ڻ�������





    vector<Board> twos;

    if (role ==computerside) 
	{

	twos.insert(twos.end(),comtwos.begin(),comtwos.end());
	twos.insert(twos.end(),humtwos.begin(),humtwos.end());}

    else	{
	twos.insert(twos.end(),humtwos.begin(),humtwos.end());
	twos.insert(twos.end(),comtwos.begin(),comtwos.end());

}




    return &result[0];

  }




