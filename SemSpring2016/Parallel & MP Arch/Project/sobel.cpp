#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "string.h"
#include<iostream>
#include<time.h>
#include<math.h>

using namespace std;

unsigned int *read_ppm( char *filename, int * xsize, int * ysize, int *maxval ){
  
  if ( !filename || filename[0] == '\0') {
    fprintf(stderr, "read_ppm but no file name\n");
    return NULL;  // fail
  }

  FILE *fp;

  fprintf(stderr, "read_ppm( %s )\n", filename);
  fp = fopen( filename, "rb");
  if (!fp) 
    {
      fprintf(stderr, "read_ppm()    ERROR  file '%s' cannot be opened for reading\n", filename);
      return NULL; // fail 

    }

  char chars[1024];
  //int num = read(fd, chars, 1000);
  int num = fread(chars, sizeof(char), 1000, fp);

  if (chars[0] != 'P' || chars[1] != '6') 
    {
      fprintf(stderr, "Texture::Texture()    ERROR  file '%s' does not start with \"P6\"  I am expecting a binary PPM file\n", filename);
      return NULL;
    }

  unsigned int width, height, maxvalue;


  char *ptr = chars+3; // P 6 newline
  if (*ptr == '#') // comment line! 
    {
      ptr = 1 + strstr(ptr, "\n");
    }

  num = sscanf(ptr, "%d\n%d\n%d",  &width, &height, &maxvalue);
  fprintf(stderr, "read %d things   width %d  height %d  maxval %d\n", num, width, height, maxvalue);  
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

  // find the start of the pixel data.   no doubt stupid
  sprintf(duh, "%d\0", *xsize);
  line = strstr(line, duh);
  line += strlen(duh) + 1;

  sprintf(duh, "%d\0", *ysize);
  line = strstr(line, duh);
  line += strlen(duh) + 1;

  sprintf(duh, "%d\0", *maxval);
  line = strstr(line, duh);


  fprintf(stderr, "%s found at offset %d\n", duh, line - chars);
  line += strlen(duh) + 1;

  long offset = line - chars;
  fseek(fp, offset, SEEK_SET); // move to the correct offset
  long numread = fread(buf, sizeof(char), bufsize, fp);
  fprintf(stderr, "Texture %s   read %ld of %ld bytes\n", filename, numread, bufsize); 

  fclose(fp);


  int pixels = (*xsize) * (*ysize);
  for (int i=0; i<pixels; i++) pic[i] = (int) buf[3*i];  // red channel

 
  printf("Success in reading\n");
  return pic; // success
}

void write_ppm( char *filename, int xsize, int ysize, int maxval, int *pic) 
{
  FILE *fp;
  int x,y;
  
  fp = fopen(filename, "w");
  if (!fp) 
    {
      fprintf(stderr, "FAILED TO OPEN FILE '%s' for writing\n");
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
	char *filename;
	filename = strdup( DEFAULT_FILENAME);
  
    if (argc == 3)  { // filename
	   filename = strdup( argv[1]);
    }
    if (argc == 2) { 
      
      filename = strdup( argv[1]);
    }

    fprintf(stderr, "file %s\n", filename); 


	int xsize, ysize, maxval;
	unsigned int *pic = read_ppm( filename, &xsize, &ysize, &maxval ); 
	
	
	int *result = (int*) malloc( xsize * ysize * 3 * sizeof( int ) );
	int *imagetmp1 = (int*) malloc( xsize * ysize * 3 * sizeof( int ) );
	int *imagetmp2 = (int*) malloc( xsize * ysize * 3 * sizeof( int ) );
	
	if (!result) { 
		fprintf(stderr, "sobel() unable to malloc %d bytes\n", 4*xsize * ysize * 3 * sizeof( int ));
		exit(-1); // fail
	}
	
	int r, c, magnitude, sum1, sum2; 
	clock_t begin, end;
	double time_spent;
	int *out = result;

	
	for (int col=0; col<ysize; col++) {
		for (int row=0; row<xsize; row++) { 
			*out++ = 0; 
		}
	}
	
	begin = clock();

	for(r=1;r<ysize-1;r++){
		for(c=1;c<xsize-1;c++){
		int offset = r*xsize + c;
		imagetmp1[r*xsize+c] = (pic[(r-1)*(xsize)+(c-1)])*(-1) + (pic[(r-1)*xsize+c])*(-2) + (pic[(r-1)*xsize+c+1])*(-1) +	
							(pic[r*xsize+(c-1)])*(0) + (pic[r*xsize+c])*(0) + (pic[r*(xsize)+c+1])*(0) +
							(pic[(r+1)*(xsize)+(c-1)])*(1) + (pic[(r+1)*xsize+ c])*(2) + (pic[(r+1)*xsize+c+1])*(1);
      }
   }
   
      for(r=1;r<ysize-1;r++){
      for(c=1;c<xsize-1;c++){
		int offset = r*xsize + c;
		imagetmp2[r*xsize+c] = pic[(r-1)*(xsize)+(c-1)]*(-1) + pic[(r-1)*xsize+c]*(0) + pic[(r-1)*xsize+c+1]*(1) +	
						 pic[r*xsize+(c-1)]*(-2) + pic[r*xsize+c]*(0) + pic[r*(xsize)+c+1]*(2) +
						 pic[(r+1)*(xsize)+(c-1)]*(-1) + pic[(r+1)*xsize+ c]*(0) + pic[(r+1)*xsize+c+1]*(1);
      }
   }
   
      for(r=1;r<ysize-1;r++){
      for(c=1;c<xsize-1;c++){
         
	result[r*xsize+c] = sqrt(imagetmp1[r*xsize+c]*imagetmp1[r*xsize+c] + imagetmp2[r*xsize+c]*imagetmp2[r*xsize+c]);
	result[r*xsize+c] = ceil(result[r*xsize+c]);
      }
   }

  end = clock();  
  write_ppm( "/home1/03995/tg832943/project/result.ppm", xsize, ysize, 255, result);

  time_spent = (double)(end - begin) / CLOCKS_PER_SEC;  
  cout<<"Time = "<<time_spent<<endl<<endl;
  fprintf(stderr, "done\n"); 
   
}

