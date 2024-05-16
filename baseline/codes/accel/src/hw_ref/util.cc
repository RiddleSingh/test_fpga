#include "util.h"

void *mem_alloc(size_t nbytes)
{
#ifdef __SDSCC__
	return sds_alloc(nbytes);
#else
	return malloc(nbytes);
#endif
}

void mem_free(void *ptr)
{
#ifdef __SDSCC__
	sds_free(ptr);
#else
	free(ptr);
#endif
}

int read_bin_dat(const char *filename,
				 unsigned char *dst,
				 unsigned int nb)
{
	int res = 0;
#if BARE_METAL
	FIL file;
	FRESULT fr;
	unsigned int br = 0;

	fr = f_open(&file, filename, FA_READ);
	if (fr != FR_OK)
		return ERR_FILE_OPEN;
	fr = f_read(&file, dst, nb, &br);
	if (fr != FR_OK || nb != br)
		return ERR_FILE_BR;
	fr = f_close(&file);
	if (fr != FR_OK)
		return ERR_FILE_CLOSE;
#else
	FILE *fp = fopen(filename, "rb");

	if (fp == NULL)
		return ERR_FILE_OPEN;
	if ((res = fread(dst, 1, nb, fp)) != (int) nb)
		return ERR_FILE_BR;
	if (fclose(fp) != 0)
		return ERR_FILE_CLOSE;
#endif
	return res;
}