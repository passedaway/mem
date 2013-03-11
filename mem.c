/*
 * =====================================================================================
 *
 *       Filename:  mem.c
 *
 *    Description:  impl mem.h
 *
 *        Version:  1.0
 *        Created:  2013年03月07日 16时24分40秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zhao, Changqing (NO), changqing.1230@163.com
 *        Company:  iPanel TV inc.
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <string.h>

#include "mem.h"

#define SEAL_1	0x5AA5A55A
#define SEAL_2	0x55AAAA55
#define SEAL_3	0xAA5555AA

typedef struct queue_s {
	struct queue_s *prev;
	struct queue_s *next;
}queue_t;

#define MEMNODE_INIT	0
#define MEMNODE_FREE	1
#define MEMNODE_DIRTY	2

typedef struct mem_node_s {
	union {
		queue_t dirty_queue;/* dirty queue*/
		queue_t free_queue;/* free queue*/
	};
	unsigned int seal;

	int size; /*  the size is the actiual size, size + 4 + 4 + ...  */
	int type; /* this is not used */

#ifdef DEBUG
	const char *file_name;
	const char *func_name;
	int line_num;
#endif
	void *data; /* addr */
}mem_node_t;

#if 1
#define ele_to_stuct_start(m, s) (int)&(((s*)0)->m)
#define set_node_data(mnode)	(mnode)->data =  (unsigned char *)(mnode) + ele_to_stuct_start(data, mem_node_t) + 4 

#define ele_to_strut_point(d, m, s) (s*)((unsigned char *)d - ele_to_struct_start(m,s))
#else
#define set_node_data(mnode)	(mnode)->data =  (unsigned char *)(mnode) + sizeof(mem_node_t)
#endif

#define SIZE(size)	((size+3) & (~0x3))
#define mem_node_set_size(a, size) (a)->size = SIZE(size) + 16 
#define mem_node_next(a) (mem_node_s *)( (a)->start + (a)->size ) 
#define mem_node_dnext(a) (mem_node_s *)( (a)->d_next ) 

#define mem_node_init(a, ptr, size) (a)->start = ptr; \
									mem_node_set_size(a, size); \
									

struct mem_mgr_s {
	int total_size;/*  total size != free_size + alocate_size , because the struct */
	int free_size;
	int alocate_size;
	mem_node_t *head_dirty, *head_free;
};

static inline queue_init(queue_t *q)
{
	q->next = q;
	q->prev = q;
}

static inline queue_in(queue_t *q, queue_t *e)
{
	e->prev = q->prev;
	q->prev->next = e;
	q->prev = e;
	e->next = q;
}

static inline queue_out(queue_t *e)
{
	e->prev->next = e->next;
	e->next->prev = e->prev;
}

static inline mem_node_t *memnode_alloc(void *ptr, int size)
{
	mem_node_t *mnode = (mem_node_t*)ptr;
	memset(ptr, 0, sizeof(mem_node_t));

	mnode->type = MEMNODE_INIT;
	mnode->size = size;
	set_node_data(mnode);
	mnode->seal = SEAL_1;
	return mnode;
}

static inline mem_node_t *memnode_free(mem_node_t *mnode)
{
	mnode->seal = SEAL_1;
	mnode->type = MEMNODE_FREE;
}

static void memnode_dirty_to_free(mem_mgr_t *mgr, mem_node_t *mnode)
{
	/* check free_queue, if it can combine with some item */
	/* play some triks to in queue */
	/* ? can combine with some other item */
	int result_tail,  /*  result_tail indicate will combine in other's tail */
		result_head; /* result_head indicate will combine in other's head */
	mem_node_t *tmpm; 
	queue_t *q, *head, *tmpq;

	/* move mnode out of dirty */
	if( mgr->head_dirty == mnode )
	{
		if( mnode->dirty_queue.next == (queue_t*)mnode )
			mgr->head_dirty = NULL;
		else
		{
			mgr->head_dirty = (mem_node_t*)mnode->dirty_queue.next;
			queue_out(&mnode->dirty_queue);
		}
	}else
		queue_out(&mnode->dirty_queue);

	mnode->type = MEMNODE_FREE;
	mnode->seal = SEAL_3;

	mgr->alocate_size -= mnode->size;
	mgr->free_size += mnode->size; /*  this didnot include sizeof mem_node_t */

	/* free queue is null ,just insert it */
	if( mgr->head_free == NULL )
	{
		mgr->head_free = mnode;
		queue_init(&mnode->free_queue);
		return;
	}

	result_tail = (int)mnode;
	result_head = (int)mnode->data + mnode->size;
	printf("result_tail = 0x%x result_head = 0x%x\n", result_tail, result_head);

	head = &mgr->head_free->free_queue;
	q = head;
	do{
		tmpm = (mem_node_t*)q;
		printf("size + data = 0x%x tmpm=0x%x\n", tmpm->size + (int)tmpm->data, (int)tmpm);
		if( (tmpm->size + (int)tmpm->data) == result_tail )
		{ 
			/* combine (other's tail) */
			printf("combine other's tail ptr=%p size=%d\n", tmpm->data, tmpm->size);
			tmpm->size += ( mnode->size + sizeof(mem_node_t) );
			mgr->free_size += sizeof(mem_node_t);
			goto OUT;
		}

		if( (int)tmpm == result_head )
		{
			/* combine (other's head) */
			printf("combine other's head ptr=%p size=%d\n", tmpm->data, tmpm->size);
			mnode->size += (tmpm->size + sizeof(mem_node_t));
			mgr->free_size += sizeof(mem_node_t);

#if 0
			queue_out(&tmp->free_queue);
			queue_in(&tmp->free_queue, (queue_t*)mnode);

			goto OUT;
#else
			queue_out(&tmpm->free_queue);
			if( &tmpm->free_queue == &mgr->head_free->free_queue )
			{
				if( mgr->head_free->free_queue.next == mgr->head_free->free_queue.prev )
					mgr->head_free = NULL;
				else
					mgr->head_free = (mem_node_t*)tmpm->free_queue.next;
			}
			break;
#endif
		}

		q = q->next;
	}while(q != head );

	/* in queue: first is the min, the last is the max */
	if ( mgr->head_free )
	{
		head = &mgr->head_free->free_queue;
		q = head;
		tmpq = head;
		while( q->next != head )
		{
			if( ((mem_node_t*)q)->size > mnode->size )
				break;

			q = q->next;
		}
		queue_in(q, &mnode->free_queue);

#if 0
		if( mgr->head_free->size > mnode->size )
			mgr->head_free = mnode;
#endif

	}else{
		mgr->head_free = mnode;
		queue_init(&mnode->free_queue);
	}

OUT:
	return;
}

