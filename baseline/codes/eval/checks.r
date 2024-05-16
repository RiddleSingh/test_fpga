library(Metrics)
## USAGE
##	Launch R from terminal in eval/ folder, then:
##	source('checks.r') to source the file containing functions; then
##	check_all(f, f2) to compare two input arrays.
##		f: name of first folder
##		f2: name of second folder
##	For list of folders run 'ls checks' from eval/ folder.
##	x <- check_all("orig", "matlab")
##	x <- check_all("c", "matlab")
##	Then access elements of each check:
##		x$kprime
##		x$kcross
##		x$coeffs
##		x$r2
##	The default display output uses few significant digits. 
##	Access elements individually for more resolution in display, e.g.:
##		x$r2$d2


## reldiff
##	From http://c-faq.com/fp/fpequal.html
##	To supplement where percentage error fails due to 0.0 values in src arrays
reldiff <- function(a, b)
{
	d <- max(abs(a), abs(b))
	if (d == 0.0) {
		return(0.0)
	} else {
		return(abs(a - b) / d)
	}
 
}
## do_calcs
## 	performs and prints various metrics between d and d2
do_calcs <- function(d, d2)
{
	o <- list()
	
	## Including these in o$ make str() messy; sort out later if needed
	o$d <- d
	o$d2 <- d2
	o$reldiff <- mapply(function(x1, x2) reldiff(x1, x2), d, d2)
	o$absdiff <- mapply(function(x1, x2) abs(x1-x2), d, d2)

	o$mse <- mse(d, d2)
	o$mase <- mase(d, d2)
	o$rmse <- rmse(d, d2)
	o$max_abs_delta <- max(o$absdiff)
	o$mape <- mape(d, d2)
	o$smape <- smape(d, d2)
	o$mean_reldiff <- mean(o$reldiff)
	#o$ape <- ape(d, d2)
	o$max_delta <- max(mapply('-', d, d2))
	o$min_delta <- min(mapply('-', d, d2))
	o$mae <- mae(d, d2)
	o$sd_abs_delta <- sd(o$absdiff)
	o$cor_pearson <- cor(d, d2, method="pearson")
	
	
	options(digits=15)
	str(o)
	return(o)
}

## do_check
## 	Checks the same file between folder0 and folder1
##	Sep is "," for Matlab-generated file and "\n" for C-generated
do_check <- function(folder0, folder1, file, sep0=",", sep1=",")
{
	root0 <-"./checks/"
	f0 <- paste(root0, folder0, "/", file, ".csv", sep="", collapse=NULL)
	f1 <- paste(root0, folder1, "/", file, ".csv", sep="", collapse=NULL)

	d<-read.delim(f0, header=FALSE, sep=sep0)
	d2<-read.delim(f1, header=FALSE, sep=sep1)

	d<-as.numeric(unlist(d))
	d2<-as.numeric(unlist(d2))

	o <- do_calcs(d, d2)

	return(o)
}

## check_all
## 	Runs checks on all four file types between two folders
check_all <- function(folder0, folder1, sep0=",", sep1=",")
{
	res <- list()

	## If foldername begins with 'c', use \n separator
	if (substring(folder0, 1, 1) == "c") {
		sep0 <- "\n"
	}
	if (substring(folder1, 1, 1) == "c") {
		sep1 <- "\n"
	}

	## On with the show
	file <- "kprime"
	print("*** kprime *** -----------")
	res$kprime <- do_check(folder0, folder1, file, sep0, sep1);

	file <- "kcross"
	print("*** kcross *** -----------")
	res$kcross <- do_check(folder0, folder1, file, sep0, sep1);

	file <- "coeffs"
	print("*** coeffs *** -----------")
	res$coeffs <- do_check(folder0, folder1, file, sep0, sep1);

	file <- "r2bg"
	print("*** R^2 *** -----------")
	res$r2 <- do_check(folder0, folder1, file, sep0, sep1);

	invisible(res);

}


