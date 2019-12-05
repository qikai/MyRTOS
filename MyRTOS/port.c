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
