//********************************************************************
//
// Erman Gurses
// Advances Operating Systems COSC 5352
// Project #2: Implementation of Doubly Token Ring
//
// March 27, 2013
// Instructor: Dr. Ajay K. Katangur
//********************************************************************

//********************************************************************
//
// Includes and Defines
//
//********************************************************************
#include <stdio.h>
#include <unistd.h>
#include <mpi.h>
//********************************************************************
//
// Function Prototypes
//
//********************************************************************
  
 void doublyTokenRing ( int nprocs, int rank, int rounds, 
					    MPI_Status status, MPI_Request request, int tag);
  
  
//********************************************************************
//
// main Function
//
// This function creates MPI environment in the program and calls proper
// functions for the propers processes then it finalize MPI environment. 
//
// Return Value
// ------------
// 
//
// Value Parameters
// ----------------
//	int	     argc				It counts how many argument is entered 
//								in command line  
//
// Reference Parameters
// --------------------
// char 	*argv[]				Pointers of Arrays that keeps command 
//								line parameters 
// Local Variables
// ---------------
// nprocs		int			The number of processes 
// rank			int 		Rank Number
// rounds		int 		The number of rounds
// status		MPI_Status  Status information	
// request		MPI_Request Request information
// tag			int 		Tag information
//*******************************************************************
int main(int argc, char *argv[])
{
	int rank,
		nprocs,
		tag=0,
		rounds;
		
	MPI_Status  status;
	MPI_Request request;
	
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	// Get rounds number from user and parse it to int
	rounds = atoi(argv[1]);
    
	// Check the number of processes wheter is greater than 2 or not
	if(nprocs > 2)
	{
		// Function call
		doublyTokenRing(nprocs,rank,rounds,status,request,tag);
	}
	else
	{
		if(rank == 0)
		{
			// Process 0 prints this on the screen 
			printf("Number of prosesses must be greater than 2\n");			
		}	
	}
	MPI_Finalize();
	return 0;
}

//********************************************************************
//
// doublyTokenRing  Function
//
// This function performs doubly token ring operation. Initially, 
// process 0 sends a token its right and sends a token its left 
// respectively. Then the token that turns clock wise order is 
// increased by 1 and the token that turns counter clock wise order 
// is decrased by 1. The  token which is increased by one has a value
// of  nprocs times rounds. After all, each tokens is ended up process
// 0
//
// Return Value
// ------------
// void                       
//
// Value Parameters
// ----------------
// nprocs		int			The number of processes 
// rank			int 		Rank Number
// rounds		int 		The number of rounds
// status		MPI_Status  Status information	
// request		MPI_Request Request information
// tag			int 		Tag information
//
// Reference Parameters
// --------------------
//
// Local Variables
// ---------------
// i		int		Loop Iteration Variable
// right	int 	Right neighbour of process
// left		int 	Left neighbour of process
// check	int		check point for clock wise order
// tokenCCV	int		initial token for counter clock wise order
// tokenCV	int		initial token for  clock wise order
// token	int		genereal token which is turning around the ring
//*******************************************************************
void doublyTokenRing ( int nprocs, int rank, int rounds, 
					   MPI_Status status, MPI_Request request, int tag )
{
	// Variables Declarations 
	int i,
		right,
		left,
		check,
		tokenCCV,
		tokenCV,
		token;

	// Calculate left and right neighbour
	right = (rank + 1) % nprocs;
	left  = (nprocs + rank - 1) % nprocs;

	
	// Calculate check points
	check = nprocs*rounds;
   
    // Process 0 does this 
	if (rank == 0)
	{
		// Initialize
		tokenCCV  = nprocs*rounds;
		 tokenCV  = 0;	
		   token  = 0;
		
		// Proper output
		printf("\n****************************************************  Clock Wise  Counter Clock Wise\n");

		// Send first token for clock wise order
		MPI_Isend(&tokenCV, 1, MPI_INT, right, tag, MPI_COMM_WORLD, &request);	
		// Wait for the send operation complete
		MPI_Wait(&request, &status);

		
		// Send first token for counter clock wise order
		MPI_Isend(&tokenCCV, 1, MPI_INT, left, tag, MPI_COMM_WORLD, &request);
		// Wait for the send operation complete		
		MPI_Wait(&request, &status);
	}
   
   
   // loop for rounds times 2
   for(i = 0; i < rounds*2; i++)
   {

		// Wait for reveive token from any resource and any tag
		MPI_Irecv(&token, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &request);		
		// Wait to compete receive operation
		MPI_Wait(&request, &status);
		
		// Check the source where did you get this token from
		// If it comes from left
		if(status.MPI_SOURCE == left)
		{
			// Print the token where did you take from
			printf("Rank %3d: received  a token from rank %3d. Token is:%9d\n",rank,left,token);
			// Increase token value by 1
			token++;
			
			
			// In order to prove the cycles are independent
			// It is up to user to use it, 
			// If he wants to see independent cycles clearly
			//sleep(3);
						
			// Check whether cycle is consumed ar not for clock wise order			
			if(check > token)
			{
				// Send the token to right neighbour
				MPI_Isend(&token, 1, MPI_INT, right, tag, MPI_COMM_WORLD,&request);	
				
				// Wait for the send operation 
				MPI_Wait(&request, &status);
				
			}
		
		}
		// Check whether cycle is consumed ar not for counter clock wise order	
		else if(status.MPI_SOURCE == right)
		{
			// Print the token where did you take from
			printf("Rank %3d: received  a token from rank %3d. Token is: \t\t\t%d\n",rank,right,token);
			// Decrease token value by 1
			token--;
			
			// In order to prove the cycles are independent
			// It is up to user to use it, 
			// If he wants to see independent cycles clearly
			//sleep(3);
			
			// Check whether cycle is consumed ar not for counter clock wise order	
			if(0 < token)
			{
				// Send the token to left neighbour
				MPI_Isend(&token, 1, MPI_INT, left, tag, MPI_COMM_WORLD,&request);								
				// Wait for the send operation 
				MPI_Wait(&request, &status);
				
			}
		}		
	}
}
