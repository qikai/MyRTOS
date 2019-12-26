#include "task.h"
#include "list.h"
#include "portable.h"

/*����uxTopReadyPriority*/
static volatile UBaseType_t uxTopReadyPriority = tskIDLE_PRIORITY;

static volatile UBaseType_t uxCurrentNumberOfTasks = 0;

/*����������ȼ��ľ�������ͨ�÷���*/
#if ( configUSE_PORT_OPTIMISED_TASK_SELECTION == 0)

/* UXtOPrEADYpRIORITY ����Ǿ��������������ȼ�*/
#define taskRECORD_READY_PRIORITY( uxPriority )\
{\
	if( ( uxPriority ) > uxTopReadyPriority )\
	{\
		uxTopReadyPriority = ( uxPriority );\
	}\
}

#define taskSELECT_HIGHEST_PRIORITY_TASK() \
{\
	UBaseType_t uxTopPriority = uxTopReadyPriority;\
	/*Ѱ�Ұ������������������ȼ��Ķ���*/\
	while( listLIST_IS_EMPTY( &(pxReadyTasksLists[uxTopPriority])))\
	{\
		--uxTopPriority;\
	}\
	/*��ȡ���ȼ���ߵľ��������TCB��Ȼ����µ�pxCurrentTCB*/\
	listGET_OWNER_OF_NEXT_ENTRY(pxCurrentTCB, &(pxReadyTasksLists[ uxTopPriority ]));\
	/* ����uxTopReadyPriority */\
	uxTopReadyPriority = uxTopPriority;\
}

/* �������궨��ֻ����ѡ���Ż�����ʱ���ã����ﶨ��Ϊ�� */
#define taskRESET_READY_PRIORITY( uxPriority )
#define portRESET_READY_PRIORITY( uxPriority, uxTopReadyPriority )

/* ����������ȼ��ľ������񣺸��ݴ������ܹ��Ż���ķ��� */
#else
#define taskRECORD_READY_PRIORITY( uxPriority )\
          portRECORD_READY_PRIORITY( uxPriority, uxTopReadyPriority )

#define taskSELECT_HIGHEST_PRIORITY_TASK()\
{\
	UBaseType_t uxTopPriority;\
	/*Ѱ��������ȼ�*/\
	portGET_HIGHEST_PRIORITY( uxTopPriority, uxTopReadyPriority );\
	/* ��ȡ���ȼ���ߵľ��������TCB�� Ȼ����µ�pxCurrentTCB */\
	listGET_OWNER_OF_NEXT_ENTRY( pxCurrentTCB, &( pxReadyTasksLists[ uxTopPriority ] ) );\
}

#define taskSWITCH_DELAYED_LISTS()\
{\
	List_t *pxTemp;\
	pxTemp = pxDelayedTaskList;\
	pxDelayedTaskList = pxOverflowDelayedTaskList;\
	pxOverflowDelayedTaskList = pxTemp;\
	xNumOfOverflows++;\
	prvResetNextTaskUnblockTime();\
}


#if 1
#define taskRESET_READY_PRIORITY( uxPriority )\
{\
	if(listCURRENT_LIST_LENGTH(&(pxReadyTasksLists[(uxPriority)])) == (UBaseType_t)0 )\
	{\
	    portRESET_READY_PRIORITY( ( uxPriority ), ( uxTopReadyPriority ));\
	}\
}
#else
#define taskRESET_READY_PRIORITY( uxPriority )\
{\
	portRESET_READY_PRIORITY( ( uxPriority ), ( uxTopReadyPriority ));\
}
#endif

#endif

#define prvAddTaskToReadyList(pxTCB)\
        taskRECORD_READY_PRIORITY( ( pxTCB )->uxPriority );\
        vListInsertEnd( &( pxReadyTasksLists[ ( pxTCB )->uxPriority ] ),\
        &( ( pxTCB )->xStateListItem ) );



/*������ʱ�б�*/
static List_t xDelayedTaskList1;      //��xTickCountû�����ʱ���������б�
static List_t xDelayedTaskList2;      //��xTickCount���ʱ���������б�
static List_t * volatile pxDelayedTaskList; //ָ��û������������б�
static List_t * volatile pxOverflowDelayedTaskList; //ָ����������б�


/*ȫ��ָ�룬����ָ��ǰ�������л��߼���Ҫ���е������������ƿ�*/
TCB_t *pxCurrentTCB=NULL;  

