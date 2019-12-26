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
	  �쳣����ʱ���Զ����ص�CPU�Ĵ��������� 
	  ���� 8���Ĵ������ֱ�Ϊ R0��R1��R2��R3��
	   R12��R14��R15 �� xPSR ��λ 24����˳���ܱ�
	*/
	pxTopOfStack--;
	*pxTopOfStack = portINITIAL_XPSR;			/*xPSR �� bit24 ������ 1*/
	pxTopOfStack--;
	*pxTopOfStack = ( ( StackType_t ) pxCode ) & portSTART_ADDRESS_MASK; /*�������ڵ�ַ*/
	pxTopOfStack--;
	/*����ķ��ص�ַ��ͨ�������ǲ��᷵�صģ���������˾���ת��prvTaskExitError*/
	*pxTopOfStack = ( StackType_t ) prvTaskExitError; 
	pxTopOfStack -= 5; /* R12, R3, R2 and R1Ĭ�ϳ�ʼ��Ϊ0 */
	*pxTopOfStack = ( StackType_t ) pvParameters;

	/* �쳣����ʱ���ֶ����ص�CPU�Ĵ��������� */
	pxTopOfStack -= 8;  /*�����һ������ʱ�����Ǵ����ջָ�뿪ʼ�ֶ����� 8 ���ֵ����ݵ� CPU �Ĵ���*/
	
	/*����ջ��ָ�룬��ʱpxTopOfStackָ�����ջ*/
	return pxTopOfStack;  
}

#define portNVIC_SYSPRI2_REG			(*((volatile uint32_t *)0xe000ed20))
#define portNVIC_PENDSV_PRI			(((uint32_t) configKERNEL_INTERRUPT_PRIORITY ) << 16UL)
#define portNVIC_SYSTICK_PRI     (((uint32_t) configKERNEL_INTERRUPT_PRIORITY) << 24UL)

BaseType_t xPortStartScheduler(void)
{
	/* ����PendSV �� SysTick���ж����ȼ�Ϊ��� */
	portNVIC_SYSPRI2_REG |= portNVIC_PENDSV_PRI;
	portNVIC_SYSPRI2_REG |= portNVIC_SYSTICK_PRI;
	
	/*��ʼ��SysTick*/
	vPortSetupTimerInterrupt();
	
	/*������һ�����񣬲��ٷ���*/
	prvStartFirstTask();
	
	
	/* Initialise the critical nesting count ready for the first task. */
	uxCriticalNesting = 0;
	
	/*��Ӧ�����е�����*/
	return 0;
}

/*
prvStartFirstTask()�������ڿ�ʼ��һ��������Ҫ��������������
һ���Ǹ��� MSP ��ֵ�����ǲ��� SVC ϵͳ���ã�Ȼ��ȥ�� SVC ��
�жϷ��������������л�����һ������
*/
__asm void prvStartFirstTask( void )
{
	PRESERVE8			/*��ǰջ����8�ֽڶ��룬�������32�Ĳ�����4�ֽڶ���Ҳ��*/
	/*��Cortex-M�У� 0xE000ED08��SC_VTOR����Ĵ����ĵ�ַ��
	  �����ŵ������������ʼ��ַ����MSP�ĵ�ַ
	*/
	ldr r0, =0xE000ED08		  /*��0XE000ED08������������ص��Ĵ���R0*/
	ldr r0, [r0]            /*��E000ED08�����ַָ������ݼ��ص��Ĵ����� SCB_VTOR �Ĵ���������� */
	ldr r0, [r0]            /*���ж�������ĵ�ַ����r0*/
	
	/*��������ջָ��msp��ֵ*/
	msr msp, r0             
	
	/*ʹ��ȫ���ж�*/
	cpsie i
	cpsie f
	dsb
	isb
	
	/*����SVCȥ������һ������*/
	/*The SVC instruction causes the SVC exception. 
	imm is ignored by the processor*/
	svc 0	               //������������ڲ���ϵͳ�������������
	nop
	nop	
	
}

//
__asm void vPortSVCHandler( void )
{
	//�����ⲿ������ָ��ǰ�������л򼴽�Ҫ���еĳ���
	extern pxCurrentTCB;			
	
	PRESERVE8
	
	ldr r3, =pxCurrentTCB /*��ַ���ص�r3*/
	ldr r1, [r3]          /*����pxCurrentTCB��r1*/
	ldr r0, [r1]          /*����ջ��ָ�뵽r0*/
	ldmia r0!, {r4-r11}   /*��r0Ϊ����ַ�����ϼ���8���ֵ����ݵ�r4~r11*/
	msr psp, r0					  /*���µ�ջ��ָ��r0���µ�psp,����ִ�е�ʱ��ʹ�õĶ�ջָ����psp*/	
	isb                   /**/
	mov r0, #0						/*�Ĵ���r0����*/
	msr basepri, r0       /*�������ж�*/
	orr r14, #0xd         /*ͨ����r14�Ĵ������4λ��λ��0x0D��
													ʹ��Ӳ�����˳�ʱʹ�ý��̶�ջָ��PSP��ɳ�ջ������
	                        ���غ��������ģʽ������Thumb״̬
	                        ��SVC�жϷ����ʹ�õ�MSP��ջָ�룬����ARM״̬*/
	
	bx r14                /*�쳣���أ���ջʹ��PSPָ�룬�Զ���ջ�е�ʣ�µ�
	                       ���ݼ��ص�CPU�Ĵ���*/
}


