/*
 * =====================================================================================
 *
 *       Filename:  mem allocate.h
 *
 *    Description:  for malloc free
 *
 *        Version:  2.0
 *        Created:  2013年03月07日 16时01分11秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zhao, Changqing ,changqing.1230@163.com
 *        Company:  iPanel TV inc.
 *
 * =====================================================================================
 */

#ifndef _MEM_H_
#define _MEM_H_

class Queue{
	public:
		Queue(){}
		~Queue(){}

		init()
		{
			this->next = this;
			this->prev = this;
		}

		in(Queue *e)
		{
			e->prev = this->prev;
			this->prev->next = e;
			this->prev = e;
			e->next = this;
		}

		out()
		{
			this->prev->next = this->next;
			this->next->prev = this->prev;
		}
		Queue *getNext(){return next}
		Queue *getPrev(){return prev}
	private:
		Queue *next;
		Queue *prev;
};

#define SIZE(size)	((size+3) & (~0x3))
class MemAllocater{
	public:
		MemAllocater(void *ptr, int size);
		~MemAllocater();

		virtual void print(void);
		virtual void dump(void);

#ifdef DEBUG
		virtual void *alloc(const char *file_name, const char *func_name, const int line, int size)=0;
		virtual int free(const char *file_name, const char *func_name, const int line, void *ptr)=0;
#define MALLOC(size)	alloc(__FILE__, __FUNCTION__, __LINE__, size)
#define FREE(ptr)	free(__FILE__, __FUNCTION__, __LINE__, ptr)
#else
		virtual void *alloc(int size)=0;
		virtual int free(void *ptr)=0;
#define MALLOC(size)	alloc(size)
#define FREE(ptr)		free(ptr)
#endif

	private:
	int totalSize;
	int allocSize;
	int freeSize;
};

#endif
