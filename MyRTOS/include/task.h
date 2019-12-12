#ifndef __TASK_H_
#define __TASK_H_
#include "portmacro.h"
#include "list.h"
#include "myRTOSConfig.h"
#include "projdefs.h"

#define taskYIELD()        portYIELD()

//进入和退出临界段
#define taskENTER_CRITICAL()             portENTER_CRITICAL()
#define taskENTER_CRITICAL_FROM_ISR()    portSET_INTERRUPT_MASK_FROM_ISR()  
#define taskEXIT_CRITICAL()              portEXIT_CRITICAL()
#define taskEXIT_CRITICAL_FROM_ISR(x)    portCLEAR_INTERRUPT_MASK_FROM_ISR(x)


typedef void * TaskHandle_t;

typedef struct tskTaskControlBlock
{
	volatile StackType_t  *pxTopOfStack;  /*栈顶*/
	ListItem_t            xStateListItem; /*任务节点*/
	StackType_t           *pxStack;       /*任务栈起始地址*/
	char                  pcTaskName[ configMAX_TASK_NAME_LEN ];  /*任务名字字符串*/
	TickType_t            xTicksToDelay;  /*用于延时*/
}tskTCB;

typedef tskTCB TCB_t;

extern List_t pxReadyTasksLists[configMAX_PRIORITIES];

void prvInitialiseTaskLists(void);
TaskHandle_t xTaskCreateStatic( TaskFunction_t pxTaskCode,
                                const char * const pcName,
																const uint32_t ulStackDepth,
																void * const pvParameters,
																StackType_t * const puxStackBuffer,
																TCB_t * const pxTaskBuffer);

void vTaskSwitchContext(void);	
void vTaskStartScheduler( void );		
void xTaskIncrementTick(void);																
void vTaskDelay(const TickType_t xTicksToDelay);																
#endif
																
																
																