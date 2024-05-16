#pragma once

#include <iostream>
#include <ctime>
#include <utility>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C"
{
void runOnfpga(unsigned char* outputs, unsigned char* inputs, int len);
}
