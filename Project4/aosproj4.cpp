//********************************************************************
//
// Erman Gurses
// Advances Operating Systems COSC 5352
// Project #4: Chandy-Misra-Haas's edge chasing algorithm
//
// May 2, 2013
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
// calls proper functions for the propers processes then it finalizes 
// MPI environment. 
//
// Return Value
// ------------
// int 
//
// Value Parameters
// ----------------
// int    argc           It counts how many argument is entered 
//                       in command line  
//
// Reference Parameters
// --------------------
// char 	*argv[]  Array of pointer that keeps command 
//                       line parameters 
// Local Variables
// ---------------
// nprocs       int	    The number of processes 
// rank         int 	    The rank Number
// status       MPI_status  The status information	
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
// status	  MPI_status      The status information	
// nprocs	  int             The number of processes 
//
// Reference  Parameters
// --------------------
// *argv[]            char        Array of pointers that keeps 
//                                command line parameters 
// Local Variables
// ---------------
// inFile             ifstream    File pointer
// source             int         Source of event	
// destination	      int         Destination of event
// eventLineString    string      Entire event line 
//                                that is read from file
// completed          char        Key message to check operations in 
//                                order
// endString          string      Key string check end of file
// * file             char        To keep file name that is entered 
//                                from command line 
// startPointString   string      To keep information about which 
//                                process started detection
// startPointInteger  string      To keep information about which 
//                                process started detection
// parent             int         Row number in int **adjacencyMatrix 
//                                represents parent of child node
// child              int         Column number in int 
//                                **adjacencyMatrix represents 
//                                child node
// flag               int         To keep information about whether 
//                                there is a deadlock or not.
// counter            int 	  Counter fot column
// **adjacencyMatrix  int[][]     That represents dependency graph
// probe              int[]       It keeps probe info that contains 
//                                ID of process that blocked, ID of 
//                                process sending message, ID of 
//                                process that receiving message.
// eventLineCString   char[]	  C string version of eventLineString
//
//*******************************************************************
void manager(MPI_Status status, char * argv[],int nprocs){

    ifstream inFile;
    string eventLineString,
           startPointString;
    int    probe[3];
    string endString   = ""; 	
    int    startPointInteger,
           parent,
           child;
    char   completed   = 'o';	
    char   * file;
    char   * eventLineCString;
    int    destination; 
    int    flag = 0;
    int    counter = 0;
    
    file = argv[1];
    // Function call
    openInputFile(inFile,file);
    
    // Dynamic 2-D array allocation
    int **adjacencyMatrix = new int*[nprocs];

    for(int i = 0; i < nprocs; i++){

        adjacencyMatrix[i] = new int[nprocs];
    }

    // Initialize 2-D array
    for(int i = 0; i < nprocs; i++){
        for(int j = 0; j < nprocs; j++){
            adjacencyMatrix[i][j] = 0;
        }
    }

    // Read next lines	
    while (getline(inFile, eventLineString)){
 
        // check if it is end string
        if(eventLineString.compare(endString)){

            eventLineCString = strdup(eventLineString.c_str());
            parent = atoi(strtok(eventLineCString,"-"));
            child  = atoi(strtok(NULL," ")); 
            adjacencyMatrix[parent][child] = 1;

        } else {

            // Get startPoint from file
            getline(inFile, startPointString);
            startPointInteger  = atoi(strdup(startPointString.c_str()));
            cout<<"Start Point: "<<startPointInteger<<endl;
        }
    }
    cout<<" ";
    for(int i = 1; i < nprocs; i++){
        cout<<" "<<i<<" ";
    }
    printf("\n");
    cout<<" ";
    
    for(int i = 1; i < nprocs; i++){
        cout<<" "<<"_ ";
    }
    printf("\n");

    for(int i = 1; i < nprocs; i++){
    
        cout<<i<<"|";
        for(int j = 1; j < nprocs; j++){
            cout<<adjacencyMatrix[i][j]<<"  ";
        }
	cout<<endl;
    }

    // Check wherher or nor there is a deadlock
    for(int j = 1; j < nprocs; j++){
    
        for(int i = 1; i < nprocs; i++){
            counter = counter + adjacencyMatrix[i][j];
        }
        if(counter == 0){
            flag = 1;
        }
        counter = 0;
    }
    // Travers the graph from startPoint to end of matrix
    // And send probes to proper processes

    for(int i = startPointInteger; i < nprocs; i++){

        for(int j = 1; j < nprocs; j++){
            
            if(adjacencyMatrix[i][j] == 1){
  
                probe[0] = startPointInteger;
                probe[1] = i;
                probe[2] = j;
                
                for(int k = 1; k < nprocs; k++){
                    MPI_Send(probe,3,MPI_INT,k,1,MPI_COMM_WORLD);
                }
                destination = i;

                // Wait Check the operation if completed
                MPI_Recv(&completed,1,MPI_CHAR,destination,MPI_ANY_TAG, MPI_COMM_WORLD,&status);
            }
        }
    }

    // Travers the graph from first process to startPoint
    // And send probes to proper processes
    for(int i = 1; i < startPointInteger; i++){
    
        for(int j = 1; j < nprocs; j++){
   
            // Check adjacencyMatrix and find connection between processes
            if(adjacencyMatrix[i][j] == 1){
           
                probe[0] = startPointInteger;
                probe[1] = i;
                probe[2] = j;

                for(int k = 1; k < nprocs; k++){
                    MPI_Send(probe,3,MPI_INT,k,1,MPI_COMM_WORLD);
                }

                destination = i;
                // Wait Check the operation if completed
                MPI_Recv(&completed,1,MPI_CHAR,destination,MPI_ANY_TAG, MPI_COMM_WORLD,&status);
            }
        }
    }

    // Send trivial probe to say them operation is done
    for(int i = 1; i < nprocs; i++){
        MPI_Send(probe,3,MPI_INT,i,0,MPI_COMM_WORLD);
    }
    // Send flag info to starter process
    MPI_Send(&flag,1,MPI_INT,startPointInteger,0,MPI_COMM_WORLD);	
}