TaskHandle_t xIdleTaskHandle;

extern TCB_t Task1TCB; 
extern TCB_t Task2TCB;
extern TCB_t IdleTaskTCB;
extern TickType_t xTickCount;

static volatile BaseType_t xNumOfOverflows 			= ( BaseType_t ) 0;
static volatile TickType_t xNextTaskUnblockTime;

void vApplicationGetIdleTaskMemory(TCB_t **ppxIdleTaskTCBBuffer,
	                                 StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize);

static void prvAddNewTaskToReadyList( TCB_t *pxNewTCB)
{
	/*�����ٽ��*/
	taskENTER_CRITICAL();
	
	/*ȫ�������������һ*/
	uxCurrentNumberOfTasks++;
	
	/*���pxCurrentTCBΪ�գ���pxCurrentTCBָ���´���������*/
	if ( pxCurrentTCB == NULL )
	{
		pxCurrentTCB = pxNewTCB;
		/*����ǵ�һ�δ�����������Ҫ��ʼ��������ص��б�*/
		if( uxCurrentNumberOfTasks == ( UBaseType_t ) 1 )
		{
			/*��ʼ��������ص��б�*/
			prvInitialiseTaskLists();
		}
	}
	else/*���pxCurrentTCB��Ϊ�գ��������������ȼ���pxCurrentTCBָ�����ȼ���ߵ�TCB*/
	{
		if( pxCurrentTCB->uxPriority <= pxNewTCB->uxPriority)
		{
			pxCurrentTCB = pxNewTCB;
		}
	}
	
	/*��������ӵ������б�*/
	prvAddTaskToReadyList( pxNewTCB );
	
	/*�˳��ٽ��*/
	taskEXIT_CRITICAL();
	
}

void prvIdleTask(void *p_arg);

List_t pxReadyTasksLists[configMAX_PRIORITIES];

void prvInitialiseNewtask( TaskFunction_t pxTaskCode,
	                                const char * const pcName,
																	const uint32_t ulStackDepth,
																	void * const pvParameters,
																	UBaseType_t uxPriority,
																	TaskHandle_t * const pxCreatedTask,
																	TCB_t *pxNewTCB );


#if( configSUPPORT_STATIC_ALLOCATION == 1)
TaskHandle_t xTaskCreateStatic( TaskFunction_t pxTaskCode,
                                const char * const pcName,
																const uint32_t ulStackDepth,
																void * const pvParameters,
																UBaseType_t   uxPriority,
																StackType_t * const puxStackBuffer,
																TCB_t * const pxTaskBuffer)
{
	TCB_t *pxNewTCB;
	TaskHandle_t xReturn;
	
	if( ( pxTaskBuffer  != NULL ) && ( puxStackBuffer != NULL ))
	{
		pxNewTCB = ( TCB_t *) pxTaskBuffer;
		pxNewTCB->pxStack = ( StackType_t * ) puxStackBuffer; //��������ƿ��е�ջָ��ָ������ջ
		
		/*�����µ�����*/
		prvInitialiseNewtask(pxTaskCode,	       /*�������*/
		                      pcName,            /*�������ƣ��ַ�����ʽ*/
		                      ulStackDepth,      /*����ջ��С����λΪ��*/
		                      pvParameters,      /*�����β�*/
		                      uxPriority,        /*�������ȼ�*/
		                      &xReturn,          /*������*/
		                      pxNewTCB);         /*����ջ��ʼ��ַ*/
		
		/*��������ӵ������б�*/
		prvAddNewTaskToReadyList( pxNewTCB );
	}
	else
	{
		xReturn = NULL;
	}
	
	/*������������������񴴽��ɹ�����ʱxReturnӦ��ָ��������ƿ�*/
	return xReturn;
}
#endif

