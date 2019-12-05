#ifndef __TASK_H_
#define __TASK_H_
#include "portmacro.h"
#include "list.h"
#include "myRTOSConfig.h"

typedef void * TaskHandle_t;

typedef struct tskTaskControlBlock
{
	volatile StackType_t  *pxTopOfStack;  /*栈顶*/
	ListItem_t            xStateListItem; /*任务节点*/
	StackType_t           *pxStack;       /*任务栈起始地址*/
	char                  pcTaskName[ configMAX_TASK_NAME_LEN ];  /*任务名字字符串*/
}tskTCB;

typedef tskTCB TCB_t;


#endif