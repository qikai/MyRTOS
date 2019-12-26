#ifndef __LIST_H_
#define __LIST_H_

#include "portmacro.h"


/*������ڵ�ṹ�嶨�� */
struct xMINI_LIST_ITEM
{
    TickType_t xItemvalue;											/*����ֵ�����ڰ����ڵ�����������*/
	  struct xLIST_ITEM * pxNext;									/*ָ��������һ���ڵ�*/
	  struct xLIST_ITEM * pxPrevious;             /*ָ������ǰһ���ڵ�*/
};
typedef struct xMINI_LIST_ITEM MiniListItem_t;	/*����ڵ����������ض���*/

/* ����ڵ����ݽṹ���� */
struct xLIST_ITEM
{
    TickType_t             xItemValue;					/*����ֵ�����ڰ����ڵ���˳������*/
    struct xLIST_ITEM *    pxNext;							/*ָ��������һ���ڵ�*/
    struct xLIST_ITEM *    pxPrevious;					/*ָ������ǰһ���ڵ�*/
	  void  *  pvOwner;                           /*ָ��ӵ�иýڵ���ں˶���ͨ����TCP*/
    void  *  pvContainer;                       /*ָ��ýڵ����ڵ�����*/
};

typedef struct xLIST_ITEM ListItem_t;					  /*�ڵ����������ض���*/


/* ������ڵ����ݽṹ���� */
typedef struct xLIST
{
	UBaseType_t uxNumberOfItems;					        /*����ڵ�����������ڱ�ʾ���������ж��ٸ��ڵ㣬���ڵ����*/
	ListItem_t * pxIndex;									        /*����ڵ�����ָ��*/
	MiniListItem_t xListEnd;							        /*�������һ���ڵ� Ҳ���ǵ�һ���ڵ�*/
}List_t;




/*
	�ڵ���κ꺯��
*/

#define listGET_OWNER_OF_HEAD_ENTRY( pxList )\
        ( ( &( ( pxList )->xListEnd ) )->pxNext->pvOwner )

/*��ʼ���ڵ�ӵ����*/
#define listSET_LIST_ITEM_OWNER( pxListItem, pxOwner )\
				( ( pxListItem )->pvOwner = ( void * ) ( pxOwner ) )
	
/*��ȡ�ڵ�ӵ����*/
#define listGET_LIST_ITEM_OWNER( pxListItem)\
			  ( ( pxListItem )->pvOwner )
				
/*��ʼ���ڵ�������ֵ*/
#define listSET_LIST_ITEM_VALUE( pxListItem, xValue )\
		    ( ( pxListItem )->xItemValue = ( xValue ) )

/*��ȡ�ڵ�������ֵ*/
#define listGET_LIST_ITEM_VALUE( pxListItem )\
				( ( pxListItem )->xItemValue )
				
/*��ȡ������ڵ�Ľڵ��������ֵ*/
#define listGET_ITEM_VALUE_OF_HEAD_ENTRY( pxList )\
        ( ( ( pxList)->xListEnd ).pxNext->xItemValue )				

/*��ȡ�������ڽڵ�*/
#define listGET_HEAD_ENTRY( pxList )\
        ( ( ( pxList )->xListEnd ).pxNext )
				
/*��ȡ�ڵ����һ���ڵ�*/				
#define listGET_NEXT( pxListItem )\
        ( ( pxListItem )->pxNext )
				
/*��ȡ��������һ���ڵ�*/
#define listGET_END_MARDER( pxList )\
        ( ( ListItem_t cont *) ( &( (pxList)->xListEnd ) ) )
				
				
/*�ж������Ƿ�Ϊ��*/			
#define listLIST_IS_EMPTY( pxList )\
        ( ( BaseType_t) ( ( pxList )->uxNumberOfItems == (UBaseType_t )0 ) )


/*��ȡ����Ľڵ���*/
#define listCURRENT_LIST_LENGTH( pxList )\
         ( ( pxList )->uxNumberOfItems )
				 
/*��ȡ����ĵ�һ���ڵ��OWNER, ��TCB */
#define listGET_OWNER_OF_NEXT_ENTRY(pxTCB, pxList)\
{																					\
	List_t * const pxConstList = (pxList);\
	/*�ڵ�����ָ������ĵ�һ���ڵ�*/\
	( pxConstList )->pxIndex = ( pxConstList )->pxIndex->pxNext;\
	/*ò���Ǹ�����Ĳ���*/\
	if( ( void *) ( pxConstList )->pxIndex == ( void * ) &( ( pxConstList)->xListEnd) )\
	{		\
			( pxConstList )->pxIndex = ( pxConstList )->pxIndex->pxNext;\
	}\
	/*��ȡ�ڵ��OWNER, ��TCB*/\
	( pxTCB ) = ( pxConstList )->pxIndex->pvOwner;\
}


void vListInitialiseItem( ListItem_t * const pxItem );
void vListInitialise(List_t * const pxList);
void vListInsertEnd( List_t * const pxList, ListItem_t * const pxNewListItem);
void vListInsert( List_t * const pxList, ListItem_t * const pxNewListItem );
UBaseType_t uxListRemove( ListItem_t * const pxItemToRemove );


#endif
