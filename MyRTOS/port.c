#include "portable.h"

#define portINITIAL_XPSR					(0x01000000)
#define portSTART_ADDRESS_MASK    ( ( StackType_t ) 0xfffffffeUL)

static void prvTaskExitError( void )
{
	for(;;);
}

StackType_t *pxPortInitialiseStack(StackType_t *pxTopOfStack,
                                   TaskFunction_t pxCode,
                                   void *pvParameters)
{
	/* 
	  异常发生时，自动加载到CPU寄存器的内容 
	  包括 8个寄存器，分别为 R0、R1、R2、R3、
	   R12、R14、R15 和 xPSR 的位 24，且顺序不能变
	*/
	pxTopOfStack--;
	*pxTopOfStack = portINITIAL_XPSR;			/*xPSR 的 bit24 必须置 1*/
	pxTopOfStack--;
	*pxTopOfStack = ( ( StackType_t ) pxCode ) & portSTART_ADDRESS_MASK; /*任务的入口地址*/
	pxTopOfStack--;
	/*任务的返回地址，通常任务是不会返回的，如果返回了就跳转到prvTaskExitError*/
	*pxTopOfStack = ( StackType_t ) prvTaskExitError; 
	pxTopOfStack -= 5; /* R12, R3, R2 and R1默认初始化为0 */
	*pxTopOfStack = ( StackType_t ) pvParameters;

	/* 异常发生时，手动加载到CPU寄存器的内容 */
	pxTopOfStack -= 8;  /*任务第一次运行时，就是从这个栈指针开始手动加载 8 个字的内容到 CPU 寄存器*/
	
	/*返回栈顶指针，此时pxTopOfStack指向空闲栈*/
	return pxTopOfStack;  
}
