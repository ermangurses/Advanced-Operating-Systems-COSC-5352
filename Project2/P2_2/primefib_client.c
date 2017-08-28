/********************************************************************
//
// Erman Gurses
// Advances Operating Systems COSC 5352
// Project #2: RPC Implementation 
//
// March 27, 2013
// Instructor: Dr. Ajay K. Katangur
********************************************************************/
/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

/********************************************************************
//
// Includes and Defines
//
/********************************************************************/

/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "primefib.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
/********************************************************************
// primefib_prog_1 Procedure
// -This procedure creates connection to server side
//
// Return Value
// ------------
//  void
//
//
// Value Parameters
// ----------------
//	numberNt  		int				n th number that is taken from the user
//
// Reference Parameters
// --------------------
//	host	 		char			Host String 
//      	
//
// Local Variables
// ---------------
//	*clnt    		CLIENT			Host string 
//	*result_1		int 			pointer that keeps result 1 
//									(n th prime number)
//
//	prime_1_arg     int				number th info from user
//
//	*result_2		int 			pointer that keeps result 2
//									(n th fibonacci number)
//
//	fibo_1_arg		int 			number th info from user
//
//	tim1 			struct timeval 	struct in order to keep time value
//									in second or micro seconds or both
//
//	tim2			struct timeval 	struct in order to keep time value
//									in second or micro seconds or both
//
//	t1				int				time by micro second 
//	t2				int				time by micro second 
********************************************************************/
void
primefib_prog_1(char *host, int numberTh)
{
	CLIENT *clnt;
	int   *result_1;
	int   prime_1_arg = numberTh;
	long  *result_2;
	int   fibo_1_arg  = numberTh;
	

	struct timeval tim1, tim2;
	int t1, t2;
	
#ifndef	DEBUG
	clnt = clnt_create (host, PRIMEFIB_PROG, PRIMEFIB_VERSION, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif	/* DEBUG */


	// Check n th information whether is proper or not. 
	if(numberTh > 0)
	{
		// get current time
		gettimeofday(&tim1, NULL);

		result_1 = prime_1(&prime_1_arg, clnt);
		
		// get current time
		gettimeofday(&tim2, NULL);
	 
	 
		if (result_1 == (int *) NULL) {
			clnt_perror (clnt, "call failed");
		}

		
		// convert the time to seconds 
		t1 = tim1.tv_usec;
		
		// convert the time to seconds 
		t2 = tim2.tv_usec;
		
		// Print the elapsed time and the prime number
		printf("Prime number     :%d is %d and the time taken is %d micro seconds\n",prime_1_arg,*result_1,t2-t1);
	
	}
	else
	{
		printf("There is no %dth prime number\n",numberTh);
	}
	
	
	if(numberTh >= 0)
	{
		// get current time
		gettimeofday(&tim1, NULL);

		result_2 = fibo_1(&fibo_1_arg, clnt);
		
		// get current time
		gettimeofday(&tim2, NULL);
		
		if (result_2 == (long *) NULL) {
			clnt_perror (clnt, "call failed");
		
		}
			
		// get the time as a microseconds 
		t1 = tim1.tv_usec;
		
		// get the time as a microseconds 
		t2 = tim2.tv_usec;
		
		
		// Print the elapsed time and the Fibonacci number
		printf("Fibonacci number :%d is %d and the time taken is %d micro seconds\n",fibo_1_arg,*result_2,t2-t1);
				
	}	
	else
	{

		printf("This program cannot calculate %dth fibonacci number\n",numberTh);

	}
#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 /* DEBUG */
}

/********************************************************************
// main  function
// -This function calls primefib_prog_1 function
//
// Return Value
// ------------
//  int 
//
//
// Value Parameters
// ----------------
//	argc			 int	number of command line argument
//
// Reference Parameters
// --------------------
//	argv[]			char	array of pointer for command line 
//      					arguments
//
// Local Variables
// ---------------
//	host 			char	server host
//
//
********************************************************************/
int
main (int argc, char *argv[])
{
	char *host;

	if (argc < 2) {
		printf ("usage: %s server_host\n", argv[0]);
		exit (1);
	}
	host = argv[1];
	primefib_prog_1 (host, atoi(argv[2]));
exit (0);
}
