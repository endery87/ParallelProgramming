ENDER YUNUS
2004101303
	
CMPE 300- PROGAMMING PROJECT
?	Introduction 
 In this project I wrote a program which creates input strings and checks if they are accepted by DFAs which are kept  in files. Then the program creates an output file of the results. 
 The aim of the project is using parallel algorithms. 


DFA file Format:
alphabet a,b	//fixed for all DFA files
states q0 q1 q2 q3
start state q2
set of accept states q1
transition function
q0 a q1
q0 b q1
q1 a q3
q1 b q1..

User Input On Command Line :

 > mpiexec –n P program.exe ML A.dfa B.dfa C.dfa

P-> Number of processors
ML->Maximum length for input strings

Output File Format( follows a lexicographic order):
  B                                              
a  A B      // meaning that input “a” is accepted by A and B dfas.                                     
aa  B                                            
aaa  B                                           
aab  A B                                         
ab  A B                                          
aba  B…    

Restrictions:
1.	The processor number must be between 5 and 20.
2.	The number of files containing dfa’s must be between 3 and 10.
     

?	Description of My Program

I wrote my program in C language.  I used the following header files:
"mpi.h"
<stdio.h>
<string.h>
<stdlib.h>

 I used 2 functions for this project.  My functions are :
?	MAIN

 This function takes command line inputs. First thing it does is creating as many processors as wanted. Then it checks whether the number of processors are between 5 and 20 . The second control it does is checking the number of dfas.  If one of those controls fail the program prints an error message and ends. 
  If there is no problem with inputs, program converts command line inputs into predefined variables. This job is done by processor#1. Then this processor sends this information to processor #2 which does the file reading.
  Processor#2 reads the dfa file line by line. It stores the information of each dfa in in arrays( such as StartStates[] , Transition[][][] …).  After reading all dfa files, it uses broadcast function of MPI to send the information of dfa files to all processors. It broadcasts the NumberOfStates, AcceptStates, CurrentStates, Transition functions,   FileName of each dfa. 
   Then each processor calls InputGenerator function. After that mpi finalizes and program ends. 

?	INPUTGENERATOR

 This function creates input in a lexicographical order in a recursive way. The logic of this function is that it starts with empty string and checks if it is accepted by any of the dfas.  Then it creates two new strings(by adding ‘a‘ and ‘b’ to end of the old string. It calls the input generator by these 2 new strings. As it keeps the last position of the old string, the position of the new string is found by only one transition function. 
 This function uses processor#0 for output string creation. For each string one output string is created. Processor#0 creates a string of length 50 character. It fills the beginning of the string with input name.  Rest is filled by ‘ ‘ character.  It sends the output string to processor #3 which does the input string creation and dfa check.
Processor#3 initially checks the last character of the input string and updates its current states for each dfa by using the transition function. Then it checks whether the current state of the string is an accept state of dfa or not. If it is an accept state it adds the name of the dfa to the output string. It does this operation for each dfa. After it ends additions to ouput string. It prints the output string to the output file(output.dat).
 Then processor#3 makes recursive calls of inputgenerator. It does this step only if the current input is smaller than MAX_LENGTH. If it is, processor #3 calls inputgenerator with updated inputs (oldinput + ‘a’ and oldinput+’b’).

NOTES:

 I used Microsoft Visiual C++ 6.0 compiler for this project.


