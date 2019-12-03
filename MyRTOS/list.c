#include "list.h"

/*
 *	����ڵ��ʼ�� 
 *  ����ڵ� ListItem_t �ܹ��� 5 ����Ա�����ǳ�ʼ����ʱ��ֻ�轫
 *  pvContainer ��ʼ��Ϊ�ռ��ɣ���ʾ�ýڵ㻹û�в��뵽�κ�����
 */
void vListInitialiseItem( ListItem_t * const pxItem ) 
{ 
    /* ��ʼ���ýڵ����ڵ�����Ϊ�գ���ʾ�ڵ㻹û�в����κ����� */
    pxItem->pvContainer = NULL;  
} 


/*
    ������ڵ��ʼ��
*/
void vListInitialize(List_t * const pxList)
{
	/*����������ָ��ָ�����һ���ڵ�*/
	pxList->pxIndex = ( ListItem_t * ) &(pxList->xListend);

	/*���������һ���ڵ�ĸ��������ֵ����Ϊ���ȷ���ýڵ������������ڵ�*/
	pxList->xListend.xItemvalue = portMAX_DELAY;
	
	
  /*�����һ���ڵ��pxNext��pxPreviousָ���ָ��ڵ�������ʾ����Ϊ��*/
	pxList->xListend.pxNext = (ListItem_t * ) &(pxList->xListend);

  /**/
  pxList->xListend.pxPrevious = (ListItem_t *) &(pxList->xListend);
	
	/*��ʼ������ڵ��������ֵΪ0����ʾ����Ϊ��*/
	pxList->uxNumberOfItems = ( UBaseType_t) 0U;
}


/*
    ���ڵ���뵽�����β��
*/
void vListInsertEnd( List_t * const pxList, ListItem_t * const pxNewListItem)
{
	ListItem_t * const pxIndex = pxList->pxIndex;
	
	pxNewListItem->pxNext = pxIndex;
	pxNewListItem->pxPrevious = pxIndex->pxPrevious;
	pxIndex->pxPrevious->pxNext = pxNewListItem;
	pxIndex->pxPrevious = pxNewListItem;
	
	
	/* ��ס�ýڵ����ڵ����� */
	pxNewListItem->pvContainer = ( void *) pxList;
	
	/* ����ڵ������ */
	( pxList->uxNumberOfItems )++;	
	
}

/*
    �����������һ���ڵ㣬�������ͬ��ֵ�����½ڵ��ھɽڵ�������
 */
void vListInsert( List_t * const pxList, ListItem_t * const pxNewListItem )
{
	ListItem_t *pxIterator;
	
	/*��ȡ�ڵ��������ֵ*/
	const TickType_t xValueOfInsertion = pxNewListItem->xItemValue;
	
	
	/*Ѱ�ҽڵ�Ҫ�����λ��*/
	if( xValueOfInsertion == portMAX_DELAY)
	{
		pxIterator = pxList->xListend.pxPrevious;
	}
	else
	{
		for(pxIterator = (ListItem_t  * ) &( pxList->xListend ); pxIterator->pxNext->xItemValue <= xValueOfInsertion; pxIterator = pxIterator->pxNext)
		{
			//�����ҵ��ڵ�Ҫ�����λ��
		}
	}
	
	/* �����������У����ڵ����*/
	pxNewListItem->pxNext = pxIterator->pxNext;
	pxNewListItem->pxPrevious = pxIterator;
	
	pxNewListItem->pxNext->pxPrevious = pxNewListItem;	
	pxIterator->pxNext = pxNewListItem;
	
	/*��ס�ýڵ����ڵ�����*/
	pxNewListItem->pvContainer = ( void * ) pxList;
	
	/* ����ڵ������ */
	(pxList->uxNumberOfItems)++;
	
}


UBaseType_t uxListRemove( ListItem_t * const pxItemToRemove )
{
	/* ��ȡ�ڵ��������� */
	List_t * const pxList = ( List_t * ) pxItemToRemove->pvContainer;
	
	/* ��ָ���Ľڵ������ɾ�� */
	pxItemToRemove->pxNext->pxPrevious = pxItemToRemove->pxPrevious;
	pxItemToRemove->pxPrevious->pxNext = pxItemToRemove->pxNext;
	
	/* ��������Ľڵ�����ָ�� */
	if ( pxList->pxIndex == pxItemToRemove)
	{
		pxList->pxIndex = pxItemToRemove->pxPrevious;
		
	}
	
	/* ��ʼ���ýڵ����ڵ�����Ϊ�գ���ʾ�ڵ㻹û�в����κ����� */
	pxItemToRemove->pvContainer = NULL;
	
	/* �ڵ������-- */
	( pxList->uxNumberOfItems )--;
	
	/* ����������ʣ��ڵ�ĸ��� */
	return pxList->uxNumberOfItems;
	
}
