/*
PendSV �жϷ�����������ʵ�������л��ĵط�
*/
__asm void xPortPendSVHandler(void)
{
	extern pxCurrentTCB;
	extern vTaskSwitchContext;
	
	PRESERVE8                   /*��ǰջ�谴�� 8 �ֽڶ���*/
	
	/*��psp��ֵ����r0 
	  ������PendSVC Handlerʱ����һ������
    ���еĻ�������	xPSR, PC, R14, R12, R3, R2, R1, R0
	  ��ЩCPU�Ĵ�����ֵ���Զ��洢�������ջ�У�ʣ�µ�
	  r4-r11��Ҫ�ֶ����棬ͬʱPSP���Զ�����
	*/
	mrs r0, psp                /*��PSP��ֵ����r0*/
	isb
	
	ldr r3, =pxCurrentTCB      /*��ȡpxCurrentTCB��ַ*/
	ldr r2, [r3]               /*��pxcurrentTCB���뵽r2��*/
	
	
	/*
	  ��r0��Ϊ��ַ��DB  decrease before
	  ��CPU�Ĵ���r4-r11��ֵ�洢������ջ,
		ͬʱ����r0��ֵ
	*/
	stmdb r0!, {r4-r11}        /*r0���ֵ������ջջ��*/
	str   r0, [r2]             /*���������ջ��ָ��,r2ָ������ջջ��*/
	
	stmdb sp!, {r3, r14}       /*�ж�ʹ������ջMSP����Ϊ������Ҫ����vTaskSwitchContext
	                            ����ʱ�����ص�ַ�Զ����浽R14�У�����һ�����÷�����R14 ��ֵ�ᱻ����
	                            �����Ҫ��ջ����
	                            R3������ǵ�ǰ�������������TCBָ���ַ,�������ú�pxCurrentTCB��ֵ�ᱻ����
	                            Ϊ�������TCB�����к��� vTaskSwitchContext ʱ��ȷ���᲻��ʹ�� R3 
	                            �Ĵ�����Ϊ�м����
	                            ��ˣ����������Ҳ��ջ����*/ 
	mov r0, #configMAX_SYSCALL_INTERRUPT_PRIORITY
	msr basepri, r0             /*���ζ���õ�������ȼ����µ������жϣ������ٽ��*/
	dsb
	isb
	bl vTaskSwitchContext       /*�л�����*/
	mov r0, #0
	msr basepri, r0             /*�����л���ɣ����ж�*/
	ldmia sp!, {r3, r14}        /*��r3��r14��ֵ��ջ*/
	
	ldr r1, [r3]                /*����pxCurrentTCB�Ѿ�ָ�����µ�TCB*/
	ldr r0, [r1]                /*���������ջ��ָ����ص�r0*/
	ldmia r0!, {r4-r11}         /*�������r4-r11�ֶ���ջ*/
  msr psp, r0							    /*��Ϊû�м���һ��䣬�����ʼ�л�ʧ��*/  
	                            /*����pspΪ�������ջ��ָ�룬�����������жϷ�����ʱ
	                              CPU���Զ����μ���r0,r1,r2,r3,r12,r14,r15,xPSR
	                             */
	isb
	bx  r14                     /*�����жϷ���������������*/
	nop
	
}


/*===========�����ٽ�Σ������жϱ���������Ƕ��=====================*/
void vPortEnterCritical( void )
{
	portDISABLE_INTERRUPTS();
	uxCriticalNesting++;
	
  /*��� uxCriticalNesting ���� 1����һ��Ƕ�ף�Ҫȷ����ǰû���жϻ�
  Ծ�����ں����� SCB �е��жϺͿ��ƼĴ��� SCB_ICSR �ĵ� 8 λҪ���� 0 */
	if(uxCriticalNesting == 1)
	{ 
		configASSERT((portNVIC_INT_CTRL_REG & portVECTACTIVE_MASK) == 0);
	}
}

/*=================�˳��ٽ�Σ������жϱ�����������Ƕ��=================*/
void vPortExitCritical( void )
{
	configASSERT( uxCriticalNesting );
	uxCriticalNesting--;
	if(uxCriticalNesting == 0)
	{
		portENABLE_INTERRUPTS();
	}
}


/*SysTick���ƼĴ���*/
#define portNVIC_SYSTICK_CTRL_REG  (*((volatile uint32_t *)0xe000e010))
/*SysTick��װ�ؼĴ���*/
#define portNVIC_SYSTICK_LOAD_REG  (*((volatile uint32_t *)0xe000e014))
	
/*SysTickʱ��Դѡ��*/
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
	/*������װ�ؼĴ�����ֵ*/
	portNVIC_SYSTICK_LOAD_REG = ( configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ ) - 1UL;
	
	/*
	����ϵͳ��ʱ����ʱ�ӵ����ں�ʱ��
	ʹ��SysTick��ʱ���ж�
	ʹ��SysTick��ʱ��
	*/
	portNVIC_SYSTICK_CTRL_REG = (portNVIC_SYSTICK_CLK_BIT |
	                             portNVIC_SYSTICK_INT_BIT |
	                             portNVIC_SYSTICK_ENABLE_BIT);
}


/*==========�жϷ�����=============*/
void xPortSysTickHandler(void)
{
	/*���ж�*/
	vPortRaiseBASEPRI();
	
	/*����ϵͳʱ��*/
	if(xTaskIncrementTick() != pdFALSE)
	{
		/* �����л�������PendSV */
		taskYIELD();
	}
	
	/*���ж�*/
	vPortClearBASEPRIFromISR();
}











