#ifndef __MYRTOS_CONFIG_H_
#define __MYRTOS_CONFIG_H_

#define configUSE_16_BIT_TICKS   0

#define configMAX_TASK_NAME_LEN  16		/*�������ֳ���*/

#define configSUPPORT_STATIC_ALLOCATION  1

#define configMAX_PRIORITIES		5			/*����������ȼ�*/

#define configUSE_PORT_OPTIMISED_TASK_SELECTION  1

#define configUSE_PREEMPTION    1

#define configUSE_TIME_SLICING	1

#define configMINIMAL_STACK_SIZE       ( ( unsigned short ) 128 )
	
#define configCPU_CLOCK_HZ						(( unsigned long ) 25000000)
#define configTICK_RATE_HZ            (( TickType_t ) 100)

#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY			15    //�ж�������ȼ�
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY	5   //ϵͳ�ɹ��������ж����ȼ�
#define configKERNEL_INTERRUPT_PRIORITY		(configLIBRARY_LOWEST_INTERRUPT_PRIORITY<<4)
#define configMAX_SYSCALL_INTERRUPT_PRIORITY ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << 4)

#define xPortPendSVHandler   PendSV_Handler 
#define xPortSysTickHandler  SysTick_Handler 
#define vPortSVCHandler      SVC_Handler

/* ���� */
#define vAssertCalled(char,int) printf("Error:%s,%d\r\n",char,int)
#define configASSERT(x) if((x)==0) vAssertCalled(__FILE__,__LINE__)

#endif /*__MYRTOS_CONFIG_H_*/
