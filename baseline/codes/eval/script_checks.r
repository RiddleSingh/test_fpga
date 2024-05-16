## Runs several checks of interest as a standalone script
## To use: source('script_checks.r')

source('checks.r')
l <- list()
l$orig_mat <- check_all('orig', 'matlab')
l$orig_mat_egm <- check_all('orig', 'm_egm_spline')
l$orig_mat_agg <- check_all('orig', 'm_agg_spline')

l$orig_c <- check_all('orig', 'c')
l$c_mat <- check_all('c', 'matlab')
l$c_cfloat <- check_all('c', 'cfloat')