/* golable function */
mem_mgr_t *mem_init(void *ptr, int size)
{
	mem_mgr_t *mgr = NULL;

	if( ptr == NULL || (size <= (sizeof(mem_mgr_t) + 512)) )
		return NULL;

	mgr = (mem_mgr_t *)ptr;

	mgr->total_size = SIZE(size);
	mgr->free_size = SIZE(size) - sizeof(mem_mgr_t) - 2 * sizeof(mem_node_t);
	mgr->alocate_size = 0;
	mgr->head_dirty = NULL;
	mgr->head_free = NULL;

	mgr->head_free = memnode_alloc((unsigned char *)ptr + sizeof(mem_mgr_t), mgr->free_size);

	mgr->head_free->type = MEMNODE_FREE;
	queue_init(&mgr->head_free->free_queue);

	return (mgr->head_free)?mgr:NULL;
}

int mem_deinit(mem_mgr_t *mgr)
{
	if( mgr == NULL )
		return -1;

	/* check if has leak */
	if( mgr->head_dirty != NULL )
	{
		queue_t *head, *q;
		mem_node_t *tmp;

		head = &mgr->head_dirty->dirty_queue;
		q = head;
		do{
			tmp = (mem_node_t*)q;
#ifdef DEBUG
			printf("MEM_LEAK AT:[%s][%s][%d] size=%d (0x%x) ptr=%p\n", tmp->file_name, tmp->func_name, tmp->line_num, tmp->size, tmp->size, tmp->data );
#else
			printf("MEM_LEAK AT:size=%d (0x%x) ptr=%p\n", tmp->size, tmp->size, tmp->data );
#endif
			q = q->next;
		}while( q != head );
	}

	memset(mgr, 0, sizeof(mem_mgr_t));
	return 0;
}

#ifdef DEBUG
void *mem_malloc(const char *file_name, const char *func_name, const int line_num, mem_mgr_t *mgr, int size)
#else
void *mem_malloc(mem_mgr_t *mgr, int size)
#endif
{
	/* first fit */

	queue_t *q, *head;
	mem_node_t *mnode, *newnode;
	unsigned char *ptr;

	if( (size <= 0) || (mgr == NULL) )
		return NULL;

	if ( mgr->free_size < size )
		return NULL;
	
	/* find free space */
	size = SIZE(size);
	if( mgr->head_free )
		head = &mgr->head_free->free_queue;
	else
		return NULL;
	q = head;

	while( q->next != head )
	{
		if( ((mem_node_t*)q)->size < size )
			q = q->next;
		else
			break;

	}

#if 0
	if( (q == head) || (q->next == head) )
#endif
	{
		mnode = (mem_node_t*)q;
		if( mnode->size < size )
		{
			/* need do some process, make them compatch, then re-check */
			return NULL;
		}
	}

	/* allocate & init newnode */
	ptr = (unsigned char*)mnode->data + mnode->size - size;
	newnode = memnode_alloc(ptr, size);
	newnode->type = MEMNODE_DIRTY;
	newnode->seal = SEAL_2;
#ifdef DEBUG
	newnode->file_name = file_name;
	newnode->func_name = func_name;
	newnode->line_num = line_num;
#endif

	/* move the "newnode" from free_queue to dirty_queue */
	mnode->size -= (size + sizeof(mem_node_t));
	if( mnode->size <= 0 )
	{
		/* free queue is NULL */
		if( mnode == mgr->head_free )
		{
			mgr->head_free = NULL;
		}else{
			queue_out((queue_t*)mnode);
			memnode_free(mnode);
		}
	}

	if( mgr->head_dirty == NULL )
	{
		mgr->head_dirty = newnode;
		queue_init(&newnode->dirty_queue);
	}else
		queue_in(&mgr->head_dirty->dirty_queue, &newnode->dirty_queue);

	mgr->alocate_size += size;
	mgr->free_size -= (size+sizeof(mem_node_t));

	return newnode->data;
}

