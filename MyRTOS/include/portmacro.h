#ifndef __PORT_MACRO_H_
#define __PORT_MACRO_H_

#include "stdint.h" 
#include "stddef.h" 
#include "myRTOSConfig.h"
 
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


#ifndef portFORCE_INLINE
	#define portFORCE_INLINE __forceinline
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

/*不带返回值的关中断函数，不能嵌套，不能在中断里面使用*/
#define portDISABLE_INTERRUPTS() vPortRaiseBASEPRI()

static portFORCE_INLINE void vPortRaiseBASEPRI( void )
{
	uint32_t ulNewBASEPRI = configMAX_SYSCALL_INTERRUPT_PRIORITY;
	__asm
  {
		msr basepri, ulNewBASEPRI
		dsb
		isb
	}
	
}

/*带返回值的关中断函数，可以嵌套，可以在中断里面使用*/
#define portSET_INTERRUPT_MASK_FROM_ISR() ulPortRaiseBASEPRI()
static portFORCE_INLINE uint32_t ulPortRaiseBASEPRI( void )
{
	uint32_t ulReturn, ulNewBASEPRI = configMAX_SYSCALL_INTERRUPT_PRIORITY;
	
	__asm
	{
		mrs ulReturn, basepri
		msr basepri, ulNewBASEPRI
		dsb
		isb
	}
	return ulReturn;
}

/*不带中断保护的开中断函数*/
#define portENABLE_INTERRUPTS() vPortSetBASEPRI( 0 )

/*带中断保护的开中断函数*/
#define portCLEAR_INTERRUPT_MASK_FROM_ISR(x) vPOrtSetBASEPRI(x)

static portFORCE_INLINE void vPortSetBASEPRI(uint32_t ulBASEPRI)
{
	__asm
	{
		msr basepri, ulBASEPRI
	}
}

static portFORCE_INLINE void vPortClearBASEPRIFromISR( void )
{
	__asm
	{
		/* Set BASEPRI to 0 so no interrupts are masked.  This function is only
		used to lower the mask in an interrupt, so memory barriers are not 
		used. */
		msr basepri, #0
	}
}

#endif