static void prvInitialiseNewtask( TaskFunction_t pxTaskCode,
	                                const char * const pcName,
																	const uint32_t ulStackDepth,
																	void * const pvParameters,
																	UBaseType_t   uxPriority,
																	TaskHandle_t * const pxCreatedTask,
																	TCB_t *pxNewTCB )
{
	StackType_t *pxTopOfStack;
	UBaseType_t x;
	
	/*��ȡջ����ַ, ���ֽ�һ����λ*/
	pxTopOfStack = pxNewTCB->pxStack + ( ulStackDepth - ( uint32_t ) 1);
	
	/*������8�ֽڶ���,��Ϊջ�ǴӸ������������������£���ΪҪ���ݸ������㣬����Ҫ8�ֽڶ���*/
	pxTopOfStack = ( StackType_t * ) ( ( ( uint32_t ) pxTopOfStack ) & ( ~( ( uint32_t ) 0x00007 ) ) ); 
	
	/*������8�ֽڶ���, ���Ǵӵ�����������*/
	//pxTopOfStack = ( StackType_t * ) ( ( ( uint32_t ) pxTopOfStack + 0x00008) & ( ~( ( uint32_t ) 0x00007 ) ) );

  /*����������ִ洢��TCB��*/
  for( x = ( UBaseType_t ) 0; x < ( UBaseType_t ) configMAX_TASK_NAME_LEN; x++)
 {
    pxNewTCB->pcTaskName[x] = pcName[x];
	 
	  if( pcName[x] == 0x00)
		{
			break;
		}
 }

 /*�������ֵĳ��Ȳ��ܳ���configMAX_TASK_NAME_LEN*/ 
	pxNewTCB->pcTaskName [ configMAX_TASK_NAME_LEN - 1] = '\0';
 
 /*��ʼ��TCB�е�xStateListItem�ڵ�*/
 vListInitialiseItem( & ( pxNewTCB->xStateListItem ) );
 
 /*����xStateListItem�ڵ��ӵ����,��������ƿ鱾��*/
 listSET_LIST_ITEM_OWNER( &( pxNewTCB->xStateListItem), pxNewTCB ); 
 
 /*��ʼ�����ȼ�*/
 if( uxPriority >= ( UBaseType_t ) configMAX_PRIORITIES )
 {
	 uxPriority = ( UBaseType_t ) configMAX_PRIORITIES - ( UBaseType_t ) 1U;
 }
 pxNewTCB->uxPriority = uxPriority;
 
 /*��ʼ������ջ*/
 pxNewTCB->pxTopOfStack = pxPortInitialiseStack(pxTopOfStack, pxTaskCode, pvParameters);
 
 /*��������ָ��������ƿ�*/
 if( ( void *) pxCreatedTask != NULL )
 {
	 *pxCreatedTask = ( TaskHandle_t ) pxNewTCB;
 }  
}


/*�����б��ʼ��*/
void prvInitialiseTaskLists(void)
{
	UBaseType_t uxPriority;
	
	for( uxPriority = ( UBaseType_t ) 0U;
	     uxPriority < ( UBaseType_t) configMAX_PRIORITIES;
	     uxPriority++)
	{ 
		vListInitialise( &( pxReadyTasksLists[uxPriority] ) );
	}
	
	/*��ʼ����ʱ�б�*/
	vListInitialise(&xDelayedTaskList1);
	vListInitialise(&xDelayedTaskList2);
	
	pxDelayedTaskList = &xDelayedTaskList1;
	pxOverflowDelayedTaskList = &xDelayedTaskList2;
}


static void prvResetNextTaskUnblockTime(void)
{
	TCB_t *pxTCB;
	if(listLIST_IS_EMPTY(pxDelayedTaskList)!=pdFALSE)
	{
		/*��ǰ��ʱ�б�Ϊ�գ�������xNextTaskUnblockTime�������ֵ*/
		xNextTaskUnblockTime = portMAX_DELAY;
	}
	else
	{
		/*��ǰ�б�Ϊ�գ�������������ʱ�����ȡ��ǰ�б��µ�һ���ڵ������ֵ
		Ȼ�󽫸ýڵ������ֵ���µ�xNextTaskUnblockTime*/
		(pxTCB) = (TCB_t*)listGET_OWNER_OF_HEAD_ENTRY(pxDelayedTaskList);
		xNextTaskUnblockTime = listGET_LIST_ITEM_VALUE(&((pxTCB)->xStateListItem));
	}
}

//=================================================
//								������
//=================================================

