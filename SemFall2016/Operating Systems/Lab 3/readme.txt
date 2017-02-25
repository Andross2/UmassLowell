/*--------------------------------------------Contributors---------------------------------------------*/

   - Naga Ganesh Kurrapati         


/*--------------------------------------------Program Files-------------------------------------------*/

  
   firstfit.c
   
   -   The purpose of this file is to intialize MMS and user threads. User thread posts the memory request on to the request buffer
       and memory free request on to the free buffer. MMU thread access those buffer to process the request and it uses a linked list 
       to keep track of memory blocks.The synchronization is achieved using mutexs and semaphores. In this case it uses firstfit algorithm 
       to assign the memory blocks.
 

   bestfit.c
   
   -   The purpose of this file is to intialize MMS and user threads. User thread posts the memory request on to the request buffer
       and memory free request on to the free buffer. MMU thread access those buffer to process the request and it uses a linked list 
       to keep track of memory blocks.The synchronization is achieved using mutexs and semaphores. In this case it uses bestfit algorithm 
       to assign the memory blocks.

   worstfit.c
   
   -   The purpose of this file is to intialize MMS and user threads. User thread posts the memory request on to the request buffer
       and memory free request on to the free buffer. MMU thread access those buffer to process the request and it uses a linked list 
       to keep track of memory blocks.The synchronization is achieved using mutexs and semaphores. In this case it uses worstfit algorithm 
       to assign the memory blocks.

   firstfit_defrag.c
   
   -   The purpose of this file is to intialize MMS and user threads. User thread posts the memory request on to the request buffer
       and memory free request on to the free buffer. MMU thread access those buffer to process the request and it uses a linked list 
       to keep track of memory blocks.The synchronization is achieved using mutexs and semaphores. In this case it uses firstfit algorithm 
       to assign the memory blocks. And it take cares of memory allocation to higher priority thread by kicking out the lower priorty thread.
       And it even uses defragmentation of memory blocks to meet the demand.


/*-------------------------------Steps to set up and execute the program------------------------------*/


Step 1 :   type 'make' in the command line   / To compile the programs /
 
Step 2 :   type './firstfit N' or './bestfit N' or './worstfit N' or './firstfit_defrag N'   / To execute the single program /

Note: N is number of users

Note: We needed to supply number of users for all the programs from the command line
as shown in the excution steps.



