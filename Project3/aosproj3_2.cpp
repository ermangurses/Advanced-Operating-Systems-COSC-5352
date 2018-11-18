//********************************************************************
//
// Erman Gurses
// Advances Operating Systems COSC 5352
// Project #3: Implementation of vector Clocks
//
// April 11, 2013
// Instructor: Dr. Ajay K. Katangur
//********************************************************************

//********************************************************************
//
// Includes and Defines
//
//********************************************************************
#include <stdio.h>
#include <fstream>
#include <cstring>
#include <cctype>
#include <iostream>
#include <string>
#include <mpi.h>
//********************************************************************
//
// Function Prototypes
//
//********************************************************************
  
using namespace std; 


void manager(MPI_Status status, char * argv[], int nprocs);	
void workers(MPI_Status status, int rank, int nprocs); 
bool openInputFile(ifstream & inFile, char * file);
//********************************************************************
//
// main Function
//
// This function creates MPI environment in the program and 
// calls proper functions for the propers processes then it finalize 
// MPI environment. 
//
// Return Value
// ------------
// int 
//
// Value Parameters
// ----------------
// int      argc         It counts how many argument is 
//                       entered in command line  
//
// Reference Parameters
// --------------------
// char     *argv[]      Array of pointers that keeps command 
//                       line parameters 
// Local Variables
// ---------------
// nprocs                int             The number of processes 
// rank                  int             The rank Number
// status                MPI_status      The status information	
//*******************************************************************
int main(int argc, char *argv[]){

    MPI_Status status; 
    int nprocs,
        rank;
			
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(rank == 0){
        // Function Call
        manager(status,argv,nprocs);
    } else {
	// Function Call
        workers(status,rank,nprocs);
    }
    // Finalice MPI environment release resources
    MPI_Finalize();
  return 0;
}
//********************************************************************
// manager Function
//
// This function creates opens proper file reads data from that file.
// It assign task to worker processes regarding the data that is read 
// from file 
//
// Return Value
// ------------
// void
//
// Value Parameters
// ----------------
// status    MPI_status  The status information	
// nprocs    int         The number of processes 
//
// Reference  Parameters
// --------------------
// *argv[]   char        The array of pointers that keeps 
//                       command line parameters 
// Local Variables
// ---------------
// inFile                       ifstream    File pointer
// eventSourceInteger           int         Source of event	
// eventLineLenght              int         Entire line's lenght		
// * clock1                     int         Actual Clock info
// eventLine                    string      Entire event line that is 
//                                          read from file
// numberOfProcesses            string      The number of worker 
//                                          processes
// completed                    char        Key message to check 
//                                          operations in order
// endString                    string      Key string check 
//                                          end of file
// * eventLineCharArrayParsing  char        The char array that is 
//                                          parsed into tokens
// * file                       char	    To keep file name that is 
//                                          entered from command line 
// * eventLineCharArray         char        To keep entire line
// * eventTypeCharArray         char        To keep event type
// * eventSourceCharArray       char        To keep event source info
//*******************************************************************
void manager(MPI_Status status, char * argv[],int nprocs){

    ifstream inFile;
    
    int    eventSourceInteger,
           eventLineLenght,
           * clock1 = NULL;
				
    string eventLine,
           numberOfProcesses;	
    char   completed   = 'o';			
    string endString   = "end";
    char   * eventLineCharArray,
           * eventLineCharArrayParsing,
           * eventTypeCharArray,
           * eventSourceCharArray;
    char   * file;

    clock1 = new int[nprocs];
    
    // Initialize vector clocks
    for(int i = 0;  i < nprocs; i++){
        clock1[i] = 0;
    }

    file = argv[1];
    // Function call
    openInputFile(inFile,file);
    // get how many workers are there
    getline(inFile, numberOfProcesses);
    
    cout<<"\n---There are "<<numberOfProcesses<<" processes in the system---\n"<<endl;

    // Read next lines
    while (getline(inFile, eventLine)){
    
        // check if it is end string
        if(eventLine.compare(endString)){
 
            // get length of the line
            eventLineLenght = eventLine.length();
            // convert eventLine char TO array
            eventLineCharArray = strdup(eventLine.c_str());
            eventLineCharArrayParsing = strdup(eventLine.c_str());

            // tokanize eventLineCharArrayParsing
            eventTypeCharArray = strtok(eventLineCharArrayParsing," ");
 
            // Learn source of event
            eventSourceCharArray = strtok(NULL," ");

            eventSourceInteger = atoi(eventSourceCharArray);

            // Send the line to all
            for(int i = 1; i < nprocs; i++){
            
                MPI_Send(&eventLineLenght,1,MPI_INT,i,1,MPI_COMM_WORLD);
                MPI_Send(eventLineCharArray,eventLineLenght,MPI_CHAR,i,1,MPI_COMM_WORLD);
            }

            // Wait for the task is completed
            MPI_Recv(&completed,1,MPI_CHAR,eventSourceInteger,MPI_ANY_TAG, MPI_COMM_WORLD,&status);
        } else{

            // get length of the line
            eventLineLenght = eventLine.length();
            // convert eventLine char TO array
            eventLineCharArray = strdup(eventLine.c_str());

            // Send workers end message

            for(int i = 1; i < nprocs; i++){
            
                MPI_Send(&eventLineLenght,1,MPI_INT,i,0,MPI_COMM_WORLD);
                MPI_Send(eventLineCharArray,eventLineLenght,MPI_CHAR,i,0,MPI_COMM_WORLD);
            }

	    printf("\n");
            printf("-- Last States of the System --\n");
            printf("\n");

            // Print last snapshot
            for(int i = 1; i < nprocs; i++){
            
                MPI_Recv(clock1,nprocs,MPI_INT,i,MPI_ANY_TAG, MPI_COMM_WORLD,&status);
                printf("Process %d has clock (",status.MPI_SOURCE);
  
                for(int i = 1; i < nprocs; i++){

                    printf("%d ",clock1[i]);
                    if(i != nprocs-1){
                        printf(",");
                    }
                }
                printf(")\n");
            }					
        }
    }
}
//********************************************************************
// workers Function
//
// This function gets data from manager parses it then implemnts proper
// operation. This operation can be execute or send operation.
//
// Return Value
// ------------
// void
//
// Value Parameters
// ----------------
// status         MPI_status  The status information	
// nprocs         int         The number of processes 
// rank           int         To keep process rank information
//
// Reference  Parameters
// --------------------
// void
//
// Local Variables
// ---------------
// eventDestinationInteger       int The destination of event
// eventSourceInteger            int The source of event
// eventMessageLength            int The message Length
// eventLineLenght               int The entire line's lenght
// * clock1                      int Actual vector clock info
// * clock2                      int The temporary vector clock info
// sendEvent[]                  char To keep send event code
// completed                    char The message that keeps info 
//                                   about operation whether is 
//                                   completed or not
// * eventLineCharArray         char To keeps entire line
// * eventTypeCharArray         char To keeps event type
// * eventSourceCharArray       char To keeps event source info
// * eventDestinationCharArray  char To keeps event destination info
// * eventMessageCharArray      char To keeps actual message
// isMessageExist            boolean The flag for while loop
//*******************************************************************
void workers(MPI_Status status, int rank, int nprocs){

    int eventDestinationInteger,
        eventSourceInteger,
        eventMessageLength,
        eventLineLenght,
        * clock1 = NULL,
        * clock2 = NULL;
					
    char sendEvent[] = "send",
            completed   = 'o';
       
    char * eventLineCharArray,
         * eventTypeCharArray,
         * eventSourceCharArray,
         * eventDestinationCharArray,
         * eventMessageCharArray;
    bool isMessageExist =  true;

    clock1 = new int[nprocs];
    clock2 = new int[nprocs];

    for(int i = 0;  i < nprocs; i++){
        clock1[i] = 0;
        clock2[i] = 0;
    }
     
    while(isMessageExist){
     
        for(int i = 1; i < nprocs; i++){
         
            // Each Worker get the line
            if(rank == i){
         
                MPI_Recv(&eventLineLenght,1,MPI_INT,0,MPI_ANY_TAG, MPI_COMM_WORLD,&status);
                eventLineCharArray = new char [eventLineLenght+1];
                MPI_Recv(eventLineCharArray,eventLineLenght,MPI_CHAR,0,MPI_ANY_TAG, MPI_COMM_WORLD,&status);
            }
        }
        // If it is not end message
        if(status.MPI_TAG == 1){

            eventTypeCharArray = strtok(eventLineCharArray," ");
            // Check event type
            if(!strcmp(eventTypeCharArray,sendEvent)){
				
                // Tokanize the line
                eventSourceCharArray      = strtok(NULL," ");
                eventDestinationCharArray = strtok(NULL," ");
                eventMessageCharArray     = strtok(NULL,"\"");

                eventSourceInteger      = atoi(eventSourceCharArray);					
                eventDestinationInteger = atoi(eventDestinationCharArray);					
                eventMessageLength      = strlen(eventMessageCharArray);

                // Allow only valid process to send operation
                if(rank == eventSourceInteger){
                    // Send Actual message with its length  and clock info
                    MPI_Send(&eventMessageLength,1,MPI_INT,eventDestinationInteger,0,MPI_COMM_WORLD);
                    MPI_Send(eventMessageCharArray,eventMessageLength,MPI_CHAR,eventDestinationInteger,0,MPI_COMM_WORLD);

                    clock1[rank]++;
  
                    // Snaphot
                    printf("Message sent from process %d to process %d: %s\n",rank,eventDestinationInteger,eventMessageCharArray);
                    printf( "Logical time at process %d is (",rank);

                    for(int i = 1; i < nprocs; i++){
                        printf("%d ",clock1[i]);

                        if(i != nprocs-1){
                            printf(",");
                        }
                    }
                    printf(")\n");
	
                    MPI_Send(clock1,nprocs,MPI_INT,eventDestinationInteger,0,MPI_COMM_WORLD);
			
                    // Reveice report the task is completed from destination
                    MPI_Recv(&completed,1,MPI_CHAR,eventDestinationInteger,1, MPI_COMM_WORLD,&status);
			
                    // Send report the task is completed to P 0
                    MPI_Send(&completed,1,MPI_CHAR,0,1,MPI_COMM_WORLD);

                // Allow only valid process to receive operation
                }else if(rank == eventDestinationInteger){

                    // Receive Actual message with its length and clock info
                    MPI_Recv(&eventMessageLength,1,MPI_INT,eventSourceInteger,MPI_ANY_TAG, MPI_COMM_WORLD,&status);
                    eventMessageCharArray = new char [eventMessageLength+1];
                    MPI_Recv(eventMessageCharArray,eventMessageLength,MPI_CHAR,eventSourceInteger,MPI_ANY_TAG, MPI_COMM_WORLD,&status);
                    MPI_Recv(clock2,nprocs,MPI_INT,eventSourceInteger,0, MPI_COMM_WORLD,&status);

                    clock1[rank]++;
                    // check the clocks
                    for(int i = 1; i < nprocs; i++){
                        if(clock1[i] >= clock2[i]){
                            clock2[i] = clock1[i];
                        } else {
			    clock1[i] = clock2[i];
                        }
                    }
                    // Snaphot
                    printf("Message received from process %d by process %d: %s\n",rank,eventSourceInteger,eventMessageCharArray); 
                    printf( "Logical time at process %d is (",rank);
                   
                    for(int i = 1; i < nprocs; i++){
                        printf("%d ",clock1[i]);

                        if(i != nprocs-1){
                            printf(",");
                        }
                    }
                    printf(")\n\n");
                    // Send report the task is completed to source
                    MPI_Send(&completed,1,MPI_CHAR,eventSourceInteger,1,MPI_COMM_WORLD);
                }
            } else {
                eventSourceCharArray = strtok(NULL," ");
                eventSourceInteger = atoi(eventSourceCharArray);
               
                if(rank == eventSourceInteger){
 
                    clock1[rank]++;
                    printf( "Execution event in process %d\n",eventSourceInteger );
                    printf( "Logical time at process %d is (",rank);

                    for(int i = 1; i < nprocs; i++){
 
                        printf("%d ",clock1[i]);
                        if(i != nprocs-1){
                            printf(",");
                        }
                    }
                    printf(")\n\n");
		    // Send report to P 0 the task is completed
                    MPI_Send(&completed,1,MPI_CHAR,0,1,MPI_COMM_WORLD);
                }
            }
        } else if(status.MPI_TAG == 0){
            // The operations are done send last snapshot to P 0
            isMessageExist = false;

            for(int i = 1; i < nprocs; i++){
           
                MPI_Send(clock1,nprocs,MPI_INT,0,1,MPI_COMM_WORLD);
            }
        }
    }
}
//********************************************************************
//
// openInputFile Function
//
// This function opens the file and checks whether is opened or not.
//
// Return Value
// ------------
// bool
//
// Value Parameters
// ----------------
//void
//
// Reference Parameters
// --------------------
// &inFile              ifstream  File pointer	
// *file                char      File name
// Local Variables
// ---------------
// void 
//*******************************************************************
bool openInputFile(ifstream & inFile,  char * file) {
    // Open file
    inFile.open(file);

    // Check file whether is opened or not
    if (!inFile){
        cout << "The input File cannot been opened!!!\n";
        return false;
    }
  return true;
}