//����������
void vTaskStartScheduler( void )
{
	/*=============������������start================*/
	TCB_t *pxIdleTaskTCBBuffer = NULL;          /*����ָ�����������ƿ�*/
	StackType_t *pxIdleTaskStackBuffer = NULL;  /*���ڿ�������ջ��ʼ��ַ*/
	uint32_t  ulIdleTaskStackSize;
	
	/*��ȡ����������ڴ棺����ջ������TCB*/
	vApplicationGetIdleTaskMemory(&pxIdleTaskTCBBuffer,
	                              &pxIdleTaskStackBuffer,
	                              &ulIdleTaskStackSize);
	
	/*������������*/
	xIdleTaskHandle =
	xTaskCreateStatic((TaskFunction_t)prvIdleTask,           /*�������*/
	                  (char *)"IDLE",                        /*�������֣��ַ�����ʽ*/
										(uint32_t)ulIdleTaskStackSize,         /*����ջ��С����λΪ��*/
										(void *)NULL,                          /*�����β�*/
										(UBaseType_t) tskIDLE_PRIORITY,
										(StackType_t *)pxIdleTaskStackBuffer,  /*����ջ��ʼ��ַ*/
										(TCB_t *)pxIdleTaskTCBBuffer);         /*������ƿ�*/

	/*��������ӵ������б���*/
//	vListInsertEnd(&(pxReadyTasksLists[0]),
//									&(((TCB_t *)pxIdleTaskTCBBuffer)->xStateListItem));
	/*============= ������������end ================*/
	
	/*�ֶ�ָ����һ�����е�����Ŀǰ����֧�����ȼ����ֶ�ָ��*/
//	pxCurrentTCB = &Task1TCB;

	xNextTaskUnblockTime = portMAX_DELAY;
										
	/*����������*/
	if( xPortStartScheduler() != pdFALSE)
	{
		/*�����������ɹ����򲻻᷵�أ���������������*/
	}
	
}
#if 1
void vTaskSwitchContext(void)
{
	/*�������������л�*/
	taskSELECT_HIGHEST_PRIORITY_TASK();
}
#else
void vTaskSwitchContext(void)
{
	/*
	*/
	if(pxCurrentTCB == &IdleTaskTCB)
	{
		if(Task1TCB.xTicksToDelay == 0)
		{
			pxCurrentTCB = &Task1TCB;
		}
		else if(Task2TCB.xTicksToDelay == 0)
		{
			pxCurrentTCB = &Task2TCB;
		}
		else
		{
			return;  /*������ʱ��û�е����򷵻أ�����ִ�п�������*/
		}
	}
	else         /*��ǰ�����ǿ�������*/
	{
		if(pxCurrentTCB == &Task1TCB)
		{
			if(Task2TCB.xTicksToDelay == 0)
			{
				pxCurrentTCB = &Task2TCB;
			}
			else if(pxCurrentTCB->xTicksToDelay != 0)  /*�ǿ������������vTaskDelay*/
			{
				pxCurrentTCB = &IdleTaskTCB;
			}
			else
			{
				return;  /*���أ��������л�����Ϊ������������ʱ��*/
			}
		}
		else if(pxCurrentTCB == &Task2TCB)
		{
			if(Task1TCB.xTicksToDelay == 0)
			{
				pxCurrentTCB = &Task1TCB;
			}
			else if(pxCurrentTCB->xTicksToDelay != 0)
			{
				pxCurrentTCB = &IdleTaskTCB;
			}
			else
			{
				return;
			}
		}
	}
}
#endif

/*===========��������===========*/
void prvIdleTask(void *p_arg)
{
	while(1)
	{
		
	}
}


static void prvAddCurrentTaskToDelayedList(TickType_t xTicksToWait)
{
	TickType_t xTimeToWake;
	
	/*��ȡϵͳʱ��������xTickCount��ֵ*/
	const TickType_t xConstTickCount = xTickCount;
	
	/*������Ӿ����б����Ƴ�*/
	if( uxListRemove(&(pxCurrentTCB->xStateListItem)) ==(UBaseType_t)0)
	{
		/*�����������ȼ�λͼ�ж�Ӧ��λ���*/
		portRESET_READY_PRIORITY(pxCurrentTCB->uxPriority,uxTopReadyPriority);
	}
	
	/*����������ʱ����ʱ��ϵͳʱ��������xTickCount��ֵ�Ƕ���*/
	xTimeToWake = xConstTickCount + xTicksToWait;
	
	/*����ʱ���ڵ�ֵ����Ϊ�ڵ������ֵ*/
	listSET_LIST_ITEM_VALUE(&(pxCurrentTCB->xStateListItem), xTimeToWake);
	
	/*��������뵽��ʱ�б���*/
	/*���*/
	if(xTimeToWake < xConstTickCount)
	{
		vListInsert(pxOverflowDelayedTaskList, &(pxCurrentTCB->xStateListItem));
	}
	else/*û�����*/
	{
		vListInsert(pxDelayedTaskList, &(pxCurrentTCB->xStateListItem));
		
		/*������һ���������ʱ�̱���xNextTaskUnblockTime��ֵ*/
		if(xTimeToWake < xNextTaskUnblockTime)
		{
			xNextTaskUnblockTime = xTimeToWake;/*ʼ�ձ���Ϊ�����*/
		}
	}
	
}


