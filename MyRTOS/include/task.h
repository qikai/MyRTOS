#ifndef __TASK_H_
#define __TASK_H_
#include "portmacro.h"
#include "list.h"
#include "myRTOSConfig.h"

typedef void * TaskHandle_t;

typedef struct tskTaskControlBlock
{
	volatile StackType_t  *pxTopOfStack;  /*ջ��*/
	ListItem_t            xStateListItem; /*����ڵ�*/
	StackType_t           *pxStack;       /*����ջ��ʼ��ַ*/
	char                  pcTaskName[ configMAX_TASK_NAME_LEN ];  /*���������ַ���*/
}tskTCB;

typedef tskTCB TCB_t;


#endif