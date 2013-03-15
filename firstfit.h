/*
 * =====================================================================================
 *
 *       Filename:  firstfit.h
 *
 *    Description:  first fit
 *
 *        Version:  2.0
 *        Created:  2013年03月14日 13时40分29秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zhao, Changqing ,changqing.1230@163.com
 *        Company:  iPanel TV inc.
 *
 * =====================================================================================
 */
#ifndef _FIRST_FIT_H_
#define _FIRST_FIT_H_

#define SEAL_1	0x5AA5A55A
#define SEAL_2	0x55AAAA55
#define SEAL_3	0xAA5555AA

class MemNode {
	public:
		MemNode(void* ptr, int size);
		~MemNode();

	private:
		int outof_free_queue();

	private:
	union {
		queue_t dirty_queue;/* dirty queue*/
		queue_t free_queue;/* free queue*/
	};

	int size; /*  the size is the actiual size, size + 4 + 4 + ...  */

#ifdef DEBUG
	unsigned int seal;
	const char *file_name;
	const char *func_name;
	int line_num;
#endif
	void *data; /* addr */
};


class MemAllocater;
class FirstFit : public MemAllocater{
	private:
		FirstFit *dirty;
		FirstFit *free;
}
#endif

