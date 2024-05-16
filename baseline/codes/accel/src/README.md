## To work:
Streams
Bitshift power-of-2 array sizes
Eliminate reads of env arrays?
Overlapping these "load internal RAM" loops with rest of computation?
EGM loop: 558, then 20, ...

Is there fast multiplicative inverse? Review other lit.

## Results
** Linux x86-64 clang++ **
````
# Good State Best Fit: logK' =  0.155011291852356 +  0.958837206031606 logk,  R2 =  0.999976277351379
# Bad  State Best Fit: logK' =  0.144512132512113 +  0.960292838062531 logK,  R2 =  0.999930322170258
Iteration: 19 Spread: 0.000991415 Regression Coeff: Bad:  0.143907025456429  0.960454046726227 R2:  0.999930322170258, Good:  0.154722303152084  0.958914399147034 R2:  0.999976277351379

Main Loop: 37.845280 s, 37845280.000000 cyc/iter, (1 iters)
SW init time: 1.126906 s, 1126906.000000 cyc/iter, (1 iters)
````

After kmts pre-compute and more pre-computed arrays:
````
~~~~ Initial kcross sum: 405241.125000000000000
~~~~ Initial alm_coeff kmts[t] sum: 44648.333496093750000

# Good State Best Fit: logK' =  0.155000181257954 +  0.958839995452069 logk,  R2 =  0.999976158142090
# Bad  State Best Fit: logK' =  0.144535794896379 +  0.960286290350328 logK,  R2 =  0.999930262565613
Iteration: 19 Spread: 0.000994144 Regression Coeff: Bad:  0.143924877047539  0.960449039936066 R2:  0.999930262565613, Good:  0.154719188809395  0.958914935588837 R2:  0.999976158142090
````

On Arm:
````
~~~~ Initial alm_coeff kmts[t] sum: 44649.979888916015625
# Good State Best Fit: logK' =  0.155040489703693 +  0.958829354939061 logk,  R2 =  0.999976158142
# Bad  State Best Fit: logK' =  0.144577752076148 +  0.960275311957217 logK,  R2 =  0.999930202960
Iteration: 19 Spread: 0.000987097 Regression Coeff: Bad:  0.143973782658577  0.960436165332794 R2:  0.999930202960968, Good:  0.154755890369415  0.958905339241028 R2:  0.999976158142090
````

## Development options
Design space exploration currently occuring in `dev_options.h`


