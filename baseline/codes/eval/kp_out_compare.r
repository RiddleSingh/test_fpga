library(Metrics)

d<-read.delim("./kp_out/double/kpo_0", header=FALSE, sep="\n")
d2<-read.delim("./kp_out/fixed/kpo_0", header=FALSE, sep="\n")

root0<-"./kp_out/fixed/kpo_"
root1<-"./kp_out/allfixed/kpo_"


for (i in 0:18) {
	iter <- as.character(i)
	f0 <- paste(root0, iter, sep="", collapse=NULL)
	f1 <- paste(root1, iter, sep="", collapse=NULL)

	d<-read.delim(f0, header=FALSE, sep="\n")
	d2<-read.delim(f1, header=FALSE, sep="\n")

	d<-as.numeric(unlist(d))
	d2<-as.numeric(unlist(d2))

	# MSE
	y1 <- mean(mapply(function(x1, x2) (x1-x2) * (x1-x2), d, d2))
	# RMSE
	y2 <- rmse(d, d2)
	# Max difference
	y3 <- max(mapply(function(x1, x2) abs(x1-x2), d, d2))
	# Most positive
	y4 <- max(mapply('-', d, d2))
	# Most negative
	y5 <- min(mapply('-', d, d2))
	# Mean of absolute delta
	y6 <-mean(mapply(function(x1, x2) abs(x1-x2), d, d2))
	# SD of absolute delta
	y7 <-sd(mapply(function(x1, x2) abs(x1-x2), d, d2))
	print(paste("Iter ", iter, ":", sep="", collapse=NULL))
	print(c(y1, y2, y3, y4, y5, y6, y7))
}
