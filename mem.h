/*
 * =====================================================================================
 *
 *       Filename:  mem.h
 *
 *    Description:  for malloc free
 *
 *        Version:  1.0
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

typedef struct mem_mgr_s mem_mgr_t;

mem_mgr_t *mem_init(void *ptr, int size);
int mem_deinit(mem_mgr_t *);

void mem_print(mem_mgr_t *mgr);
void mem_dump(mem_mgr_t *mgr);


#ifdef DEBUG_MEM
#ifdef DEBUG
void *mem_malloc(const char *file_name, const char *func_name, const int line, mem_mgr_t *, int size);

void mem_free(const char *file_name, const char *func_name, const int line, mem_mgr_t *, void *ptr);

#define MALLOC(mgr, size)	mem_malloc(__FILE__, __FUNCTION__, __LINE__, mgr, size)

#define FREE(mgr, ptr)	mem_free(__FILE__, __FUNCTION__, __LINE__, mgr, ptr)

#define Malloc(size)	mem_malloc(__FILE__, __FUNCTION__, __LINE__, NULL, size)
#define Free(ptr)	mem_free(__FILE__, __FUNCTION__, __LINE__, NULL, ptr)

#else
void *mem_malloc(mem_mgr_t *mgr, int size);
void mem_free(mem_mgr_t *mgr, void *ptr);

#define MALLOC(mgr, size)	mem_malloc(mgr, size)
#define FREE(mgr, ptr)		mem_free(mgr, ptr)
#define Malloc(size)	mem_malloc(NULL, size)
#define Free(ptr)		mem_free(NULL, ptr)
#endif

#else
#include <stdlib.h>
#define MALLOC(mgr, size)	malloc(size)
#define FREE(mgr, ptr)		free(ptr)
#define Malloc(size)	malloc(size)
#define Free(ptr)		free(ptr)
#endif

#endif

