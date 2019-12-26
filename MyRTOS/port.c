#include "portable.h"
#include "myRTOSConfig.h"
#include "task.h"

#define portINITIAL_XPSR					(0x01000000)
#define portSTART_ADDRESS_MASK    ( ( StackType_t ) 0xfffffffeUL)
/* Masks off all bits but the VECTACTIVE bits in the ICSR register. */
#define portVECTACTIVE_MASK					( 0xFFUL )

TickType_t xTickCount;

/* Each task maintains its own interrupt status in the critical nesting
variable. */
static UBaseType_t uxCriticalNesting = 0xaaaaaaaa;

void vPortSetupTimerInterrupt(void);
void prvStartFirstTask( void );

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

#define portNVIC_SYSPRI2_REG			(*((volatile uint32_t *)0xe000ed20))
#define portNVIC_PENDSV_PRI			(((uint32_t) configKERNEL_INTERRUPT_PRIORITY ) << 16UL)
#define portNVIC_SYSTICK_PRI     (((uint32_t) configKERNEL_INTERRUPT_PRIORITY) << 24UL)

BaseType_t xPortStartScheduler(void)
{
	/* 配置PendSV 和 SysTick的中断优先级为最低 */
	portNVIC_SYSPRI2_REG |= portNVIC_PENDSV_PRI;
	portNVIC_SYSPRI2_REG |= portNVIC_SYSTICK_PRI;
	
	/*初始化SysTick*/
	vPortSetupTimerInterrupt();
	
	/*启动第一个任务，不再返回*/
	prvStartFirstTask();
	
	
	/* Initialise the critical nesting count ready for the first task. */
	uxCriticalNesting = 0;
	
	/*不应该运行到这里*/
	return 0;
}

/*
prvStartFirstTask()函数用于开始第一个任务，主要做了两个动作，
一个是更新 MSP 的值，二是产生 SVC 系统调用，然后去到 SVC 的
中断服务函数里面真正切换到第一个任务。
*/
__asm void prvStartFirstTask( void )
{
	PRESERVE8			/*当前栈按照8字节对齐，如果都是32的操作则4字节对齐也可*/
	/*在Cortex-M中， 0xE000ED08是SC_VTOR这个寄存器的地址，
	  里面存放的是向量表的起始地址，即MSP的地址
	*/
	ldr r0, =0xE000ED08		  /*将0XE000ED08这个立即数加载到寄存器R0*/
	ldr r0, [r0]            /*将E000ED08这个地址指向的内容加载到寄存器， SCB_VTOR 寄存器里的内容 */
	ldr r0, [r0]            /*将中断向量表的地址加载r0*/
	
	/*设置主堆栈指针msp的值*/
	msr msp, r0             
	
	/*使能全局中断*/
	cpsie i
	cpsie f
	dsb
	isb
	
	/*调用SVC去启动第一个任务*/
	/*The SVC instruction causes the SVC exception. 
	imm is ignored by the processor*/
	svc 0	               //后面的立即数在操作系统里可以用作参数
	nop
	nop	
	
}

//
__asm void vPortSVCHandler( void )
{
	//声明外部变量，指向当前正在运行或即将要运行的程序
	extern pxCurrentTCB;			
	
	PRESERVE8
	
	ldr r3, =pxCurrentTCB /*地址加载到r3*/
	ldr r1, [r3]          /*加载pxCurrentTCB到r1*/
	ldr r0, [r1]          /*加载栈顶指针到r0*/
	ldmia r0!, {r4-r11}   /*以r0为基地址，向上加载8个字的内容到r4~r11*/
	msr psp, r0					  /*将新的栈顶指针r0更新到psp,任务执行的时候使用的堆栈指针是psp*/	
	isb                   /**/
	mov r0, #0						/*寄存器r0清零*/
	msr basepri, r0       /*打开所有中断*/
	orr r14, #0xd         /*通过向r14寄存器最后4位按位或0x0D，
													使得硬件在退出时使用进程堆栈指针PSP完成出栈操作并
	                        返回后进入任务模式，返回Thumb状态
	                        在SVC中断服务里，使用的MSP堆栈指针，处在ARM状态*/
	
	bx r14                /*异常返回，出栈使用PSP指针，自动将栈中的剩下的
	                       内容加载到CPU寄存器*/
}