/*==========����������ʱ============*/
void vTaskDelay(const TickType_t xTicksToDelay)
{
	TCB_t *pxTCB = NULL;
	
	/*��ȡ��ǰ�����TCB*/
	pxTCB = pxCurrentTCB;
	
	/*������ʱʱ��, ��λΪ SysTick ���ж�����*/
	//pxTCB->xTicksToDelay = xTicksToDelay;
	
	/*��������뵽��ʱ�б�*/
	prvAddCurrentTaskToDelayedList(xTicksToDelay);
	/*������Ӿ����б��Ƴ�*/
	//uxListRemove(&(pxTCB->xStateListItem));
	//taskRESET_READY_PRIORITY(pxTCB->uxPriority);
	
	/*����PendSV�ж� �����л�*/
	taskYIELD();
}


/*==========����ϵͳʱ��============*/
BaseType_t xTaskIncrementTick(void) 
{
	TCB_t *pxTCB;
	//BaseType_t i = 0;
	TickType_t xItemValue;
	BaseType_t xSwitchRequired = pdFALSE;
	/*����ϵͳʱ��������xTickCount, ����һ��port.c�е�ȫ�ֱ���*/
	const TickType_t xConstTickCount = xTickCount + 1;
	
	xTickCount = xConstTickCount;
	
  /*���xConstTickCount ��������л���ʱ�б�*/
  if(xConstTickCount == (TickType_t)0U)
	{
		taskSWITCH_DELAYED_LISTS();
	}		
	
	/*�������ʱ������ʱ����*/
	if(xConstTickCount >= xNextTaskUnblockTime)
	{
		for(;;)
		{
			if(listLIST_IS_EMPTY(pxDelayedTaskList)!=pdFALSE)
			{
				/*��ʱ�б�Ϊ�գ�����xNextTaskUnblockTimeΪ���ܵ����ֵ*/
				xNextTaskUnblockTime = portMAX_DELAY;
				break;
			}
			else/*��ʱ�б�Ϊ��*/
			{
				pxTCB = (TCB_t *) listGET_OWNER_OF_HEAD_ENTRY(pxDelayedTaskList);
				xItemValue = listGET_LIST_ITEM_VALUE(&(pxTCB->xStateListItem));
				
				/*ֱ������ʱ�б���������ʱ���ڵ������Ƴ�������forѭ��*/
				if(xConstTickCount < xItemValue)
				{
					xNextTaskUnblockTime = xItemValue;
					break;
				}
				
				/*���������ʱ�б��Ƴ��������ȴ�״̬*/
				(void) uxListRemove(&(pxTCB->xStateListItem));
				
				/*������ȴ���������ӵ������б�*/
				prvAddTaskToReadyList(pxTCB);
				
				
				#if( configUSE_PREEMPTION == 1)
				{
					if(pxTCB->uxPriority >= pxCurrentTCB->uxPriority)
					{
						xSwitchRequired = pdTRUE;
					}
				}
				#endif
			}
		}
	}
	#if( (configUSE_PREEMPTION == 1) && (configUSE_TIME_SLICING == 1))
	{
		if(listCURRENT_LIST_LENGTH(&(pxReadyTasksLists[pxCurrentTCB->uxPriority]))
			                          > (UBaseType_t)1)
		{
			xSwitchRequired = pdTRUE;
		}
	}
	#endif
	
	return xSwitchRequired;
//	/*ɨ������б������������xTicksToDelay�������Ϊ0�����1*/
//	for(i = 0; i < configMAX_PRIORITIES; i++)
//	{
//		pxTCB = (TCB_t *) listGET_OWNER_OF_HEAD_ENTRY( ( &pxReadyTasksLists[i] ) );
//		if(pxTCB->xTicksToDelay > 0)
//		{
//			pxTCB->xTicksToDelay --;
//			
//			/*��ʱʱ�䵽�����������*/
//			
//			if(pxTCB->xTicksToDelay == 0)
//			{
//				taskRECORD_READY_PRIORITY(pxTCB->uxPriority);
//			}
//		}
//	}
	
	/*�����л�*/
	//portYIELD();
}





