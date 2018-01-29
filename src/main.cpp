
#include <iostream>
#include <string.h>

#include "NacEngine.h"

#define		CNT		3

/* run this program using the console pauser or add your own getch, system("pause") or input loop */


void printPlayComResult(const int & row, const int & col);
void printPlayHumanResult(const int & row, const int & col);
void printPlayResult(NacEngine::PlayResult result);


int main(int argc, char** argv) {
    
    NacEngine *pNacEngine = new NacEngine();
	NacEngine::CellState board[CNT][CNT];
	int i, j;
	int row, col, no;
	
    while( true )
    {
       std::cout<<"New Game Start!\n"<<"Do you want to play first?(y/n)\n";

       for(i = 0 ; i < CNT ; i++)
       {
 		   for( j = 0 ; j < CNT ; j++)
 		   		board[i][j] = NacEngine::Empty;
	   }
	   pNacEngine->init();	   
       
       bool bFirst = false;
       char inStr[64];
       while( true )
       {
          std::cin>>inStr;
          if (stricmp(inStr, "y") == 0)
          {
           	 bFirst = true;
             break;
          }
          else if (stricmp(inStr, "n") == 0)
          {
		   	 bFirst = false;
			 break;  
		  }
		  
		  std::cout<<"Wrong input!\nInput 'y' or 'n'\n";
       }
       
	   if (!bFirst)
	   {
	   	   pNacEngine->PlayCom(row, col);
	   	   board[row][col] = NacEngine::Com;	   	   
	   	   printPlayComResult(row, col);
	   }
	   
	   while (true)
	   {
	 	   // std::cout<<"You have to play a move. \nInput number of the cell for you to be going to mark.\n The Value of number is range from 1 to 9.\n\n";
	 	   while (true)
	 	   {
       	       std::cin>>no;
       	       if ( no < 1 || no > 9 )
			   	   std::cout<<"Wrong input! Input the value between 1 and 9.\n";	  
  		       else
  		       {
       	   	   	   row = (no - 1) / CNT;
  	 	   		   col = (no - 1) % CNT;
  	 	   		   if (board[row][col] != NacEngine::Empty)
  	 	   		   {
					  std::cout<<"The cell is already marked. Choose the other cell.\n";	 
                   }
                   else
				   	   break;  		       
	           }
  	 	   }
  	 	   
  	 	   board[row][col] = NacEngine::Human;
  	 	   printPlayHumanResult(row, col);
  	 	   NacEngine::PlayResult result = pNacEngine->PlayHuman(row, col);
  	 	   
  	 	   if (result != NacEngine::Waiting)
		   {
		   	   printPlayResult( result );
		   	   break;
		   }
  	 	   
  	 	   result = pNacEngine->PlayCom(row, col);
  	 	   board[row][col] = NacEngine::Com;
  	 	   printPlayComResult(row, col);
		   if (result != NacEngine::Waiting)
		   {
		   	   printPlayResult( result );
		   	   break;
		   } 	 	   
	   }
	   
	   std::cout<<"Game Over!\n"<<"Do you want to play again?(y/n)\n\n";
       bool bAgain = true;
       while( true )
       {
          std::cin>>inStr;
          if (stricmp(inStr, "y") == 0)
          {
           	 bAgain = true;
             break;
          }
          else if (stricmp(inStr, "n") == 0)
          {
		   	 bAgain = false;
			 break;  
		  }
		  
		  std::cout<<"Wrong input!\nInput 'y' or 'n'\n";
       }
       
       if (!bAgain)
       	  break;
    }
    
	return 0;
}


void printPlayComResult(const int & row, const int & col)
{
	int no = row * CNT + col + 1;
	std::cout<<"*("<<no<<") --> \n";
	//std::cout<<"*("<<row<<", "<<col<<") --> \n";
}

void printPlayHumanResult(const int & row, const int & col)
{
	int no = row * CNT + col + 1;
	std::cout<<"O("<<no<<") --> \n";
	//std::cout<<"O("<<row<<", "<<col<<") --> \n";
}

void printPlayResult(NacEngine::PlayResult result)
{
	switch (result)
	{
		case NacEngine::HumanWin:
			std::cout<<"You win!\n\n";
		 	break;
	 	case NacEngine::ComWin:
			std::cout<<"I win!\n\n";
		 	break;
	 	case NacEngine::Draw:
			std::cout<<"This game is a draw!\n\n";
		 	break;
	 	default:
	 		;
	}
}



