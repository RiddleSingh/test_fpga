#ifndef __SDSCC__
#include "tests.h"

int main()
{
	int result = 0;
	int cr;

	printf("Begining tests...\n\n");

	cr = test_hw_util();
	result |= cr;
	printf("-- test_hw_util: %d\n", cr);

	printf("\n\n*** All tests completed: %d\n", result);
	return result;
}
#endif