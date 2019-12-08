#ifndef __MYRTOS_CONFIG_H_
#define __MYRTOS_CONFIG_H_

#define configUSE_16_BIT_TICKS   0

#define configMAX_TASK_NAME_LEN  16		/*任务名字长度*/

#define configSUPPORT_STATIC_ALLOCATION  1

#define configMAX_PRIORITIES		5			/*任务最大优先级*/

#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY			15    //中断最低优先级
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY	5   //系统可管理的最高中断优先级
#define configKERNEL_INTERRUPT_PRIORITY		(configLIBRARY_LOWEST_INTERRUPT_PRIORITY<<4)
#define configMAX_SYSCALL_INTERRUPT_PRIORITY ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << 4)

#define xPortPendSVHandler   PendSV_Handler 
#define xPortSysTickHandler  SysTick_Handler 
#define vPortSVCHandler      SVC_Handler

#endif /*__MYRTOS_CONFIG_H_*/
