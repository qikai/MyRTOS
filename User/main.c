#include "stdio.h"
#include "list.h"
#include "task.h"

portCHAR flag1;
portCHAR flag2;

extern List_t pxReadyTasksLists[ configMAX_PRIORITIES ];

/*ʵ����������*/
TCB_t Task1TCB; 
TCB_t Task2TCB;
TaskHandle_t Task1_Handle;
TaskHandle_t Task2_Handle;

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
		taskYIELD();
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
		taskYIELD();
	}
}

int main(void)
{
	/*��ʼ����������ص��б�������б�*/
	prvInitialiseTaskLists();
	 
	Task1_Handle = xTaskCreateStatic( (TaskFunction_t)Task1_Entry,		/*������ں���*/ 
                                    (char *)"Task1",                /*�������ƣ��ַ�����ʽ*/
                                    (uint32_t)TASK1_STACK_SIZE,     /*����ջ��С����λΪ��*/
																		(void *) NULL,                  /*�����β�*/
																		(StackType_t *)Task1Stack,      /*����ջ��ʼ��ַ*/
																		(TCB_t *)&Task1TCB);            /*������ƿ�*/
																		
	/*��������ӵ������б���*/ 
	vListInsertEnd( &( pxReadyTasksLists[1] ),
									&( ((TCB_t *)(&Task1TCB))->xStateListItem ) );
																		
	Task1_Handle = xTaskCreateStatic( (TaskFunction_t)Task2_Entry,		/*������ں���*/ 
                                    (char *)"Task2",                /*�������ƣ��ַ�����ʽ*/
                                    (uint32_t)TASK2_STACK_SIZE,     /*����ջ��С����λΪ��*/
																		(void *) NULL,                  /*�����β�*/
																		(StackType_t *)Task2Stack,      /*����ջ��ʼ��ַ*/
																		(TCB_t *)&Task2TCB);            /*������ƿ�*/
																		
	/*��������ӵ������б���*/ 
	vListInsertEnd( &( pxReadyTasksLists[2] ),
									&( ((TCB_t *)(&Task2TCB))->xStateListItem ) );
	
	/*���������������ɹ����򲻷���*/																	
  vTaskStartScheduler();		 														
																		
	while(1);//do something
}