#ifdef DEBUG
int mem_free(const char *file_name, const char *func_name, const int line, mem_mgr_t *mgr, void *ptr)
#else
int mem_free(mem_mgr_t *mgr, void *ptr)
#endif
{
	/* get the memnode */
#if 0
	mem_node_t *mnode = ele_to_strut_point(ptr, data, mem_node_t);
	mem_node_t *mnode = (mem_node_t*)((unsigned char*)ptr - (int)&(((mem_node_t*)0)->data) );
#else
	mem_node_t *mnode = (mem_node_t*)((unsigned char*)ptr - sizeof(mem_node_t));
#endif
	printf("ptr=%p mnode = %p\n", ptr, mnode);
	/* check if in the dirty queue */
	{
		/* error occured */
		if( mgr->head_dirty == NULL )
			return -1;

		{
			queue_t *q;
			queue_t *head = &mgr->head_dirty->dirty_queue;
			q = head;
			while( q->next != head )
			{
				if( (mem_node_t*)q == mnode )
				{
					break;
				}

				q = q->next;
			}

			if( (mem_node_t *)q != mnode )
				return -1;
		}
	}

	/* move queue from dirty_queue to free_queue  */
	memnode_dirty_to_free(mgr, mnode);
OUT:
	return 0;
}

void mem_print(mem_mgr_t *mgr)
{
	queue_t *q, *head;
	mem_node_t *tmp;

	if( mgr == NULL )
		return;

	printf("\n******************************************\n");
	printf("**************** mgr info ****************\n");
	printf("*\ttotal size = %d ( 0x%x )\n*\t"
			"free size = %d ( 0x%x )\n*\t"
			"alocate size = %d ( 0x%x )\n*\t"
			"ptr = %p\n",
			mgr->total_size, mgr->total_size, 
			mgr->free_size, mgr->free_size, 
			mgr->alocate_size, mgr->alocate_size,
			mgr);

	printf("**************** allocate ****************\n");
	{
		if( mgr->head_dirty != NULL )
		{
			head = &mgr->head_dirty->dirty_queue;
			q = head;
			do{
				tmp = (mem_node_t*)q;
#ifdef DEBUG
				printf("*[%s][%s][%d] size=%d (0x%x) ptr=%p\n", tmp->file_name, tmp->func_name, tmp->line_num, tmp->size, tmp->size, tmp->data );
#else
				printf("*\tsize=%d (0x%x) ptr=%p\n", tmp->size, tmp->size, tmp->data );
#endif
				q = q->next;
			}while( q != head );
		}else
		{
			printf("*\tNone\n");
		}
	}

	printf("****************   free   ****************\n");
	{
		if ( mgr->head_free != NULL )
		{
			head = &mgr->head_free->dirty_queue;
			q = head;
			do{
				tmp = (mem_node_t*)q;
				printf("*\tsize=%d (0x%x) ptr=%p\n", tmp->size, tmp->size, tmp->data );

				q = q->next;
			}while( q != head );
		}else{
			printf("*\tNone\n");
		}
	}
	printf("******************************************\n\n");
}

void mem_dump(mem_mgr_t *mgr)
{
	int i = 0;
	char buf1[50], buf2[32], *ptr1, *ptr2;
	unsigned char *ptr;
	if( mgr == NULL )
		return ;

	printf("\n==================mem dump===================\n");
	printf("\ntotal_size = %d ( 0x%x) ptr = %p \n", mgr->total_size, mgr->total_size, mgr);
	ptr = (unsigned char *)mgr;

#define IS_PRINTF(c)	((c)>0x20 && (c)<0x7F)
	ptr1 = buf1, ptr2 = buf2;
	memset(buf1, 0x20, sizeof(buf1));
	for(; i < mgr->total_size; i++)
	{
		ptr1 += sprintf(ptr1, "%02x ", *ptr);
		ptr2 += sprintf(ptr2, "%c", IS_PRINTF(*ptr)?*ptr:'.');
		
		if( ((i+1) % 16) == 0 )
		{
			*ptr1 = 0, *ptr2 = 0;
			printf("%s| %s\n", buf1, buf2);
			ptr1 = buf1, ptr2 = buf2;
		}

		ptr++;
	}

	if( (ptr1 != buf1) || (ptr2 != buf2) )
	{
		while( ptr1 != buf1+sizeof(buf1) - 1)
			*ptr1 = ' ';
		*ptr1 = 0;

		printf("%s| %s\n", buf1, buf2);
	}
	printf("\n==================  over  ===================\n");
}

