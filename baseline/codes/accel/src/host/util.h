#include "definitions.h"

#if BARE_METAL
#ifdef __SDSCC__
	#include <ff.h>
#endif
#endif

#define ERR_FILE_OPEN				(-1)
#define ERR_FILE_BR					(-2)
#define ERR_FILE_CLOSE				(-3)

void *mem_alloc(size_t nbytes);

void mem_free(void *ptr);

/**
 * Reads file of packed binary data.
 * Return:
 *	>= 0: number bytes red
 *	-1: file open error
 *	-2: read bytes less than requested
 *	-3: file close error
 */

int read_bin_dat(const char *filename,
				 unsigned char *dst,
				 unsigned int nb);