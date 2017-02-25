/*--------------------------------------------Contributors---------------------------------------------*/

   - Naga Ganesh Kurrapati         


/*--------------------------------------------Program Files-------------------------------------------*/

  
   1.c
   
   -   The purpose of this file is to intialize producer and buyer threads.Then producer procduce item
       and puts it on the queue. Consumer thread consume the item. The synchronization is achieved using
       one mutex and two semaphores
 

   2.c
   -   The purpose of this file is to intialize producer and buyer threads.Then producer procduce item
       and puts it on the queue. Consumer thread consume the item. The synchronization is achieved using
       one mutex and one semaphores. But in this case we can eliminate one semphore because we have only
       one producer.


/*-------------------------------Steps to set up and execute the program------------------------------*/


Step 1 :   type 'make' in the command line   / To compile the programs /
 
Step 2 :   type './1 260' or './2 6'    / To execute the single program /

	or 
 
Step 1 :   type 'gcc 1.c -pthread -o 1'    / To compile the programs /
           type 'gcc 2.c -pthread -o 2'

Step 2 :   type './1 260' or './2 6'   / To execute the single program /


Note: We needed to supply number of buyers for the two programs from the command line
as shown in the excution steps.



