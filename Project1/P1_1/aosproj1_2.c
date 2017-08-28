//********************************************************************
//
// Erman Gurses
// Advances Operating Systems COSC 5352
// Project #1: ROCK PAPER SCISSORS Game
//
// March 7, 2013
// Instructor: Dr. Ajay K. Katangur
//********************************************************************

//********************************************************************
//
// Includes and Defines
//
//********************************************************************
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define ROCK 1
#define PAPER 2
#define SCISSORS 3


//********************************************************************
//
// Function Prototypes
//
//********************************************************************
int odd(int rank, int i, int n[], int TAG, MPI_Status stat, int p);
int even(int rank, int i, int n[], int TAG, MPI_Status stat, int p);

//********************************************************************
//
// main Function
//
// This function creates MPI environment in the program and calls 
// proper functions for the propers processes then it finalize 
// MPI environment. 
//
// Return Value
// ------------
// int 		  0					It returns 0 If it is succesful 
//
// Value Parameters
// ----------------
//	int	     argc				It counts how many argument is 
//								entered in command line
//								  
//
// Reference Parameters
// --------------------
// char 	*argv[]				Pointers of Arrays that keeps 
//								command	line parameters 
//
// Local Variables
// ---------------
// rank			int			  	It keeps rank of process  
// p			int	    		It keeps total number of 
//								processes (p-1)
// n[]			int				It creates enough buffer size.				
// stat			MPI_Status		It keep status information.
// whereIam		int				It keeps position of active process
// active		int				It keeps how many process in the game
// done			int				It determines whether is the game 
//								finishes or not
// last			int				When the number of active processes 
//								is odd it keeps
//								position of last active process
// result		int				It keeps the single round result 
//								between pairs.
// i			int				Loop Iteration Variable.
// j			int				Loop Iteration Variable.
// round		int				It keeps round information in the 
//								game 
// n			int				The array which keeps the information 
//								which processes iun the game 
// TAG			int				It keeps MPI_TAG information
//*******************************************************************

int main(int argc, char* argv[])
{

	int rank,
		p,
		find,
		whereIam=0,
		active=0,
		done = 0,
		last,
		result,
		i,
		j,
		round = 1;
	int *n  = NULL;
	int TAG = 1;
	
	MPI_Status stat; 
	//Initializing MPI
	MPI_Init(&argc, &argv);
 

	// Give rank to processes
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
 
	//  Find out how many processes 
	//	there are in the default
	//  communicator 
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	n = (int*)malloc(p*sizeof(int));
	
	
	// Initialize an array n
	for(i = 0; i < p; i++)
	{
		n[i]=1;
	}
		

	// Check whether it is done or not 
   while(done == 0)
   {	

		active = 0;
		
		// Count Active Processes
		for(i = 0; i < p; i++)
		{
			if(n[i]==1)
			{
				active++;
			}	
				
		}
			
		// Synchronize  processes 
		MPI_Barrier( MPI_COMM_WORLD );
	
	
		// Check whether the active process is one or not 
		if(active == 1)
		{
					
			done=1;
					
		}
		else
		{	// P-0 print round number
			if(rank == 0)
			{
				printf("\n\t  --Round %d--\n", round++);
			}
		
		
		}
		
		// Synchronize  processes 
		MPI_Barrier( MPI_COMM_WORLD );
		
		
		// Check whether it is the active process or not 
		if(n[rank]==1)
		{

			// Find where active process in actice processes
			whereIam = 0;
			for(i = 0; i < rank; i++)
			{
				if(n[i]==1)
				{
					whereIam++;
				}
				
				
			}
			
			
			// Check whether the game is done or not.
			if( done == 0)
			{	
				// Check whether the number of active processes is even
				if(active%2 == 0)
				{
					// Check whether the position of active process is even
					if(whereIam%2 == 0)
					{
						i = rank;
						find = 0;
						
						// Find position of next process to send a message
						while(!find)
						{
							i++;
							if(n[i] == 1)
							{	
								find=1;
							}	
						}
						
						
						
						// Function Call
						result = even(rank,i,n,TAG,stat,p);
						
						
						
					}
					else
					{
						i = rank;
						find = 0;
						
						// Find position of previous process to send a message
						while(!find)
						{
							i--;
							if(n[i] == 1)
							{	
								find=1;
							}
						}	
						
						// Function Call
						result = odd(rank,i,n,TAG,stat,p);

					}
				}
				else
				{	
					
					i = p;
					last = 0;
					
					// Find the last active process in the array
					while(!last)
					{
						i--;
						if(n[i] == 1)
						{
							last = i;
						}
							
					}
				
					// Do not allow last active process to get inside
					if(rank != last)
					{	
						// Check whether the position of active process is even
						if(whereIam%2 == 0)
						{
							i = rank;
							find = 0;
							
							// Find position of next process to send a message
							while(!find)
							{
								i++;
								if(n[i] == 1)
								{	
									find=1;
								}	
							}
							
							result = even(rank,i,n,TAG,stat,p);
						}
						else
						{
							i = rank;
							find = 0;
							
							while(!find)
							{
								i--;
								if(n[i] == 1)
								{	
									find=1;
								}
							}	
							
							// Function Call
							result = odd(rank,i,n,TAG,stat,p);	
						}
					}
					else					
					{	
						result = 1;
					}	
				}
			}
			else
			{
				printf("\nProcess %d wins the rock, paper, scissors tournament!!!\n\n",rank);		
			}
		}
		
		// Distribute your status to others
		 MPI_Allgather(&result,1,MPI_INT, n, 1, MPI_INT, MPI_COMM_WORLD);
	}

	// Finalize MPI structure
	MPI_Finalize();
	
	// Set free the array.
	free(n);
	
	return 0; 
	
}

