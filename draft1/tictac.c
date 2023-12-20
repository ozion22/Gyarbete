#include <stdio.h>
int board[3][3];
void initboard()
{
	for(int i = 0; i<3;i++)
	{
		for(int j=0; j<3;j++)
		{
			board[i][j]=0;
		}
	}
}

int main()
{
	for(int i=0; i<3;i++)
	{
		for(int j=0;j<3;j++)
		{
			printf("%d", board[i][j]);
		}
	}
	return 0;
}