## Dataflow error
````
ERROR: [XFORM 203-711] Internal global variable 'kprimes.0'  failed dataflow checking: it can only be written in one process function.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.0' has write operations in process function 'hw_top_setup'.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.0' has read and write operations in process function 'hw_sim_egm'.
ERROR: [XFORM 203-711] Internal global variable 'kprimes.0'  failed dataflow checking: it can only be read in one process function.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.0' has read and write operations in process function 'hw_sim_egm'.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.0' has read operations in process function 'hw_sim_ast'.
WARNING: [XFORM 203-713] All the elements of global array 'kprimes.0'  should be updated in process function 'hw_top_setup', otherwise it may not be synthesized correctly.
WARNING: [XFORM 203-713] All the elements of global array 'kprimes.0'  should be updated in process function 'hw_sim_egm', otherwise it may not be synthesized correctly.
ERROR: [XFORM 203-711] Array 'kprimes.0'  failed dataflow checking: it cannot be connected to more than 2 processes.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.0' has write operations in process function 'hw_top_setup'.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.0' has read and write operations in process function 'hw_sim_egm'.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.0' has read operations in process function 'hw_sim_ast'.
ERROR: [XFORM 203-711] Internal global variable 'kprimes.1'  failed dataflow checking: it can only be written in one process function.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.1' has write operations in process function 'hw_top_setup'.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.1' has read and write operations in process function 'hw_sim_egm'.
ERROR: [XFORM 203-711] Internal global variable 'kprimes.1'  failed dataflow checking: it can only be read in one process function.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.1' has read and write operations in process function 'hw_sim_egm'.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.1' has read operations in process function 'hw_sim_ast'.
WARNING: [XFORM 203-713] All the elements of global array 'kprimes.1'  should be updated in process function 'hw_top_setup', otherwise it may not be synthesized correctly.
WARNING: [XFORM 203-713] All the elements of global array 'kprimes.1'  should be updated in process function 'hw_sim_egm', otherwise it may not be synthesized correctly.
ERROR: [XFORM 203-711] Array 'kprimes.1'  failed dataflow checking: it cannot be connected to more than 2 processes.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.1' has write operations in process function 'hw_top_setup'.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.1' has read and write operations in process function 'hw_sim_egm'.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.1' has read operations in process function 'hw_sim_ast'.
ERROR: [XFORM 203-711] Internal global variable 'kprimes.2'  failed dataflow checking: it can only be written in one process function.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.2' has write operations in process function 'hw_top_setup'.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.2' has read and write operations in process function 'hw_sim_egm'.
ERROR: [XFORM 203-711] Internal global variable 'kprimes.2'  failed dataflow checking: it can only be read in one process function.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.2' has read and write operations in process function 'hw_sim_egm'.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.2' has read operations in process function 'hw_sim_ast'.
WARNING: [XFORM 203-713] All the elements of global array 'kprimes.2'  should be updated in process function 'hw_top_setup', otherwise it may not be synthesized correctly.
WARNING: [XFORM 203-713] All the elements of global array 'kprimes.2'  should be updated in process function 'hw_sim_egm', otherwise it may not be synthesized correctly.
ERROR: [XFORM 203-711] Array 'kprimes.2'  failed dataflow checking: it cannot be connected to more than 2 processes.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.2' has write operations in process function 'hw_top_setup'.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.2' has read and write operations in process function 'hw_sim_egm'.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.2' has read operations in process function 'hw_sim_ast'.
ERROR: [XFORM 203-711] Internal global variable 'kprimes.3'  failed dataflow checking: it can only be written in one process function.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.3' has write operations in process function 'hw_top_setup'.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.3' has read and write operations in process function 'hw_sim_egm'.
ERROR: [XFORM 203-711] Internal global variable 'kprimes.3'  failed dataflow checking: it can only be read in one process function.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.3' has read and write operations in process function 'hw_sim_egm'.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.3' has read operations in process function 'hw_sim_ast'.
WARNING: [XFORM 203-713] All the elements of global array 'kprimes.3'  should be updated in process function 'hw_top_setup', otherwise it may not be synthesized correctly.
WARNING: [XFORM 203-713] All the elements of global array 'kprimes.3'  should be updated in process function 'hw_sim_egm', otherwise it may not be synthesized correctly.
ERROR: [XFORM 203-711] Array 'kprimes.3'  failed dataflow checking: it cannot be connected to more than 2 processes.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.3' has write operations in process function 'hw_top_setup'.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.3' has read and write operations in process function 'hw_sim_egm'.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.3' has read operations in process function 'hw_sim_ast'.
ERROR: [XFORM 203-711] Internal global variable 'kprimes.4'  failed dataflow checking: it can only be written in one process function.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.4' has write operations in process function 'hw_top_setup'.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.4' has read and write operations in process function 'hw_sim_egm'.
ERROR: [XFORM 203-711] Internal global variable 'kprimes.4'  failed dataflow checking: it can only be read in one process function.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.4' has read and write operations in process function 'hw_sim_egm'.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.4' has read operations in process function 'hw_sim_ast'.
WARNING: [XFORM 203-713] All the elements of global array 'kprimes.4'  should be updated in process function 'hw_top_setup', otherwise it may not be synthesized correctly.
WARNING: [XFORM 203-713] All the elements of global array 'kprimes.4'  should be updated in process function 'hw_sim_egm', otherwise it may not be synthesized correctly.
ERROR: [XFORM 203-711] Array 'kprimes.4'  failed dataflow checking: it cannot be connected to more than 2 processes.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.4' has write operations in process function 'hw_top_setup'.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.4' has read and write operations in process function 'hw_sim_egm'.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.4' has read operations in process function 'hw_sim_ast'.
ERROR: [XFORM 203-711] Internal global variable 'kprimes.5'  failed dataflow checking: it can only be written in one process function.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.5' has write operations in process function 'hw_top_setup'.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.5' has write operations in process function 'hw_sim_egm'.
WARNING: [XFORM 203-713] All the elements of global array 'kprimes.5'  should be updated in process function 'hw_top_setup', otherwise it may not be synthesized correctly.
WARNING: [XFORM 203-713] All the elements of global array 'kprimes.5'  should be updated in process function 'hw_sim_egm', otherwise it may not be synthesized correctly.
ERROR: [XFORM 203-711] Array 'kprimes.5'  failed dataflow checking: it cannot be connected to more than 2 processes.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.5' has write operations in process function 'hw_top_setup'.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.5' has write operations in process function 'hw_sim_egm'.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.5' has read operations in process function 'hw_sim_ast'.
ERROR: [XFORM 203-711] Internal global variable 'kprimes.6'  failed dataflow checking: it can only be written in one process function.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.6' has write operations in process function 'hw_top_setup'.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.6' has write operations in process function 'hw_sim_egm'.
WARNING: [XFORM 203-713] All the elements of global array 'kprimes.6'  should be updated in process function 'hw_top_setup', otherwise it may not be synthesized correctly.
WARNING: [XFORM 203-713] All the elements of global array 'kprimes.6'  should be updated in process function 'hw_sim_egm', otherwise it may not be synthesized correctly.
ERROR: [XFORM 203-711] Array 'kprimes.6'  failed dataflow checking: it cannot be connected to more than 2 processes.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.6' has write operations in process function 'hw_top_setup'.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.6' has write operations in process function 'hw_sim_egm'.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.6' has read operations in process function 'hw_sim_ast'.
ERROR: [XFORM 203-711] Internal global variable 'kprimes.7'  failed dataflow checking: it can only be written in one process function.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.7' has write operations in process function 'hw_top_setup'.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.7' has write operations in process function 'hw_sim_egm'.
WARNING: [XFORM 203-713] All the elements of global array 'kprimes.7'  should be updated in process function 'hw_top_setup', otherwise it may not be synthesized correctly.
WARNING: [XFORM 203-713] All the elements of global array 'kprimes.7'  should be updated in process function 'hw_sim_egm', otherwise it may not be synthesized correctly.
ERROR: [XFORM 203-711] Array 'kprimes.7'  failed dataflow checking: it cannot be connected to more than 2 processes.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.7' has write operations in process function 'hw_top_setup'.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.7' has write operations in process function 'hw_sim_egm'.
WARNING: [XFORM 203-713] Internal global variable 'kprimes.7' has read operations in process function 'hw_sim_ast'.
WARNING: [XFORM 203-713] All the elements of global array 'kprimes.8'  should be updated in process function 'hw_top_setup', otherwise it may not be synthesized correctly.
WARNING: [XFORM 203-713] All the elements of global array 'kprimes.9'  should be updated in process function 'hw_top_setup', otherwise it may not be synthesized correctly.
ERROR: [XFORM 203-711] Internal global variable 'kcross'  failed dataflow checking: it can only be written in one process function.
WARNING: [XFORM 203-713] Internal global variable 'kcross' has write operations in process function 'hw_top_setup'.
WARNING: [XFORM 203-713] Internal global variable 'kcross' has read and write operations in process function 'hw_sim_ast'.
WARNING: [XFORM 203-713] All the elements of global array 'kcross'  should be updated in process function 'hw_sim_ast', otherwise it may not be synthesized correctly.
ERROR: [XFORM 203-711] Internal global variable 'env.k'  failed dataflow checking: it can only be read in one process function.
WARNING: [XFORM 203-713] Internal global variable 'env.k' has read operations in process function 'hw_sim_egm'.
WARNING: [XFORM 203-713] Internal global variable 'env.k' has read operations in process function 'hw_sim_ast'.
ERROR: [XFORM 203-711] Array 'env.k'  failed dataflow checking: it cannot be connected to more than 2 processes.
WARNING: [XFORM 203-713] Internal global variable 'env.k' has write operations in process function 'hw_top_setup'.
WARNING: [XFORM 203-713] Internal global variable 'env.k' has read operations in process function 'hw_sim_egm'.
WARNING: [XFORM 203-713] Internal global variable 'env.k' has read operations in process function 'hw_sim_ast'.
ERROR: [XFORM 203-711] Internal global variable 'env.km'  failed dataflow checking: it can only be read in one process function.
WARNING: [XFORM 203-713] Internal global variable 'env.km' has read operations in process function 'hw_sim_egm'.
WARNING: [XFORM 203-713] Internal global variable 'env.km' has read operations in process function 'hw_sim_ast'.
ERROR: [XFORM 203-711] Array 'env.km'  failed dataflow checking: it cannot be connected to more than 2 processes.
WARNING: [XFORM 203-713] Internal global variable 'env.km' has write operations in process function 'hw_top_setup'.
WARNING: [XFORM 203-713] Internal global variable 'env.km' has read operations in process function 'hw_sim_egm'.
WARNING: [XFORM 203-713] Internal global variable 'env.km' has read operations in process function 'hw_sim_ast'.
WARNING: [XFORM 203-713] Reading dataflow channel 'coeff' in the middle of dataflow may stall the dataflow pipeline:
WARNING: [XFORM 203-713] Argument 'coeff' has read operations in process function 'hw_sim_alm'.
WARNING: [XFORM 203-713] Reading dataflow channel 'agshock.V' in the middle of dataflow may stall the dataflow pipeline:
WARNING: [XFORM 203-713] Argument 'agshock.V' has read operations in process function 'hw_sim_ast'.
WARNING: [XFORM 203-713] Reading dataflow channel 'idshock.V' in the middle of dataflow may stall the dataflow pipeline:
WARNING: [XFORM 203-713] Argument 'idshock.V' has read operations in process function 'hw_sim_ast'.
WARNING: [XFORM 203-713] All the elements of global array 'kmts' should be updated in process function 'hw_sim_ast', otherwise it may not be synthesized correctly.
````