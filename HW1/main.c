#include <stdio.h>
#include <stdlib.h>

int get_result(int computerChoice,int humanChoice)
{
	switch (computerChoice)
		{
		case 1: 
			if (humanChoice == 2)
			{
				printf("You win: paper beats rock\n");
			}
			else if(humanChoice == 3)
			{
				printf("I win: rock beats scissors\n");
			}
			else 
			{
				printf("draw\n");
			}
				
			break;
		case 2:

			if (humanChoice == 2)
			{
				printf("draw\n");
				
			}
			else if(humanChoice == 3)
			{
				printf("You win: scissors beat paper\n");
			}
			else 
			{
				printf("I win: paper beats rock\n");
			}
			break;
		case 3:

			if (humanChoice == 3)
			{
				printf("draw\n");
			}
			else if (humanChoice == 2)
			{
				printf("I win: scissors beat paper\n");
			}
			else 
			{
				printf("You win: rock beats scissors\n");
			}

			break;
		}
}

int main ()
{
	int computerChoice,humanChoice,gameNumber;
		
	printf("Please, enter the game number\n");
	scanf("%d", &gameNumber);

	while(gameNumber != 0)
	{
		printf("Please choose: 1 (rock)  - 2 (paper) - 3 (scissors)\n");
		scanf("%d", &humanChoice);
		computerChoice = rand() % (3 + 1 - 1) + 1;//rand between 1 and 3
		
		printf("You choose %d, I choose %d\n", humanChoice, computerChoice);
		get_result(computerChoice,humanChoice);
		
		gameNumber--;
	}	
}
