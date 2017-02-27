/* 
File: sobelp.cpp
Purpose: Describes all the functions used to detect the horizontal and vertical images present in a .ppm extension file by making use of the concepts of sobel filter
*/

#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdint.h>
#include <fcntl.h>
#include "string.h"
#include "unistd.h"

//This is the default case when no file name is given 
#define DEFAULT_FILENAME "/home/kapeed/Documents/deepak/cali.ppm"

void *processing(void* rank);

unsigned int *read_ppm( char *filename, int * xsize, int * ysize, int *maxval )
{  
  if ( !filename || filename[0] == '\0') {
    fprintf(stderr, "read_ppm but no file name\n");
    return NULL;  // fail
  }
  
  FILE *fp;
  
  printf("Reading file: %s\n", filename);
  fp = fopen( filename, "rb");
  if (!fp)
    {
      fprintf(stderr, "read_ppm()    ERROR  file '%s' cannot be opened for reading\n", filename);
      return NULL; // fail
    }
  
  char chars[100];

  //read the bytes of the input file and collect them in an array named: chars
  int num = fread(chars, sizeof(char), 100, fp);
  
  if (chars[0] != 'P' || chars[1] != '6')
    {
      fprintf(stderr, "Texture::Texture()    ERROR  file '%s' does not start with \"P6\"  I am expecting a binary PPM file\n", filename);
      return NULL;
    }
  
  unsigned int width, height, maxvalue;
  
  //This will calculate and determine the width, height and the maximum value present in the input file
  char *ptr = chars+3; // P 6 newline
  if (*ptr == '#') // comment line!
    ptr = 1 + strstr(ptr, "\n");
  
  num = sscanf(ptr, "%d\n%d\n%d",  &width, &height, &maxvalue);

  printf("Read %d things >> width: %d, height: %d, maxval: %d\n\n", num, width, height, maxvalue);
  *xsize = width;
  *ysize = height;
  *maxval = maxvalue;
  
  unsigned int *pic = (unsigned int *)malloc( width * height * sizeof(unsigned int));
  if (!pic) {
    fprintf(stderr, "read_ppm()  unable to allocate %d x %d unsigned ints for the picture\n", width, height);
    return NULL; // fail but return
  }
  
  // allocate buffer to read the rest of the file into
  int bufsize =  3 * width * height * sizeof(unsigned char);
  if ((*maxval) > 255) bufsize *= 2;
  unsigned char *buf = (unsigned char *)malloc( bufsize );
  if (!buf) {
    fprintf(stderr, "read_ppm()  unable to allocate %d bytes of read buffer\n", bufsize);
    return NULL; // fail but return
  }
  
  char duh[80];
  char *line = chars;
  
  //All the below sprintf's are adding the values present in the xsize, ysize, maxval pointers and storing them into an array called duh, this also adds a null character at the end
  sprintf(duh, "%d", *xsize);
  sprintf(duh, "%c", '\0');
  line = strstr(line, duh);
  line += strlen(duh) + 1;
  
  sprintf(duh, "%d", *ysize);
  sprintf(duh, "%c", '\0');
  line = strstr(line, duh);
  line += strlen(duh) + 1;
  
  sprintf(duh, "%d", *maxval);
  sprintf(duh, "%c", '\0');
  line = strstr(line, duh);
  
  line += strlen(duh) + 1;
  
  long offset = line - chars;
  fseek(fp, offset, SEEK_SET); // move to the correct offset
  long numread = fread(buf, sizeof(char), bufsize, fp);
  printf("Texture: %s Read %ld of %d bytes\n\n", filename, numread, bufsize);
  fclose(fp);
  
  int pixels = (*xsize) * (*ysize);
  for (int i=0; i<pixels; i++) 
    pic[i] = (int) buf[3*i];  // red channel
  
  return pic; // success
}

void write_ppm( char *filename, int xsize, int ysize, int maxval, int *pic)
{
  FILE *fp;
  int x,y;
  
  //open the file to write the values in it, the file in our case will be the final file containing the output
  fp = fopen(filename, "w");
  if (!fp)
    {
      fprintf(stderr, "FAILED TO OPEN FILE '%s' for writing\n", filename);
      exit(-1);
    }
  
  fprintf(fp, "P6\n");
  fprintf(fp,"%d %d\n%d\n", xsize, ysize, maxval);
  
  int numpix = xsize * ysize;
  for (int i=0; i<numpix; i++) {
    unsigned char uc = (unsigned char) pic[i];
    fprintf(fp, "%c%c%c", uc, uc, uc);
  }
  fclose(fp);
}

int xsize, ysize, maxval;
unsigned int *pic;
int *result = NULL;
int *imagetmp1 = NULL;
int *imagetmp2 = NULL;
int thread_count;

