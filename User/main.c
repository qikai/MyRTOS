#include "stdio.h"
#include "list.h"
#include "task.h"

portCHAR flag1;
portCHAR flag2;

extern List_t pxReadyTasksLists[ configMAX_PRIORITIES ];

/*实现两个任务*/
TCB_t Task1TCB; 
TCB_t Task2TCB;
TaskHandle_t Task1_Handle;
TaskHandle_t Task2_Handle;

#define TASK1_STACK_SIZE			128
StackType_t Task1Stack[TASK1_STACK_SIZE];

#define TASK2_STACK_SIZE			128
StackType_t Task2Stack[TASK2_STACK_SIZE];

/*软件延时*/
void delay(uint32_t count)
{
	for(; count!=0; count--);
}

/*任务1*/
void Task1_Entry(void *p_arg)
{
	for(;;)
	{
		flag1 = 1;
		delay(100);
		flag1 = 0;
		delay(100);
		taskYIELD();
	}
}

/*任务1*/
void Task2_Entry(void *p_arg)
{
	for(;;)
	{
		flag2 = 1;
		delay(100);
		flag2 = 0;
		delay(100);
		taskYIELD();
	}
}

int main(void)
{
	/*初始化与任务相关的列表，如就绪列表*/
	prvInitialiseTaskLists();
	 
	Task1_Handle = xTaskCreateStatic( (TaskFunction_t)Task1_Entry,		/*任务入口函数*/ 
                                    (char *)"Task1",                /*任务名称，字符串形式*/
                                    (uint32_t)TASK1_STACK_SIZE,     /*任务栈大小，单位为字*/
																		(void *) NULL,                  /*任务形参*/
																		(StackType_t *)Task1Stack,      /*任务栈起始地址*/
																		(TCB_t *)&Task1TCB);            /*任务控制块*/
																		
	/*将任务添加到就绪列表里*/ 
	vListInsertEnd( &( pxReadyTasksLists[1] ),
									&( ((TCB_t *)(&Task1TCB))->xStateListItem ) );
																		
	Task1_Handle = xTaskCreateStatic( (TaskFunction_t)Task2_Entry,		/*任务入口函数*/ 
                                    (char *)"Task2",                /*任务名称，字符串形式*/
                                    (uint32_t)TASK2_STACK_SIZE,     /*任务栈大小，单位为字*/
																		(void *) NULL,                  /*任务形参*/
																		(StackType_t *)Task2Stack,      /*任务栈起始地址*/
																		(TCB_t *)&Task2TCB);            /*任务控制块*/
																		
	/*将任务添加到就绪列表里*/ 
	vListInsertEnd( &( pxReadyTasksLists[2] ),
									&( ((TCB_t *)(&Task2TCB))->xStateListItem ) );
	
	/*启动调度器，若成功，则不返回*/																	
  vTaskStartScheduler();		 														
																		
	while(1);//do something
}
