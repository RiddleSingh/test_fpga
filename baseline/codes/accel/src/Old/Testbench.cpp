#include <iostream>
#include <cstdlib>
#include <chrono>
#include "fpga_model.h"
#include "init.h"

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

  	env_t env;
  	input_t in;
  	vars_t vars;

  	init_all(&env, &in, &vars);

  	real R2[NSTATES_AG];
  	real *kcross_in = vars.kcross_a;
  	real *kcross_tmp;
  	real *kcross_out = vars.kcross_b;

  	real *kprime_in = vars.kprime_a;
  	real *kprime_tmp;
  	real *kprime_out = vars.kprime_b;


  	bool first = true;
  	real kcross_init = env__kss;

  	output_t outputs;

  	printf("Solving the Model\n");

  	output_t output = runOnfpga(outputs, &(in.agshock[0]), &(in.idshock[0]), env, vars, 1);

  	printf("R2    = [%lf,  %lf]\n", output.R2[0], output.R2[1]);
  	printf("coeff = [%lf,  %lf]\n        [%lf,  %lf]\n", output.coeff[0], output.coeff[1], output.coeff[2], output.coeff[3]);

  	return 0;
}
