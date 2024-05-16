#include "app.h"

// Global counters
perf_ctr pctr_init = INIT_PERF_CTR;
perf_ctr pctr_main = INIT_PERF_CTR;
perf_ctr pctr_hw = INIT_PERF_CTR;
perf_ctr pctr_alm_coeff = INIT_PERF_CTR;

//map to array
perf_ctr pctr[4] = {INIT_PERF_CTR, INIT_PERF_CTR, INIT_PERF_CTR, INIT_PERF_CTR}

int main(int argc, char **argv)
{
	printf("K-S app start\n");
	int num_iter = 0;
	if (argc > 1) {
		num_iter = atoi(argv[1]);		
	}

	env_t env; // sizeof? ptr with sds_alloc?
	input_t in;
	vars_t vars;

	perf_ctr_start(&pctr_init);
	init_all(&env, &in, &vars);
	perf_ctr_update(&pctr_init);

	real R2[NSTATES_AG];
	real *kcross_in = vars.kcross_a;
	real *kcross_tmp;
	real *kcross_out = vars.kcross_b;

	real *kprime_in = vars.kprime_a;
	real *kprime_tmp;
	real *kprime_out = vars.kprime_b;

	printf("Solving the Model\n");
	perf_ctr_start(&pctr_main);

	bool first = true;
	real kcross_init = env__kss;

	output_t outputs;

	open_cl_init(in, env, pctr, outputs, num_iter);

	/** Exit (free memory) */
	free_all(&in, &vars);
	return 0;
}

void open_cl_init(input_t input, env_t env, vars_t vars, perf_ctr perf[4], output_t output, int num_iter) {
	//initialize fpga and rig for async
	// Boilerplate code to load the FPGA binary, create the kernel and command queue
	vector<cl::Device> devices = xcl::get_xil_devices();
	cl::Device device = devices[0];
	cl::Context context(device);
	cl::CommandQueue q(context, device,
			CL_QUEUE_PROFILING_ENABLE | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE);

	string run_type =
			xcl::is_emulation() ?
					(xcl::is_hw_emulation() ? "hw_emu" : "sw_emu") : "hw";
	string binary_file = kernel_name + "_" + run_type + ".xclbin";
	cl::Program::Binaries bins = xcl::import_binary_file(binary_file);
	cl::Program program(context, devices, bins);
	cl::Kernel kernel(program, kernel_name_charptr, NULL);
	//end boilerplate init

	// Create buffers - need to update these to specific to our function
	cl::Buffer buffer_inputs(context, CL_MEM_READ_ONLY,
			sizeof(input_t)); //need to double check this. might be way too big.
	cl::Buffer buffer_env(context, CL_MEM_READ_ONLY,
			sizeof(env_t));
	cl::Buffer buffer_vars(context, CL_MEM_READ_ONLY,
			sizeof(vars_t));
	cl::Buffer buffer_outputs(context, CL_MEM_WRITE_ONLY,
			sizeof(output_t));

	//create shared Virtual Memory Pointers
	unsigned char *agshock =  clSVMAlloc ( context, CL_MEM_READ_ONLY, AGSHOCK_ARR_SIZE, 8);
	unsigned char *idshock =  clSVMAlloc ( context, CL_MEM_READ_ONLY, IDSHOCK_ARR_SIZE, 8);

	//copy memory arrays into shared memory
	clEnqueueSVMMemcpy ( q, true, agshock, &(input.agshock[0]), AGSHOCK_ARR_SIZE, 0, NULL, NULL);
 	clEnqueueSVMMemcpy ( q, true, idshock, &(input.idshock[0]), IDSHOCK_ARR_SIZE, 0, NULL, NULL);


	//get user side pointers
	input_t *input_ptr = (input_t *) q.enqueueMapBuffer(
			buffer_inputs, CL_TRUE, CL_MAP_WRITE, 0, sizeof(input_t));
	env_t *env_ptr = (env_t *) q.enqueueMapBuffer(
			buffer_env, CL_TRUE, CL_MAP_WRITE, 0, sizeof(env_t));
	vars_t *vars_ptr = (vars_t *) q.enqueueMapBuffer(
			buffer_vars, CL_TRUE, CL_MAP_WRITE, 0, sizeof(vars_t));
	output_t* output_ptr = (unsigned int *) q.enqueueMapBuffer(
			buffer_outputs, CL_TRUE, CL_MAP_READ, 0, input_size * sizeof(unsigned int));
	//end create buffers
	
	// Make buffers resident in the device
	q.enqueueMigrateMemObjects( { buffer_env, buffer_vars, buffer_outputs} ,
		  						CL_MIGRATE_MEM_OBJECT_CONTENT_UNDEFINED);

	vector<cl::Event> inWait; //word
	vector<cl::Event> outWait; //flag

	cl::Event buffDone, krnlDone;
	cl::Event outputDone;
	vector<cl::Event> krnlWait;
	
	//copy in data
	memcpy(env_ptr, env, input_size);
	memcpy(vars_ptr, vars, input_size);

	
	// Set buffer kernel arguments (needed to migrate the buffers in the correct memory)
	kernel.setArg(0, buffer_outputs);
	clSetKernelArgSVMPointer(kernel, 1, agshock);
	clSetKernelArgSVMPointer(kernel, 2, idshock);
	kernel.setArg(3, buffer_env);
	kernel.setArg(4, buffer_vars);
	kernel.setArg(5, num_iter);

	q.enqueueMigrateMemObjects( { buffer_env, buffer_vars }, 0, &inWait,
			&buffDone);	
	inWait.push_back(buffDone);
	q.enqueueTask(kernel, &inWait, &krnlDone);
	krnlWait.push_back(krnlDone);

	q.enqueueMigrateMemObjects( { buffer_outputs },
				CL_MIGRATE_MEM_OBJECT_HOST, &krnlWait, &outputDone);

	set_callback(outputDone, "oooqueue");
	outWait.push_back(outputDone);

	printf("Iteration %d: Spread: %10.9lf; Regression Coeff:\n    Bad [%18.15lf %18.15lf] R2: %18.15lf\n    Good [%18.15lf %18.15lf] R2: %18.15lf\n",
			   output_ptr->iter_coeff,
			   output_ptr->metric_coeff,
			   output_ptr->coeff[0],
			   output_ptr->coeff[1],
			   output_ptr->R2[0],
			   output_ptr->coeff[2],
			   output_ptr->coeff[3],
			   output_ptr->R2[1]);

	//metric

	clWaitForEvents(1, (const cl_event *) &outWait);
}
