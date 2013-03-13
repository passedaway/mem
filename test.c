/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  test mem
 *
 *        Version:  1.0
 *        Created:  2013年03月08日 09时15分04秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zhao, Changqing (NO), changqing.1230@163.com
 *        Company:  iPanel TV inc.
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mem.h"

static mem_mgr_t *mgr;
static char *ptr[100];

static int test_malloc(int len)
{
	int i = 0;
	for( ; i < sizeof(ptr) / sizeof(ptr[0]); i++)
	{
		ptr[i] = MALLOC(mgr, len);

		len += 10;

		if( !ptr[i] )
		{
			return i;
		}
#if 0
			mem_print(mgr);
			getchar();
#endif
	}

	return i;
}

static int test_free(int num)
{
	int i = 0;
	int ret = 0;
	for(; i < num; i++)
	{
	//	if( ptr[i] )
		{
			ret = FREE(mgr, ptr[i]);
			if( ret )
			{
				printf("test_free:ptr[%d] = %p error\n", i, ptr[i]);
			}
			mem_print(mgr);
#if 0
			getchar();
#endif
		}
	}

	return 0;
}

int main(void)
{
	int num = 0;
	char *_ptr;
	_ptr = malloc(4096);
	mgr = mem_init(_ptr, 1024);

	mem_print(mgr);

	/* test  */
	{
		printf("test malloc:\n");
		num = test_malloc(50);
		printf("memory out at %d times. first size=%d end size=%d\n", num, 50, 50+num*10);

		mem_print(mgr);
		strcpy(ptr[1], "changqingZhao.1230@163.com");
		strcpy(ptr[3], "tain@linuxmint: /opt/broadcom/7851/a0");

		mem_print(mgr);
	//	mem_dump(mgr);
	}

	FREE(mgr, ptr[1]);
	mem_print(mgr);
	getchar();

	FREE(mgr, ptr[3]);
	mem_print(mgr);
	getchar();

	{
		printf("test free: num=%d\n", num);
		test_free(num + 2);
		mem_print(mgr);
	//	mem_dump(mgr);
	}

	mem_deinit(mgr);
	free(_ptr);
	return 0;
}

