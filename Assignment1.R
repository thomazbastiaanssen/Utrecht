##Assignment 1

#Produce a table	of	the	outcomes of	the	formula: y=2 * cos(x),
#	with	x	ranging from	-10	to	10,	with	increments	of	1.

#define df with the desired dimensions
result <- data.frame(x=-10:10, y= (rep(NA,21)))

#define a counter for in the for-loop
n      <- 1

#fill the df with a for-loop
for( x in -10:10)
{
   result[n,2] <- 2*cos(x)
   n <- n+1

}

View(result)