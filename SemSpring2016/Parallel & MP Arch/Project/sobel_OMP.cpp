/* 
File: sobel_OMP.cpp
Purpose: Describes all the functions used to detect the horizontal and vertical images present in a .ppm extension file by making use of the concepts of sobel filter
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "string.h"
#include<iostream>
#include<time.h>
#include<omp.h>
#include <math.h>

using namespace std;

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
  
  //read the bytes of the input file and collect them in an array named: chars
  char chars[16384];
  int num = fread(chars, sizeof(char), 1000, fp);

  if (chars[0] != 'P' || chars[1] != '6') 
    {
      fprintf(stderr, "Texture::Texture()    ERROR  file '%s' does not start with \"P6\"  I am expecting a binary PPM file\n", filename);
      return NULL;
    }
  
  unsigned int width, height, maxvalue;
  
  //This will calculate and determine the width, height and the maximum value present in the input file
  char *ptr = chars+3; // P 6 newline

  if (*ptr == '#') // comment line! 
    {
      ptr = 1 + strstr(ptr, "\n");
    }
  
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

int main( int argc, char **argv )
{
  clock_t begin, end;
  double stime=omp_get_wtime();
  
  int nthreads = atoi(argv[1]);
  char *filename;
   
  filename = strdup( argv[2]);
  printf("Input Filename: %s\n", filename); 

  printf("Number of Threads Working:%d\n", nthreads);    
    
  int xsize, ysize, maxval;
  unsigned int *pic = read_ppm( filename, &xsize, &ysize, &maxval ); 
  
  int *result1 = (int*) malloc( xsize * ysize * 3 * sizeof( int ) );
  int *result2 = (int*) malloc( xsize * ysize * 3 * sizeof( int ) );
  int *result = (int*) malloc( xsize * ysize * 3 * sizeof( int ) );
  
  if (!result) { 
    fprintf(stderr, "sobel() unable to malloc %lu bytes\n", 4*xsize * ysize * 3 * sizeof( int ));
    exit(-1); // fail
  }
  
  int i, j, magnitude, sum1, sum2; 
  
  double time_spent;
  int *out = result;
  int tid;
  
  for (int col=0; col<ysize; col++) {
    for (int row=0; row<xsize; row++) { 
      *out++ = 0; 
    }}
  
#pragma omp parallel num_threads(nthreads) shared(pic,nthreads) private(tid,i,j)
  {
    tid = omp_get_thread_num();
    
    for (i = (tid*(ysize)/nthreads)+1;  i <= (tid+1)*(ysize)/nthreads; i++) {
      for (j = 1; j < xsize -1; j++) {
	result1[i*xsize+j] = (pic[(i-1)*(xsize)+(j-1)])*(-1) + (pic[(i-1)*xsize+j])*(-2) + (pic[(i-1)*xsize+j+1])*(-1) +	
	  (pic[i*xsize+(j-1)])*(0) + (pic[i*xsize+j])*(0) + (pic[i*(xsize)+j+1])*(0) +
	  (pic[(i+1)*(xsize)+(j-1)])*(1) + (pic[(i+1)*xsize+ j])*(2) + (pic[(i+1)*xsize+j+1])*(1);
      }
    }  
  }
  
#pragma omp parallel num_threads(nthreads) shared(pic,nthreads) private(tid,i,j)
  {
    tid = omp_get_thread_num();
    
    for (i = (tid*(ysize)/nthreads)+1;  i <= (tid+1)*(ysize)/nthreads; i++) {
      for (j = 1; j < xsize -1; j++) {
	result2[i*xsize+j] = pic[(i-1)*(xsize)+(j-1)]*(-1) + pic[(i-1)*xsize+j]*(0) + pic[(i-1)*xsize+j+1]*(1) +	
	  pic[i*xsize+(j-1)]*(-2) + pic[i*xsize+j]*(0) + pic[i*(xsize)+j+1]*(2) +
	  pic[(i+1)*(xsize)+(j-1)]*(-1) + pic[(i+1)*xsize+ j]*(0) + pic[(i+1)*xsize+j+1]*(1);
      }
    }
    
  }
  
#pragma omp parallel num_threads(nthreads) shared(nthreads) private(tid,i,j)
  {
    tid = omp_get_thread_num();
    
    for (i = (tid*(ysize)/nthreads)+1;  i <= (tid+1)*(ysize)/nthreads; i++) {
      for (j = 1; j < xsize -1; j++) {
	result[i*xsize+j] = sqrt(result1[i*xsize+j]*result1[i*xsize+j] + result2[i*xsize+j]*result2[i*xsize+j]);
      }
    }	
  }
  
  double etime=omp_get_wtime();
  write_ppm((char *)"/home/kapeed/Documents/deepak/result.ppm", xsize, ysize, 255, result);
  
  double total=etime-stime;

  printf("Task Successfully Completed\n");
  cout<<"Total Time Taken="<<total<<endl;	
  
}

