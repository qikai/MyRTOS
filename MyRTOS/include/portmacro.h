#ifndef __PORT_MACRO_H_
#define __PORT_MACRO_H_

#include "stdint.h" 
#include "stddef.h" 
 
 
/* 数据类型重定义 */ 
#define portCHAR    char 
#define portFLOAT    float 
#define portDOUBLE    double 
#define portLONG    long 
#define portSHORT    short 
#define portSTACK_TYPE uint32_t 
#define portBASE_TYPE  long 
 
typedef portSTACK_TYPE StackType_t; 
typedef long BaseType_t; 
typedef unsigned long UBaseType_t; 
 
#if( configUSE_16_BIT_TICKS == 1 )        
typedef uint16_t TickType_t; 
#define portMAX_DELAY ( TickType_t ) 0xffff 
#else 
typedef uint32_t TickType_t; 
#define portMAX_DELAY ( TickType_t ) 0xffffffffUL 
#endif 


/*
中断控制状态寄存器：0xe000ed04
BIT 28 PENDSVSET:PendSV 悬起位
*/
#define portNVIC_INT_CTRL_REG		(*(( volatile uint32_t *) 0xe000ed04))
#define portNVIC_PENDSVSET_BIT   (1UL << 28UL)

#define portSY_FULL_READ_WRITE   ( 15 )


/*
：portYIELD 的实现很简单，实际就是将 PendSV 的悬起位置 1，当
没有其它中断运行的时候响应 PendSV 中断，去执行我们写好的 PendSV 
中断服务函数，在里面实现任务切换。
*/
#define portYIELD()                                     \
{                                                       \
	/*触发PendSV, 产生上下文切换*/                         \
	portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT;       \
	__dsb( portSY_FULL_READ_WRITE );                      \
	__isb( portSY_FULL_READ_WRITE );                      \
}




#endif
