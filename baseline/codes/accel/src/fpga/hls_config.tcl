config_interface -m_axi_max_widen_bitwidth 512
#set_directive_allocation -limit 1 -type function sim_alm_coeff regression
#set_directive_allocation -limit 1 -type function sim_alm_coeff RSquaredCalc
#set_directive_allocation -limit 1 -type function sim_alm_coeff hw_log
# set_directive_allocation -limit 1 -type function regression hw_pow
# set_directive_allocation -limit 1 -type function RSquaredCalc hw_pow
#set_directive_allocation -limit 1 -type function hw_sim_alm hw_exp
#set_directive_allocation -limit 1 -type function hw_sim_alm hw_log
#set_param route.enableGlobalHoldIter true