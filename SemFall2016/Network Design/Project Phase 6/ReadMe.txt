
/*--------------------------------------------Contributors---------------------------------------------*/

   - Naga Ganesh Kurrapati         
   - Ritesh Dumpala Basaveswara    


/*--------------------------------------------Program Files-------------------------------------------*/

 **This phase demostrates working of selective repeat protocol in JAVA programming using clien-server architecture 
   UDPClient.java
   
   -   The purpose of this file is to send data (.bmp file) from the client to server which 
       is read through a input file stream, send the file through a UDP datagram socket, 
       to receive the response (.bmp file) from the server and to write it to an output file stream. 
 
   UDPServer.java
 
   -   The purpose of this file is to receive data (.bmp file) from the client through an 
       UDP data gram socket which is written to an output file stream, send the response 
       (.bmp file) to the client which is read from output file stream.  


/*-------------------------------Steps to set up and execute the program------------------------------*/


Step 1 :   javac UDPServer.java    / To compile the server program (setting up the server) /
 
Step 2 :   java UDPServer          / To execute the server program /
 
Step 3 :   javac UDPClient.java    / To compile the client program (Setting up the client) /

Step 4 :   java UDPClient          / TO execute the client program /

( Note : *Perform the above steps in different folders for client and server
         *The name of the .bmp file in the client folder is ex3.bmp

/*-------------------------------------------References------------------------------------------------*/

  [1] Lecture notes - Network Design by Vinod Vokkarane
  [2] Java The Complete Reference (Ninth edition) by Herbert Schildt 
  