//********************************************************************
// even Function
//
// This function belongs to process One that sends and receives 
// messages process Zero. The message size is growing iteratively. 
//
// Return Value
// ------------
// int                      if result equals 0
//								it returns 1
//							if result equals 1
//								it returns 0
//
// Value Parameters
// ----------------
// stat			MPI_Status	It keep status information.
// TAG			int			It keeps MPI_TAG information
// i			int			It keeps position of next active process
// p			int			The number of processes
// Reference Parameters
// --------------------
// n[]			int			It points the buffer.       	
//
// Local Variables
// ---------------
// myDecision	const int  	It determinates number of send 
//							and receive iteration  for the fixed  
//							size of package.
// decision		int			It keeps decision of process
//
// result		int 		After the game finished between pairs
//							it keeps result of the game
// seed			int			Random number for the decisions.
//
// j			int			Loop Iteration Variable.
// wall_Clock	double		It is beeing used to produce random number
//*******************************************************************
int even(int rank, int i, int n[], int TAG, MPI_Status stat, int p )
{	
		unsigned int seed;
		
		int myDecision,
			decision,
			result;
	
		double wall_Clock;
		do
		{
			// Gererate Random Number 
			wall_Clock = MPI_Wtime();
			wall_Clock = wall_Clock * 400000000;
			seed = (unsigned int)wall_Clock;
					
			myDecision = 1+seed%3;
						
			// Get desicison from opposite side			
			MPI_Recv(&decision,1,MPI_INT,i ,TAG,MPI_COMM_WORLD,&stat);
			

			// Determine who wins
			// In case of tie, do whole operation again until one side wins
			if(myDecision == decision)
			{		
				result = -1;
				// In case of tie, send special message
				MPI_Send(&result,1,MPI_INT,i ,TAG,MPI_COMM_WORLD);
			
			}
		    else if( myDecision == ROCK && decision == PAPER)
			{

				printf("Process %d wins over process %d\n",i, rank); 					
				result = 1;
				MPI_Send(&result,1,MPI_INT,i ,TAG,MPI_COMM_WORLD);

			}
			else if (myDecision == PAPER && decision == ROCK)
			{

				printf("Process %d wins over process %d\n",rank, i ); 	
				result = 0;
				MPI_Send(&result,1,MPI_INT,i ,TAG,MPI_COMM_WORLD);

			}
			else if (myDecision == ROCK && decision == SCISSORS)
			{
				printf("Process %d wins over process %d\n",rank, i); 
				result = 0;
				MPI_Send(&result,1,MPI_INT,i ,TAG,MPI_COMM_WORLD);
		
			}
			else if (myDecision == SCISSORS && decision == ROCK)
			{
				printf("Process %d wins over process %d\n",i, rank); 				
				result = 1;
				MPI_Send(&result,1,MPI_INT,i ,TAG,MPI_COMM_WORLD);
			}
			else if (myDecision == PAPER && decision == SCISSORS)
			{
				printf("Process %d wins over process %d\n", i ,rank);
				result = 1;		
				MPI_Send(&result,1,MPI_INT,i ,TAG,MPI_COMM_WORLD);
	
			}
			else if (myDecision == SCISSORS && decision == PAPER)
			{
				printf("Process %d wins over process %d\n",rank, i ); 					
				result = 0;
				MPI_Send(&result,1,MPI_INT,i,TAG,MPI_COMM_WORLD);
			}
			
			
		} while(result == -1);
		

		// Check Result
		if(result  == 0)
		{
			
			return 1;
			
		}
		else
		{
		
			return 0;
		}

}


//********************************************************************
// odd Function
//
// This function belongs to process One that sends and receives 
// messages process Zero. The message size is growing iteratively. 
//
// Return Value
// ------------
// int                      if result equals 0
//								it returns 0
//							if result equals 1
//								it returns 1
//
// Value Parameters
// ----------------
// stat			MPI_Status	It keep status information.
// TAG			int			It keeps MPI_TAG information
///i			int			It keeps position of next active process
// p			int			The number of processes
//
// Reference Parameters
// --------------------
// n[]			int				It points the buffer.       	
//
// Local Variables
// ---------------
// myDecision	const int  	It determinates number of send 
//							and receive iteration  for the fixed  
//							size of package.
// decision		int			It keeps decision of process
//
// result		int 		After the game finished between pairs
//							it keeps result of the game
// wall_Clock	double		It is beeing used to produce random number
//*******************************************************************

int odd(int rank, int i, int n[], int TAG, MPI_Status stat, int p)
{
	unsigned int seed;
	
	int	myDecision,
		decision,
		result;
	
	double wall_Clock;
	
	do
	{
		// Generate Random Number
		wall_Clock = MPI_Wtime();
		wall_Clock = wall_Clock * 400000000;
		seed = (unsigned int)wall_Clock;
				
		// Get Decision		
		myDecision = 1+seed%3;
		
		decision = myDecision;

		// Send your decision
		MPI_Send(&decision,1,MPI_INT, i,TAG,MPI_COMM_WORLD);
				
		// Get the the game result between pairs		
		MPI_Recv(&result,1,MPI_INT,i,TAG,MPI_COMM_WORLD,&stat);

	 // In case of tie, continue until one of them wins	
	}while(result==-1);

		// Check result
		if(result  == 0)
		{
			
			return 0;
			
		}
		else
		{
		
			return 1;
		}

}
