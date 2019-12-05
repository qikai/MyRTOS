#include "task.h"
#include "projdefs.h"
#include "list.h"
#include "portable.h"

TCB_t Task1TCB; 
TCB_t Task2TCB;

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
	
	/*��ȡջ����ַ*/
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








