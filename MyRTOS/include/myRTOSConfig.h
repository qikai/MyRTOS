#ifndef __MYRTOS_CONFIG_H_
#define __MYRTOS_CONFIG_H_

#define configUSE_16_BIT_TICKS   0

#define configMAX_TASK_NAME_LEN  16		/*�������ֳ���*/

#define configSUPPORT_STATIC_ALLOCATION  1

#define configMAX_PRIORITIES		5			/*����������ȼ�*/

#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY			15    //�ж�������ȼ�
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY	5   //ϵͳ�ɹ��������ж����ȼ�
#define configKERNEL_INTERRUPT_PRIORITY		(configLIBRARY_LOWEST_INTERRUPT_PRIORITY<<4)
#define configMAX_SYSCALL_INTERRUPT_PRIORITY ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << 4)

#define xPortPendSVHandler   PendSV_Handler 
#define xPortSysTickHandler  SysTick_Handler 
#define vPortSVCHandler      SVC_Handler

#endif /*__MYRTOS_CONFIG_H_*/
