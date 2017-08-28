I am providing all the files related to project.

You should follow these instructions;

1) To compile 

$make -f Makefile.primefib [ENTER]

2) To run

********SERVER SIDE (clst.tamucc.edu)********

$primefib_server & [ENTER]

********CLIENT SIDE (penguin.tamucc.edu)********

$primefib_client clst.tamucc.edu 5 [ENTER]


"5" can be changed up to 46 including.


"0" can be accepted for Fibonacci number's sequence but
It cannot be accepted for Prime numbers' sequence


			Note 1: 

In primefib_server.c file 

Functions "isPrime" and "prime_1_svc" are retrieved from:

http://jonlandrum.com/2012/02/06/computing-the-nth-prime/

that implements Sieve of Eratosthenes Algorithm

			Note 2:

For calculation of n th fibonacci number my n starting from "0"


Fibonacci 0 th : 0
Fibonacci 1 th : 1
Fibonacci 2 th : 1
Fibonacci 3 th : 2
Fibonacci 4 th : 3
Fibonacci 5 th : 5 ...

	