int main( int argc, char **argv )
{
  pthread_t* thread_handles;
  char *filename;
  char path[] = "/home/kapeed/Documents/deepak/result.ppm";
  filename = strdup( DEFAULT_FILENAME);
  
  if (argc == 3)  { // filename
    thread_count=atoi(argv[1]);
    filename = strdup( argv[2]);
  }
  
  if (argc == 2) 
    thread_count = atoi(argv[1]);
  
  printf("Input Filename: %s\n\n", filename);

  //assigning memory to the variables
  pic = read_ppm((char *) filename, &xsize, &ysize, &maxval );
  result = (int*) malloc( xsize * ysize * 3 * sizeof( int) );
  imagetmp1 = (int*) malloc( xsize * ysize * 3 * sizeof( int ) );
  imagetmp2 = (int*) malloc( xsize * ysize * 3 * sizeof( int ) );
  
  if (!result) {
    fprintf(stderr, "sobel() unable to malloc %ld bytes\n", 4*xsize * ysize * 3 * sizeof( int ));
    exit(-1); // fail
  }
  
  int magnitude, sum1, sum2;
  clock_t begin, end;
  double time_spent;
  int *out = result;
  
  for (int col=0; col<ysize; col++) {
    for (int row=0; row<xsize; row++) {
      *out++ = 0;
    }}
  
  thread_handles = (pthread_t*)malloc(thread_count*sizeof(pthread_t));
    
  //begin the clock
  begin = clock();
  
  printf("Number of threads working: %d\n", thread_count);

  for(long i=0;i<thread_count;i++)
    pthread_create(&thread_handles[i], NULL, processing, (void*)i);
    
  for(long i=0;i<thread_count;i++)
    pthread_join(thread_handles[i], NULL);
  
  //end the clock
  end = clock();
  write_ppm( (char *) path, xsize, ysize, 255, result);
  
  //calculate the execution time
  time_spent = (double)(end - begin) / CLOCKS_PER_SEC; 
 
  printf("Task Successfully Completed\n");

  printf("\nTotal Time Taken = %f\n",time_spent);
}

void *processing(void* rank)
{
  int r, c;
  int my_q,my_r;
  long first_r,last_r,temp;
  my_q = ysize/thread_count;
  my_r = ysize%thread_count;

  //This is the method by which we will figure out till what value should each thread use the data passed into it.
  if((long)rank< my_r)
    {
      temp=my_q + 1;
      first_r=(long)rank * temp;
    }
  else
    {
      temp=my_q;
      first_r=(long)rank*temp + my_r;
    }
  last_r = first_r + temp;
  
  /*In our case we are dividing the according to its height, i.e., if the height is of 100 bytes and the number of threads working on it are 4 then each thread will work on 100/4 = 25 bytes*/

  //The below loops are for applying the horizontal mask
  for(r=first_r+1;r<=last_r-1;r++){
    for(c=1;c<xsize-1;c++){
      int offset = r*xsize + c;
      imagetmp1[r*xsize+c] = (pic[(r-1)*(xsize)+(c-1)])*(-1) + (pic[(r-1)*xsize+c])*(-2) + (pic[(r-1)*xsize+c+1])*(-1) +
	(pic[r*xsize+(c-1)])*(0) + (pic[r*xsize+c])*(0) + (pic[r*(xsize)+c+1])*(0) +
	(pic[(r+1)*(xsize)+(c-1)])*(1) + (pic[(r+1)*xsize+ c])*(2) + (pic[(r+1)*xsize+c+1])*(1);
    }}
    
  //The below loops are for applying the vertical mask
  for(r=first_r+1;r<=last_r-1;r++){
    for(c=1;c<xsize-1;c++){
      int offset = r*xsize + c;
      imagetmp2[r*xsize+c] = pic[(r-1)*(xsize)+(c-1)]*(-1) + pic[(r-1)*xsize+c]*(0) + pic[(r-1)*xsize+c+1]*(1) +
	pic[r*xsize+(c-1)]*(-2) + pic[r*xsize+c]*(0) + pic[r*(xsize)+c+1]*(2) +
	pic[(r+1)*(xsize)+(c-1)]*(-1) + pic[(r+1)*xsize+ c]*(0) + pic[(r+1)*xsize+c+1]*(1);
    }}

  //The below loop will find the resultant image after combining the values obtained from applying the horizontal and vertical loops
  for(r=first_r;r<last_r;r++){
    for(c=1;c<xsize-1;c++){
      
      result[r*xsize+c] = sqrt(imagetmp1[r*xsize+c]*imagetmp1[r*xsize+c] + imagetmp2[r*xsize+c]*imagetmp2[r*xsize+c]);
      result[r*xsize+c] = ceil(result[r*xsize+c]);
    }}
}
