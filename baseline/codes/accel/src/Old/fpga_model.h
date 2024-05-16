#pragma once

#include <iostream>
#include <ctime>
#include <utility>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "definitions.h"

extern "C"
{
output_t runOnfpga(output_t outputs, unsigned char* agshock, unsigned char* idshock, env_t env, vars_t vars, int num_iter);
}
