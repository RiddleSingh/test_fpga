#include <iostream>
#include <cstdlib>
#include <chrono>
#include "fpga_model.h"

#define AGSHOCK_ARR_SIZE 138

class stopwatch
{
public:
  double total_time, calls;
  std::chrono::time_point<std::chrono::high_resolution_clock> start_time, end_time;
  stopwatch() : total_time(0), calls(0){};

  inline void reset()
  {
    total_time = 0;
    calls = 0;
  }

  inline void start()
  {
    start_time = std::chrono::high_resolution_clock::now();
    calls++;
  };

  inline void stop()
  {
    end_time = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
    total_time += static_cast<double>(elapsed);
  };

  // return latency in ns
  inline double latency()
  {
    return total_time;
  };

  // return latency in ns
  inline double avg_latency()
  {
    return (total_time / calls);
  };
};

bool Compare_arrs(unsigned char *actual,
                  unsigned char *fpga,
				  int len)
{
  bool Equal = true;
  for (int Y = 0; Y < len; Y++)
      if (actual[Y] != fpga[Y])
      {
        std::cout << actual[Y] << "!=" << fpga[Y] << std::endl;
        Equal = false;
      }
  return Equal;
}


int main()
{


  unsigned char actual[AGSHOCK_ARR_SIZE] = {
			0,
			64,
			192,
			71,
			126,
			158,
			7,
			0,
			224,
			255,
			255,
			247,
			135,
			247,
			199,
			0,
			48,
			0,
			0,
			255,
			63,
			240,
			7,
			192,
			24,
			255,
			255,
			13,
			252,
			31,
			124,
			244,
			255,
			255,
			255,
			255,
			255,
			248,
			255,
			3,
			0,
			0,
			192,
			1,
			0,
			255,
			15,
			128,
			31,
			0,
			198,
			1,
			0,
			0,
			32,
			224,
			255,
			31,
			248,
			79,
			0,
			240,
			31,
			0,
			0,
			63,
			248,
			7,
			224,
			7,
			0,
			3,
			0,
			0,
			0,
			192,
			255,
			255,
			255,
			15,
			252,
			128,
			3,
			0,
			176,
			255,
			103,
			252,
			255,
			255,
			255,
			252,
			47,
			1,
			254,
			15,
			0,
			192,
			255,
			255,
			255,
			231,
			255,
			15,
			240,
			255,
			63,
			255,
			95,
			0,
			0,
			128,
			127,
			15,
			252,
			255,
			7,
			1,
			0,
			0,
			30,
			254,
			63,
			192,
			255,
			127,
			0,
			0,
			64,
			0,
			240,
			63,
			0,
			0,
			255,
			15,
			240
		};

  unsigned char fpga[AGSHOCK_ARR_SIZE];

  runOnfpga(&fpga[0], &actual[0], AGSHOCK_ARR_SIZE);

  bool Equal = Compare_arrs(actual, fpga, 5);

  std::cout << "TEST " << (Equal ? "PASSED" : "FAILED") << std::endl;

  return Equal ? 0 : 1;
}
