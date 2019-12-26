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


StackType_t IdleTaskStack[configMINIMAL_STACK_SIZE];
/*������������������ƿ�*/
TCB_t IdleTaskTCB;

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
#if 0
		flag1 = 1;
		delay(100);
		flag1 = 0;
		delay(100);
		taskYIELD();
#else
		flag1 = 1;
		vTaskDelay(100);
		flag1 = 0;
		vTaskDelay(100);
#endif
	}
}

/*����1*/
void Task2_Entry(void *p_arg)
{
	for(;;)
	{
#if 0
		flag2 = 1;
		delay(100);
		flag2 = 0;
		delay(100);
		taskYIELD();
#else
		flag2 = 1;
		vTaskDelay(100);
		flag2 = 0;
		vTaskDelay(100);
#endif
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
																		(UBaseType_t) 1,                /*�������ȼ�������Խ�ߣ����ȼ�Խ��*/
																		(StackType_t *)Task1Stack,      /*����ջ��ʼ��ַ*/
																		(TCB_t *)&Task1TCB);            /*������ƿ�*/
																		
	/*��������ӵ������б���*/ 
//	vListInsertEnd( &( pxReadyTasksLists[1] ),
//									&( ((TCB_t *)(&Task1TCB))->xStateListItem ) );
																		
	Task1_Handle = xTaskCreateStatic( (TaskFunction_t)Task2_Entry,		/*������ں���*/ 
                                    (char *)"Task2",                /*�������ƣ��ַ�����ʽ*/
                                    (uint32_t)TASK2_STACK_SIZE,     /*����ջ��С����λΪ��*/
																		(void *) NULL,                  /*�����β�*/
																		(UBaseType_t) 2,                /*�������ȼ�������Խ�ߣ����ȼ�Խ��*/
																		(StackType_t *)Task2Stack,      /*����ջ��ʼ��ַ*/
																		(TCB_t *)&Task2TCB);            /*������ƿ�*/
																		
	/*��������ӵ������б���*/ 
//	vListInsertEnd( &( pxReadyTasksLists[2] ),
//									&( ((TCB_t *)(&Task2TCB))->xStateListItem ) );
	
	/*���������������ɹ����򲻷���*/																	
  vTaskStartScheduler();		 														
																		
	while(1);//do something
}


void vApplicationGetIdleTaskMemory(TCB_t **ppxIdleTaskTCBBuffer,
	                                 StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize)
{
	*ppxIdleTaskTCBBuffer = &IdleTaskTCB;
	*ppxIdleTaskStackBuffer = IdleTaskStack;
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}



