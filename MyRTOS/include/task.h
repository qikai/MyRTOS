#ifndef __TASK_H_
#define __TASK_H_
#include "portmacro.h"
#include "list.h"
#include "myRTOSConfig.h"
#include "projdefs.h"

#define taskYIELD()        portYIELD()

typedef void * TaskHandle_t;

typedef struct tskTaskControlBlock
{
	volatile StackType_t  *pxTopOfStack;  /*ջ��*/
	ListItem_t            xStateListItem; /*����ڵ�*/
	StackType_t           *pxStack;       /*����ջ��ʼ��ַ*/
	char                  pcTaskName[ configMAX_TASK_NAME_LEN ];  /*���������ַ���*/
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
																
#endif
																
																
																