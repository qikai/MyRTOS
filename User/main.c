#include "stdio.h"
#include "list.h"

char flag1;
char flag2;
/*ʵ����������*/

#define TASK1_STACK_SIZE			128
StackType_t Task1Stack[TASK1_STACK_SIZE];

#define TASK2_STACK_SIZE			128
StackType_t Task2Stack[TASK2_STACK_SIZE];

/*�����ʱ*/
void delay(uint32_t count)
{
	for(; count!=0; count--);
}

/*����1*/
void Task1_Entry(void *p_arg)
{
	for(;;)
	{
		flag1 = 1;
		delay(100);
		flag1 = 0;
		delay(100);
	}
}

/*����1*/
void Task2_Entry(void *p_arg)
{
	for(;;)
	{
		flag2 = 1;
		delay(100);
		flag2 = 0;
		delay(100);
	}
}

int main(void)
{

	
	while(1);//do something
}
