#include "task.h"
#include "list.h"
#include "portable.h"

/*ȫ��ָ�룬����ָ��ǰ�������л��߼���Ҫ���е������������ƿ�*/
TCB_t *pxCurrentTCB=NULL;  

extern TCB_t Task1TCB; 
extern TCB_t Task2TCB;
 
List_t pxReadyTasksLists[configMAX_PRIORITIES];

void prvInitialiseNewtask( TaskFunction_t pxTaskCode,
	                                const char * const pcName,
																	const uint32_t ulStackDepth,
																	void * const pvParameters,
																	TaskHandle_t * const pxCreatedTask,
																	TCB_t *pxNewTCB );


#if( configSUPPORT_STATIC_ALLOCATION == 1)
TaskHandle_t xTaskCreateStatic( TaskFunction_t pxTaskCode,
                                const char * const pcName,
																const uint32_t ulStackDepth,
																void * const pvParameters,
																StackType_t * const puxStackBuffer,
																TCB_t * const pxTaskBuffer)
{
	TCB_t *pxNewTCB;
	TaskHandle_t xReturn;
	
	if( ( pxTaskBuffer  != NULL ) && ( puxStackBuffer != NULL ))
	{
		pxNewTCB = ( TCB_t *) pxTaskBuffer;
		pxNewTCB->pxStack = ( StackType_t * ) puxStackBuffer;
		
		/*�����µ�����*/
		prvInitialiseNewtask(pxTaskCode,	       /*�������*/
		                      pcName,            /*�������ƣ��ַ�����ʽ*/
		                      ulStackDepth,      /*����ջ��С����λΪ��*/
		                      pvParameters,      /*�����β�*/
		                      &xReturn,          /*������*/
		                      pxNewTCB);         /*����ջ��ʼ��ַ*/
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
}


//=================================================
//								������
//=================================================

//����������
void vTaskStartScheduler( void )
{
	/*�ֶ�ָ����һ�����е�����Ŀǰ����֧�����ȼ����ֶ�ָ��*/
	pxCurrentTCB = &Task1TCB;
	
	/*����������*/
	if( xPortStartScheduler() != pdFALSE)
	{
		/*�����������ɹ����򲻻᷵�أ���������������*/
	}
}

void vTaskSwitchContext(void)
{
	/*�������������л�*/
	if( pxCurrentTCB == &Task1TCB)
	{
		pxCurrentTCB = &Task2TCB;
	}
	else
	{
		pxCurrentTCB = &Task1TCB;
	}
}






