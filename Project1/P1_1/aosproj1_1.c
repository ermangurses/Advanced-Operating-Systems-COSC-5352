//********************************************************************
//
// Erman Gurses
// Advances Operating Systems COSC 5352
// Project #1: Implementation of Pingpong on Distributed Systems
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
#define SIZE 1048576


//********************************************************************
//
// Function Prototypes
//
//********************************************************************

void processZero( FILE *cfPtr, MPI_Status stat, char n[]);
void processOne(MPI_Status stat, char n[]);

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
// 
// FILE		*cfPtr				File pointer 
//
// Local Variables
// ---------------
// rank			int			    
// p			int	    
// n[]			int				It creates enough buffer size.
//					
// stat			MPI_Status		It keep status information.
//
// 
//*******************************************************************
int main(int argc, char* argv[])
{

 int  rank,
	  p;
 char n[SIZE];
 FILE *cfPtr;
 MPI_Status stat; 
 
 //Initializing MPI
 MPI_Init(&argc, &argv);
 
 // Give rank to processes
 MPI_Comm_rank(MPI_COMM_WORLD, &rank);
 
 //  Find out how many processes 
 //	 there are in the default
 //  communicator 
 MPI_Comm_size(MPI_COMM_WORLD, &p);
  
    if(rank == 0) 
    {	
       processZero(cfPtr,stat,n);
	
    }
    else
    { 
	  processOne(stat,n);
		
    }// else	
			
	MPI_Finalize();
}


//********************************************************************
// processZero Function
//
// 	-This function belongs to process Zero that sends and receives 
// messages process one. The message size is growing iteratively.
//	-It also prints elapsed time information for the message of 
// length "n" on screen and into file.  
// 
// Return Value
// ------------
// void                         no return value
//
// Value Parameters
// ----------------
// stat			MPI_Status		It keep status information.
// 
//
// Reference Parameters
// --------------------
// n[]			int				It points the buffer.  
// FILE		    *cfPtr			File pointer   	
//
// Local Variables
// ---------------
// limit		const int  		It determinates number of send 
//								and receive iteration  for the fixed  
//								size of package.
//
// total		double   		Total amount of time for the sends 
//								and receives(10 times) for the 
//								message of length "n".
//								
//
// average		double  		Average message passing time for 
//								a message of length "n".
//
// elapsed_time double 			Total time of the one send and one 
//								reveive.
//
// i			int				Loop Iteration Variable.
// m			int				Loop Iteration Variable.
//*******************************************************************
void processZero( FILE *cfPtr, MPI_Status stat, char n[])
{
	const int limit        =10;
	double 	  total        =0,
		      average      =0,
		      elapsed_time =0;
	
	int 	  i,
			  m;
	
	printf("n\t\t  T\n");
  
	// Open and check file to write elapsed time information.
	if((cfPtr=fopen("aosproj1_1.txt","w"))==NULL)
	{
		printf("The file cannot be opened");
	}                   
	else
	{     
		// increase message length for each iteration
		 for(m = 0; m <= SIZE; m+=32768)
		 {                

			i=0;
			
			// 
			while(i < limit)
			{		
					// Record current time.
					elapsed_time=-MPI_Wtime();
					
					// Send data to process one
					MPI_Send(n,m,MPI_CHAR,1,0,MPI_COMM_WORLD);
		
					// Receive data from process one
					MPI_Recv(n, m, MPI_CHAR, 1, 0, MPI_COMM_WORLD,&stat);
					
					// Estimate elapsed time.
					elapsed_time+=MPI_Wtime();
		
					// Estimate average time.
					average=elapsed_time/2;
					
					// Estimate total time.
					total=total+average;
			
					i++;

			}//while
		 
			// Write the data into file
			fprintf(cfPtr,"%d  %f\n",m,total/10);	
				
			// Write the data onto screen
			printf("%d\t\t %f\n",m,total/10);
			
			total=0; 	   		
			
		}// for
	 
		
		fclose(cfPtr);

	}// else

}


//********************************************************************
// processOne Function
//
// This function belongs to process One that sends and receives 
// messages process Zero. The message size is growing iteratively. 
//
// Return Value
// ------------
// void                         no return value
//
// Value Parameters
// ----------------
// stat			MPI_Status		It keep status information.
// 
//
// Reference Parameters
// --------------------
// n[]			int				It points the buffer.       	
//
// Local Variables
// ---------------
// limit		const int  		It determinates number of send 
//								and receive iteration  for the fixed  
//								size of package.
//
// j			int				Loop Iteration Variable.
// m			int				Loop Iteration Variable.

//*******************************************************************
void processOne(MPI_Status stat, char n[])
{
	const int limit=10;
	int j,
		m;
	
	for(m = 0; m <= SIZE; m+=32768)
	{
		   
		j = 0;  
		
		while(j < limit)
		{
			// Receive data from process zero
			MPI_Recv(n,m,MPI_CHAR,0,0,MPI_COMM_WORLD,&stat);
			
			// Send data to process one
			MPI_Send(n,m,MPI_CHAR,0,0,MPI_COMM_WORLD);
		
		    j++;
		} //while
	
	}//for

}
