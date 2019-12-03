#include "list.h"

/*
 *	链表节点初始化 
 *  链表节点 ListItem_t 总共有 5 个成员，但是初始化的时候只需将
 *  pvContainer 初始化为空即可，表示该节点还没有插入到任何链表。
 */
void vListInitialiseItem( ListItem_t * const pxItem ) 
{ 
    /* 初始化该节点所在的链表为空，表示节点还没有插入任何链表 */
    pxItem->pvContainer = NULL;  
} 


/*
    链表根节点初始化
*/
void vListInitialize(List_t * const pxList)
{
	/*将链表索引指针指向最后一个节点*/
	pxList->pxIndex = ( ListItem_t * ) &(pxList->xListend);

	/*将链表最后一个节点的辅助排序的值设置为最大，确保该节点就是链表的最后节点*/
	pxList->xListend.xItemvalue = portMAX_DELAY;
	
	
  /*将最后一个节点的pxNext和pxPrevious指针均指向节点自身，表示链表为空*/
	pxList->xListend.pxNext = (ListItem_t * ) &(pxList->xListend);

  /**/
  pxList->xListend.pxPrevious = (ListItem_t *) &(pxList->xListend);
	
	/*初始化链表节点计数器的值为0，表示链表为空*/
	pxList->uxNumberOfItems = ( UBaseType_t) 0U;
}


/*
    将节点插入到链表的尾部
*/
void vListInsertEnd( List_t * const pxList, ListItem_t * const pxNewListItem)
{
	ListItem_t * const pxIndex = pxList->pxIndex;
	
	pxNewListItem->pxNext = pxIndex;
	pxNewListItem->pxPrevious = pxIndex->pxPrevious;
	pxIndex->pxPrevious->pxNext = pxNewListItem;
	pxIndex->pxPrevious = pxNewListItem;
	
	
	/* 记住该节点所在的链表 */
	pxNewListItem->pvContainer = ( void *) pxList;
	
	/* 链表节点计数器 */
	( pxList->uxNumberOfItems )++;	
	
}

/*
    按照升序插入一个节点，如果有相同的值，则新节点在旧节点后面插入
 */
void vListInsert( List_t * const pxList, ListItem_t * const pxNewListItem )
{
	ListItem_t *pxIterator;
	
	/*获取节点的排序辅助值*/
	const TickType_t xValueOfInsertion = pxNewListItem->xItemValue;
	
	
	/*寻找节点要插入的位置*/
	if( xValueOfInsertion == portMAX_DELAY)
	{
		pxIterator = pxList->xListend.pxPrevious;
	}
	else
	{
		for(pxIterator = (ListItem_t  * ) &( pxList->xListend ); pxIterator->pxNext->xItemValue <= xValueOfInsertion; pxIterator = pxIterator->pxNext)
		{
			//迭代找到节点要插入的位置
		}
	}
	
	/* 根据升序排列，将节点插入*/
	pxNewListItem->pxNext = pxIterator->pxNext;
	pxNewListItem->pxPrevious = pxIterator;
	
	pxNewListItem->pxNext->pxPrevious = pxNewListItem;	
	pxIterator->pxNext = pxNewListItem;
	
	/*记住该节点所在的链表*/
	pxNewListItem->pvContainer = ( void * ) pxList;
	
	/* 链表节点计数器 */
	(pxList->uxNumberOfItems)++;
	
}


UBaseType_t uxListRemove( ListItem_t * const pxItemToRemove )
{
	/* 获取节点所在链表 */
	List_t * const pxList = ( List_t * ) pxItemToRemove->pvContainer;
	
	/* 将指定的节点从链表删除 */
	pxItemToRemove->pxNext->pxPrevious = pxItemToRemove->pxPrevious;
	pxItemToRemove->pxPrevious->pxNext = pxItemToRemove->pxNext;
	
	/* 调整链表的节点索引指针 */
	if ( pxList->pxIndex == pxItemToRemove)
	{
		pxList->pxIndex = pxItemToRemove->pxPrevious;
		
	}
	
	/* 初始化该节点所在的链表为空，表示节点还没有插入任何链表 */
	pxItemToRemove->pvContainer = NULL;
	
	/* 节点计数器-- */
	( pxList->uxNumberOfItems )--;
	
	/* 返回链表中剩余节点的个数 */
	return pxList->uxNumberOfItems;
	
}
