/*
PendSV 中断服务函数是真正实现任务切换的地方
*/
__asm void xPortPendSVHandler(void)
{
	extern pxCurrentTCB;
	extern vTaskSwitchContext;
	
	PRESERVE8                   /*当前栈需按照 8 字节对齐*/
	
	/*将psp的值存入r0 
	  当进入PendSVC Handler时，上一个任务
    运行的环境即：	xPSR, PC, R14, R12, R3, R2, R1, R0
	  这些CPU寄存器的值会自动存储到任务的栈中，剩下的
	  r4-r11需要手动保存，同时PSP会自动更新
	*/
	mrs r0, psp                /*将PSP的值存入r0*/
	isb
	
	ldr r3, =pxCurrentTCB      /*获取pxCurrentTCB地址*/
	ldr r2, [r3]               /*将pxcurrentTCB存入到r2中*/
	
	
	/*
	  以r0作为基址，DB  decrease before
	  将CPU寄存器r4-r11的值存储到任务栈,
		同时更新r0的值
	*/
	stmdb r0!, {r4-r11}        /*r0里的值是任务栈栈顶*/
	str   r0, [r2]             /*更新任务的栈顶指针,r2指向任务栈栈顶*/
	
	stmdb sp!, {r3, r14}       /*中断使用主堆栈MSP，因为接下来要调用vTaskSwitchContext
	                            调用时，返回地址自动保存到R14中，所以一旦调用发生，R14 的值会被覆盖
	                            因此需要入栈保护
	                            R3保存的是当前正在运行任务的TCB指针地址,函数调用后，pxCurrentTCB的值会被更新
	                            为新任务的TCB。运行函数 vTaskSwitchContext 时不确定会不会使用 R3 
	                            寄存器作为中间变量
	                            因此，保险起见，也入栈保护*/ 
	mov r0, #configMAX_SYSCALL_INTERRUPT_PRIORITY
	msr basepri, r0             /*屏蔽定义好的最大优先级以下的所有中断，进入临界段*/
	dsb
	isb
	bl vTaskSwitchContext       /*切换任务*/
	mov r0, #0
	msr basepri, r0             /*任务切换完成，开中断*/
	ldmia sp!, {r3, r14}        /*将r3和r14的值出栈*/
	
	ldr r1, [r3]                /*现在pxCurrentTCB已经指向了新的TCB*/
	ldr r0, [r1]                /*将新任务的栈顶指针加载到r0*/
	ldmia r0!, {r4-r11}         /*新任务的r4-r11手动出栈*/
  msr psp, r0							    /*因为没有加这一语句，导致最开始切换失败*/  
	                            /*更新psp为新任务的栈顶指针，接下来跳出中断服务函数时
	                              CPU会自动依次加载r0,r1,r2,r3,r12,r14,r15,xPSR
	                             */
	isb
	bx  r14                     /*跳出中断服务函数，运行任务*/
	nop
	
}


/*===========进入临界段，不带中断保护，不能嵌套=====================*/
void vPortEnterCritical( void )
{
	portDISABLE_INTERRUPTS();
	uxCriticalNesting++;
	
  /*如果 uxCriticalNesting 等于 1，即一层嵌套，要确保当前没有中断活
  跃，即内核外设 SCB 中的中断和控制寄存器 SCB_ICSR 的低 8 位要等于 0 */
	if(uxCriticalNesting == 1)
	{ 
		configASSERT((portNVIC_INT_CTRL_REG & portVECTACTIVE_MASK) == 0);
	}
}

/*=================退出临界段，不带中断保护，不可以嵌套=================*/
void vPortExitCritical( void )
{
	configASSERT( uxCriticalNesting );
	uxCriticalNesting--;
	if(uxCriticalNesting == 0)
	{
		portENABLE_INTERRUPTS();
	}
}


/*SysTick控制寄存器*/
#define portNVIC_SYSTICK_CTRL_REG  (*((volatile uint32_t *)0xe000e010))
/*SysTick重装载寄存器*/
#define portNVIC_SYSTICK_LOAD_REG  (*((volatile uint32_t *)0xe000e014))
	
/*SysTick时钟源选择*/
#ifndef configSYSTICK_CLOCK_HZ
   #define configSYSTICK_CLOCK_HZ configCPU_CLOCK_HZ
   #define portNVIC_SYSTICK_CLK_BIT    (1UL << 2UL)
#else
   #define portNVIC_SYSTICK_CLK_BIT    ( 0 )
#endif

#define portNVIC_SYSTICK_INT_BIT 						( 1UL << 1UL )
#define portNVIC_SYSTICK_ENABLE_BIT         ( 1UL << 0UL )

void vPortSetupTimerInterrupt(void)
{
	/*设置重装载寄存器的值*/
	portNVIC_SYSTICK_LOAD_REG = ( configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ ) - 1UL;
	
	/*
	设置系统定时器的时钟等于内核时钟
	使能SysTick定时器中断
	使能SysTick定时器
	*/
	portNVIC_SYSTICK_CTRL_REG = (portNVIC_SYSTICK_CLK_BIT |
	                             portNVIC_SYSTICK_INT_BIT |
	                             portNVIC_SYSTICK_ENABLE_BIT);
}


/*==========中断服务函数=============*/
void xPortSysTickHandler(void)
{
	/*关中断*/
	vPortRaiseBASEPRI();
	
	/*更新系统时基*/
	if(xTaskIncrementTick() != pdFALSE)
	{
		/* 任务切换，触发PendSV */
		taskYIELD();
	}
	
	/*开中断*/
	vPortClearBASEPRIFromISR();
}