//********************************************************************
// workers Function
//
// This function gets data from manager parses it then implements 
// proper operation. This can be execute or send operation.
//
// Return Value
// ------------
// void
//
// Value Parameters
// ----------------
// status         MPI_status  The status information	
// nprocs         int         The number of processes 
// rank 	  int         To keep process rank information
//
// Reference  Parameters
// --------------------
//	void
//
// Local Variables
// ---------------
// destination    int         The destination of event
// source         int         The source of event
// completed      char        The message that keeps info about 
//                            operation whether is completed or not
// isMessageExit  boolean     Flag for while loop
// starter        int         To keep information about which process 
//                            started detection
// probe[]        int         It keeps probe info that contains 
//                            ID of process that blocked, ID of 
//                            process sending message, ID of process
//                            that receiving message.
// flag           int         To keeps information about whether 
//                            there is a deadlock or not. 
//*******************************************************************
void workers(MPI_Status status, int rank, int nprocs){

    int  starter,
         source,
         destination;	
    bool isMessageExist =  true;
     int probe[3];
    char completed   = 'o';
     int flag;

    while(isMessageExist){
    
        for(int i = 1; i < nprocs; i++){
        
            // Each Worker gets the probe
            if(rank == i){
                MPI_Recv(probe,3,MPI_INT,0,MPI_ANY_TAG, MPI_COMM_WORLD,&status);
                starter = probe[0];
                source = probe[1];
                destination = probe[2];
            }
        }
        // Check whether or not it is real probe
        if(status.MPI_TAG == 1){
 
            // Check for source info in probe
            if(rank == source){

                // Send probe to receiver
                MPI_Send(probe,3,MPI_INT,destination,0,MPI_COMM_WORLD);
                printf("PROCESS %d SENDS [%d, %d, %d] TO PROCESS %d\n",rank,probe[0],probe[1],probe[2],destination);
   
                // Get completed message from reveiver
                MPI_Recv(&completed,1,MPI_CHAR,destination,1, MPI_COMM_WORLD,&status);

                // Say process 0 the operation is done
                MPI_Send(&completed,1,MPI_CHAR,0,1,MPI_COMM_WORLD);
    	    // Check for destionation info in probe
            } else if(rank == destination) {
        
                // Get probe from sender
                MPI_Recv(probe,3,MPI_INT,source,MPI_ANY_TAG, MPI_COMM_WORLD,&status);

                // Say sender the probe has been gotten
                MPI_Send(&completed,1,MPI_CHAR,source,1,MPI_COMM_WORLD);
            }
        } else if(status.MPI_TAG == 0) {
            // The operations are done 
            isMessageExist = false;
        }
    }

    // Check rank whether it is starter or not
    if(rank == probe[0]){

        // Get flag information from process 0
        MPI_Recv(&flag,1,MPI_INT,0,MPI_ANY_TAG, MPI_COMM_WORLD,&status);
        // Check the flag to detect deadlock in the system. 
        if(flag == 0){
            printf("THE SYSTEM IS DEADLOCKED\n");
        } else {
            printf("THE SYSTEM IS NOT DEADLOCKED\n");
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
//	void
//
// Reference Parameters
// --------------------
// &inFile    ifstream    File pointer	
// *file      char        File name
//
// Local Variables
// ---------------
//      void 
//*******************************************************************
bool openInputFile(ifstream & inFile,  char * file){
	// Open file
   inFile.open(file);
   
	// Check file whether is opened or not
   if (!inFile){
      cout << "The input File cannot been opened!!!\n";

	  return false;
   }
   return true;
}
