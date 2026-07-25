/*
 * Heap.h
 *
 *  Created on: 2026Äê3ÔÂ19ÈÕ
 *      Author: XYY
 */

#ifndef USER_HEAP_H_
#define USER_HEAP_H_

#include "zf_common_headfile.h"

#define TOTAL_HEAP_SIZE ((size_t) (1024 * 16)) // byte

void  *HEAP_malloc(size_t xWantedSize);
void   HEAP_free(void *pv);
size_t HEAP_get_free_size(void);
size_t HEAP_get_minimumEver_free_size(void);

#endif /* USER_HEAP_H_ */
