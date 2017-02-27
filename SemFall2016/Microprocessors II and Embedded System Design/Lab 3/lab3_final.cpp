
/*cAPTURE IMAGE USING OPENCV LIBRARY*/

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include <iostream>
#include <cstdio>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

//#define PDATA 				0x9C
#define TEMP_SENSOR			0x48
#define GESTURE_SENSOR			0x39

#define TMP_RD 				0x93
#define TMP_WR 				0x92
#define TMP_REG 			0x00

unsigned char gesture_getValue();
float temp_getValue();
void imageCapture();
int gesture();

using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{
	
	while(1)
    {
	        //unsigned char gesture_result = gesture_getValue();
		float temp_result = temp_getValue();
		sleep(1);
		int gesture_result = gesture();
		sleep(1);
		//float temp_result = 0.0;
	
	
		if( temp_result > 27.0 || (gesture_result >50 && gesture_result < 255))
		{
			imageCapture();
			sleep(0.5);
			printf("Image captured\n");
		}
	}
	return 0;
}

int gesture()
{
	int result;
	FILE *fp;
	fp = fopen("gout.txt", "r");
	fscanf(fp, "%d",&result);
	fclose(fp);
	printf("Gesture = %d\n", result);
	return (result);
}
unsigned char gesture_getValue()
{
        int i;
        int r;
        int fhand;
        unsigned char value[2] ={0,0} ;
        useconds_t delay = 2000;

        const char *dev = "/dev/i2c-0";
        int addr = 0x39;

        fhand = open(dev, O_RDWR );
        if(fhand < 0)
        {
                perror("ERROR: Opening i2c device node\n");
                return 1;
        }

        r = ioctl(fhand, I2C_SLAVE, addr);
        if(r < 0)
        {
                perror("ERROR: Selecting i2c device\n");
        }
		
		char command[3] = {0x80,0b01011111, 0x9c};
		r = write(fhand, command ,3);
        if(r != 3)
			printf("ERROR: Writing I2C device\n");
	
        for(i=0;i<2;i++)
		{
		    value[0] = 0x9c;
			value[1] = 0x00;
			r = read(fhand, &value[i], 1);
            if(r != 1)
            {
                perror("ERROR: reading i2c device\n");
            }
		//	usleep(delay);
		}
		unsigned char result = value[1];
		printf("Gesture = %d\n", result);
        
        close(fhand);
        return result;
}



float temp_getValue()
{
    int i;
    int r;
    int fd;
    float result = 0.0;
    char value[2] ={0} ;
    useconds_t delay = 2000;

    const char *dev = "/dev/i2c-0";
    int addr = 0x48;

    fd = open(dev, O_RDWR );
    if(fd < 0)
    {
        perror("Opening i2c device node\n");
        return 1;
    }

    r = ioctl(fd, I2C_SLAVE, addr);
    if(r < 0)
    {
        perror("Selecting i2c device\n");
    }
	char command[3] = {TMP_WR,TMP_REG,TMP_RD};
    		        
    for(i=0;i<2;i++)
    {
            r = read(fd, &value[i], 1);
            if(r != 1)
            {
                perror("reading i2c device\n");
            }
            usleep(delay);
    }
		
		float  tlow =0;
		tlow = (float)(((value[0] << 8) | value[1]) >> 4);
		result = 0.0625*(tlow);
        printf("Temperature: %f\n",result);
   

        close(fd);
        return result;
}

void imageCapture()
{
    VideoCapture cap(0); // open the video camera no. 0

    if (!cap.isOpened())  // if not success, exit program
    {
        cout << "ERROR: Cannot open the video file" << endl;
        
    }
 
   double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
   double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video

   cout << "Frame Size = " << dWidth << "x" << dHeight << endl;

   vector<int> compression_params; //vector that stores the compression parameters of the image

   compression_params.push_back(CV_IMWRITE_JPEG_QUALITY); //specify the compression technique

   compression_params.push_back(95); //specify the jpeg quality

	 
	 
   Mat img(dWidth, dHeight, CV_8UC1);
   cap.read(img);
	 
   bool bSuccess = imwrite("/media/card/img.jpg", img, compression_params); //write the image to file


   if ( !bSuccess )
   {
        cout << "ERROR : Failed to save the image" << endl;
   }
}
