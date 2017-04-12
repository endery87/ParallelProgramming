#include "mpi.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>




void InputGenerator(char Input[],int MAX_LENGTH,char *CurrentStates,char Transition[10][20][3],int NumberOfFiles,
					int length,char AcceptStates[10][10],int *NumberOfAcceptStates,char dfa[10],
					int mypid,int NumberOfProcessors,FILE* wFile,int id,MPI_Status status)
{
	
	int i=0,j=0,g=0;	
	char Output[50];					//output string for each line
	int NumberOfDFAAccepts=0;
	int tag=0;


	if(mypid==0)					//for processor with id 0
	{
		for(i=0;i<length;i++)
			Output[i]=Input[i];
		Output[i]='\0';
		for(i=length;i<49;i++)
			Output[i]=' ';					//initially output string is filled with input string name and ' '
		Output[i]='\n';
		MPI_Send(&Output, 50, MPI_CHAR,3, tag, MPI_COMM_WORLD);
	}

	if(mypid==3)				//for processor with id 3
	{

		MPI_Recv(&Output, 50, MPI_CHAR,0, tag, MPI_COMM_WORLD,&status);//receives the output string

	
	Output[50]='\0';

	for(i=0;i<NumberOfFiles;i++)
	{
		if(strlen(Input)!=0)			// if we don't have empty string
		{

			if(Input[strlen(Input)-1]=='a')	//if the last character added is 'a'
			{
				CurrentStates[i]=Transition[i][(CurrentStates[i]-'0')*2][2];	//go to transition function and update the current state
			}

			if(Input[strlen(Input)-1]=='b')	//if the last character added is 'b'
			{
				CurrentStates[i]=Transition[i][(CurrentStates[i]-'0')*2+1][2]; //go to transition function and update the current state
			}
		}

		for(j=0;j<NumberOfAcceptStates[i];j++)
		{
			if(CurrentStates[i]==AcceptStates[i][j]	)		// if the current state is an accept state
			{
				Output[length+NumberOfDFAAccepts*2+2]=dfa[i];		
				NumberOfDFAAccepts++;
				break;
			}
		}				
	}


	NumberOfDFAAccepts=0;
	fputs(Output, wFile);//prints the output string to the output.dat
	Output[0]='\0';

	}


	if(strlen(Input)<MAX_LENGTH)
	{
		Input[length]='a';
		Input[length+1] = '\0';//add 'a' to the end of the array
		
		InputGenerator(Input,MAX_LENGTH,CurrentStates,Transition,NumberOfFiles,length+1,AcceptStates,NumberOfAcceptStates
			,dfa,mypid,NumberOfProcessors,wFile,id,status);
		//call InputGenerator for "oldstring" +a

		
		Input[length] = '\0';
		Input[length] = 'b';
		Input[length+1] = '\0'; //add 'b' to the end of the array
		InputGenerator(Input,MAX_LENGTH,CurrentStates,Transition,NumberOfFiles,length+1,AcceptStates,NumberOfAcceptStates
			,dfa,mypid,NumberOfProcessors,wFile,id,status);
		//call InputGenerator for "oldstring" +b
	}
	
}

 
main(
int argc,
char **argv)
{
  int mypid;
  int numprocs;
  int t1=0,t2=0,t3=0,t4=0,t5=0;


	int NumberOfProcessors,NumberOfFiles,NumberOfStates,MAX_LENGTH;
	int NumberOfAcceptStates[10];
	int i=0,j=0,k=0;
	char FileNames[10];
	char s[10][25][100];
	char AcceptStates[10][10];
	char StartState[10];
	char Transition[10][20][3];
	char CurrentStates[10];
	char Input[10];
	int tag=0;
	FILE * hFile;						//pointer for input files
	FILE * wFile;						//pointer for output file

	MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&mypid);

	wFile = fopen( "output.dat", "w");
   

	if(numprocs<5||numprocs>20) //check if the number of processors are between 5 and 20
	{
		MPI_Finalize();
		printf("\nError: number of processors should be between 5 and 20\n");
		
		return 0;
	}

	if(argc<5 || argc >12)	//check if the number of dfas is between 3 and 10
	{
		MPI_Finalize();
		printf("\nError: number of dfas should be between 3 and 10\n");		
		return 0;
	}

	if(mypid==1)
    {
		MAX_LENGTH =atoi(argv[1]);
		NumberOfFiles=argc -2;
		NumberOfProcessors=numprocs;
	
		MPI_Send(&MAX_LENGTH,1,MPI_INT,2,tag,MPI_COMM_WORLD); //sends the information to the processor#2
		MPI_Send(&NumberOfFiles,1,MPI_INT,2,tag,MPI_COMM_WORLD);
		MPI_Send(&NumberOfProcessors,1,MPI_INT,2,tag,MPI_COMM_WORLD);
	
	}
  
	if(mypid==2)
	{
			
	MPI_Recv(&MAX_LENGTH,1,MPI_INT,1,tag,MPI_COMM_WORLD,&status);//receives the information from processor#1
	MPI_Recv(&NumberOfFiles,1,MPI_INT,1,tag,MPI_COMM_WORLD,&status);
	MPI_Recv(&NumberOfProcessors,1,MPI_INT,1,tag,MPI_COMM_WORLD,&status);


	for(i=0;i<NumberOfFiles;i++)
	{

		FileNames[i]=argv[i+2][0];
		hFile = fopen( argv[i+2], "r");

		if (hFile == NULL)
		{
			// Error, file not found
			printf("null----\n",mypid);
		}
		else                                           //start reading file
		{
			// Process & close file			
			fgets(s[i][0],80,hFile);						//read file line by line line1 : alphabet

			fgets(s[i][1],80,hFile);						//line2 : number of states
			NumberOfStates=(strlen(s[i][1])-6)/3;		
		
			fgets(s[i][2],80,hFile);						//line3 : start state
			StartState[i] =s[i][2][strlen(s[i][2])-2];
			CurrentStates[i]=StartState[i];

			fgets(s[i][3],80,hFile);						//line4: accept states
			NumberOfAcceptStates[i]=(strlen(s[i][3])-20)/3;
			for(j=0;j<NumberOfAcceptStates[i];j++)
				AcceptStates[i][j]=s[i][3][22+j*3];

			fgets(s[i][4],80,hFile);						//line 5: title....

			for(k=0;k<NumberOfStates*2;k++)					//rest of the lines are transition functions
			{
				fgets(s[i][5+k],80,hFile);
				Transition[i][k][0]=s[i][5+k][1];
				Transition[i][k][1]=s[i][5+k][3];
				Transition[i][k][2]=s[i][5+k][6];
				Transition[i][k][3]='\0';
			}
			fclose(hFile);									//close input file
		}														
	}	

		MPI_Bcast(&NumberOfFiles, 1, MPI_INT, 2, MPI_COMM_WORLD);
		MPI_Bcast(&NumberOfStates, 1, MPI_INT, 2, MPI_COMM_WORLD);
		MPI_Bcast(&MAX_LENGTH, 1, MPI_INT, 2, MPI_COMM_WORLD);
		MPI_Bcast(&CurrentStates, 10, MPI_CHAR, 2, MPI_COMM_WORLD);
		for(i=0;i<NumberOfFiles;i++)
		{
			for(k=0;k<NumberOfStates*2;k++)					//rest of the lines are transition functions
			{
				MPI_Bcast(&Transition[i][k], 3, MPI_CHAR, 2, MPI_COMM_WORLD);
			}
		}	
		for(i=0;i<NumberOfFiles;i++)
		{

				MPI_Bcast(&AcceptStates[i],10, MPI_CHAR, 2, MPI_COMM_WORLD);

		}
		MPI_Bcast(&NumberOfAcceptStates, 10, MPI_INT, 2, MPI_COMM_WORLD);
		MPI_Bcast(&FileNames, 10, MPI_CHAR, 2, MPI_COMM_WORLD);
		MPI_Bcast(&NumberOfProcessors,1,MPI_INT,2,MPI_COMM_WORLD);

	}


		MPI_Bcast(&NumberOfFiles, 1, MPI_INT, 2, MPI_COMM_WORLD);
		MPI_Bcast(&NumberOfStates, 1, MPI_INT, 2, MPI_COMM_WORLD);
		MPI_Bcast(&MAX_LENGTH, 1, MPI_INT, 2, MPI_COMM_WORLD);
		MPI_Bcast(&CurrentStates, 10, MPI_CHAR, 2, MPI_COMM_WORLD);
		for(i=0;i<NumberOfFiles;i++)
		{
			for(k=0;k<NumberOfStates*2;k++)					//rest of the lines are transition functions
			{
				MPI_Bcast(&Transition[i][k], 3, MPI_CHAR, 2, MPI_COMM_WORLD);
			}
		}
		
		for(i=0;i<NumberOfFiles;i++)
		{

				MPI_Bcast(&AcceptStates[i],10, MPI_CHAR, 2, MPI_COMM_WORLD);

		}
		MPI_Bcast(&NumberOfAcceptStates, 10, MPI_INT, 2, MPI_COMM_WORLD);
		MPI_Bcast(&FileNames, 10, MPI_CHAR, 2, MPI_COMM_WORLD);
		MPI_Bcast(&NumberOfProcessors,1,MPI_INT,2,MPI_COMM_WORLD);

		Input[0]='\0';	


		InputGenerator(Input,MAX_LENGTH,CurrentStates,Transition,NumberOfFiles,0,AcceptStates,NumberOfAcceptStates,FileNames
,mypid,NumberOfProcessors,wFile,1,status); //calls the input generator function
	
	
	MPI_Finalize();
 }
