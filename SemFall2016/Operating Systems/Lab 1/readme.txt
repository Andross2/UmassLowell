/*--------------------------------------------Contributors---------------------------------------------*/

   - Naga Ganesh Kurrapati         


/*--------------------------------------------Program Files-------------------------------------------*/

  
   1a.c
   
   -   The purpose of this file is to design a parent process to repeatedly create child 
       processes and waiting for child to finish its job. The child displays that it is
       running and exits.
 
   1b.c
 
   -   The purpose of this file is to similar to program 1a, but the difference is that the
       child spawns another program 'b' using execve.
   
   b.c
 
   -   The purpose of this file is to spawns another program 'ls' using execve.

   2.c
   -   The purpose of this file is to create a program consists of parents and child processes
       that display its process number in sequence and process id


/*-------------------------------Steps to set up and execute the program------------------------------*/


Step 1 :   type 'make' in the command line   / To compile the programs /
 
Step 2 :   type './1a' or './1b' or './2'    / To execute the single program /

	or 
 
Step 1 :   type 'gcc -o 1a 1a.c'    / To compile the programs /
           type 'gcc -o 1b 1b.c' 
           type 'gcc -o b b.c' 
           type 'gcc -o 2 2.c' 

Step 2 :   type './1a' or './1b' or './b' or './2'    / To execute the single program /




