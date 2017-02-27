//headers for lab 2
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>
#include<fcntl.h>
// headers for client
#include <curl/curl.h> 
#include <sys/stat.h>
#include <pthread.h>
//Headers for gesture sensor
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <mraa.h>

#define PDATA 0x9C
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

//#define PDATA 			0x9C
#define TEMP_SENSOR			0x48
#define GESTURE_SENSOR		0x39

#define TMP_RD 				0x93
#define TMP_WR 				0x92
#define TMP_REG 			0x00
//golbals
int ldr_value;
int local_ldr_value;
float temp_result;

void *ldr(void *id);
void *gesture_thread(void *arg);
float temp_getValue();
void imageCapture();
int gesture();
void HTTP_POST(const char* url, const char* image, int size);
void send_pic();

using namespace cv;
using namespace std;
pthread_mutex_t mutex,mutex2;

int main(int argc, char* argv[])
{
	pthread_mutex_init(&mutex,NULL);
	pthread_mutex_init(&mutex2,NULL);
	int pid=0,pid2=1;
	pthread_t g,ld;
	pthread_attr_t attr,attr2;
	pthread_attr_init(&attr);
	pthread_attr_init(&attr2);
	pthread_create(&g, NULL, gesture_thread, (void *)pid);
	sleep(1);
	pthread_create(&ld, NULL, ldr, (void *)pid2);
	sleep(1);
	
	while(1)
    {
	        //unsigned char gesture_result = gesture_getValue();
		temp_result = temp_getValue();
		sleep(1);
		int gesture_result = gesture();
		sleep(1);
		//float temp_result = 0.0;
		pthread_mutex_lock(&mutex);
		local_ldr_value = ldr_value;
		pthread_mutex_unlock(&mutex);
		//printf("ADC value = %d\n", local_ldr_value);
	
		if( temp_result > 27.0 || (gesture_result >50 && gesture_result <= 255))
		{
			imageCapture();
			send_pic();
			sleep(0.5);
			printf("Image captured\n");
		}
	}
	pthread_join(g, NULL);
	pthread_join(ld, NULL);
	pthread_mutex_destroy(&mutex);
	pthread_mutex_destroy(&mutex2);
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

void *gesture_thread( void *arg)
{
        int i;
        int r;
        int fd;
        unsigned char value[2] ={0,0} ;
        useconds_t delay = 2000;

        const char *dev = "/dev/i2c-0";
        int addr = 0x39;
        pthread_mutex_lock(&mutex);
        fd = open(dev, O_RDWR );
        if(fd < 0)
        {
                perror("Gesture:Opening i2c device node\n");
               // return 1;
        }

        r = ioctl(fd, I2C_SLAVE, addr);
        if(r < 0)
        {
                perror("Gesture:Selecting i2c device\n");
        }
	char command[2] = {0x80,0b01011111};
	r = write(fd, command ,2);
        if(r != 2)
	  printf("ERROR(Gesture): Writing I2C device\n");
	pthread_mutex_unlock(&mutex);
	while(1)
        {      
	//	pthread_mutex_lock(&mutex);
                for(i=0;i<2;i++)
		{
			
                        value[0] = PDATA;
			value[1] = 0x00;
			r = read(fd, &value[i], 1);
                        if(r != 1)
                        {
                                perror("Gesture:reading i2c device\n");
                        }
                        usleep(delay);
		}
		FILE *fp;
		fp = fopen("gout.txt", "w");
		fprintf(fp,"%d\n", value[1]);
		fclose(fp);
		//printf("Gesture = %d\n", value[1]);
		//system("echo value[1] >> gout");
		//pthread_mutex_unlock(&mutex);
        }
        close(fd);
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
    pthread_mutex_lock(&mutex);
    fd = open(dev, O_RDWR );
    if(fd < 0)
    {
        perror("Temperature:Opening i2c device node\n");
        return 1;
    }

    r = ioctl(fd, I2C_SLAVE, addr);
    if(r < 0)
    {
        perror("Temp:Selecting i2c device\n");
    }
	char command[3] = {TMP_WR,TMP_REG,TMP_RD};
    		        
    for(i=0;i<2;i++)
    {
            r = read(fd, &value[i], 1);
            if(r != 1)
            {
                perror("Temp:reading i2c device\n");
            }
            usleep(delay);
    }
		
		float  tlow =0;
		tlow = (float)(((value[0] << 8) | value[1]) >> 4);
		result = 0.0625*(tlow);
        printf("Temperature: %f\n",result);
   

        close(fd);
	pthread_mutex_unlock(&mutex);
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
	 
   bool bSuccess = imwrite("img.jpg", img, compression_params); //write the image to file


   if ( !bSuccess )
   {
        cout << "ERROR : Failed to save the image" << endl;
   }
}
int convertStrToInt(char a)
{
	int b;
	if(a == '0')
	  b = 0;
	else if(a == '1')
	  b = 1;
	else
	  b = -1;

//	printf("conversion: a = %d\n", b);
	return b;


}

void write_gpio(int data)
{
	// make strobe high
	//printf("make strobe\n");
	system("./pin_out_strobe.sh 40 1"); // set GPIO_8 as output - Strobe
	//printf("after making strobe high\n");
	
	if(data == 0) // reset
	{
		system("./pin_out.sh 16 14 0"); // set GPIO_3 as output - D0
		system("./pin_out.sh 36 6 0"); // set GPIO_4 as output - D1
		system("./pin_out.sh 18 0 0"); // set GPIO_5 as output - D2
		system("./pin_out.sh 20 1 0"); // set GPIO_6 as output - D3
	}
	else if(data == 1) // ping
	{
		system("./pin_out.sh 16 14 1"); // set GPIO_3 as output - D0
		system("./pin_out.sh 36 6 0"); // set GPIO_4 as output - D1
		system("./pin_out.sh 18 0 0"); // set GPIO_5 as output - D2
		system("./pin_out.sh 20 1 0"); // set GPIO_6 as output - D3
	}
	else if(data == 2) // get
	{
		system("./pin_out.sh 16 14 0"); // set GPIO_3 as output - D0
		system("./pin_out.sh 36 6 1"); // set GPIO_4 as output - D1
		system("./pin_out.sh 18 0 0"); // set GPIO_5 as output - D2
		system("./pin_out.sh 20 1 0"); // set GPIO_6 as output - D3
	}
	
	printf("Send value: %d\n", data);
	sleep(0.01);
	
	// make strobe low
	system("./pin_out_strobe.sh 40 0"); // set GPIO_8 as output - Strobe
		
	system("./pin_out.sh 16 14 0"); // set GPIO_3 as output - D0
	system("./pin_out.sh 36 6 0"); // set GPIO_4 as output - D1
	system("./pin_out.sh 18 0 0"); // set GPIO_5 as output - D2
	system("./pin_out.sh 20 1 0"); // set GPIO_6 as output - D3
		
	sleep(0.01);
	
}

int read_gpio()
{
	int a;
	FILE *fp;
	
	// make strobe high
	system("./pin_out_strobe.sh 40 1"); // set GPIO_8 as output - Strobe
	
//	sleep(0.01);
	
	system("./pin_in.sh 16 14"); // set GPIO_3 as input - D0
	fp = fopen("out.txt", "r");
	a = convertStrToInt(fgetc(fp));
	
	system("./pin_in.sh 36 6"); // set GPIO_4 as input - D1
	fp = fopen("out.txt", "r");
	a = a |(convertStrToInt(fgetc(fp)) << 1);
	
	system("./pin_in.sh 18 0"); // set GPIO_5 as input - D2
	fp = fopen("out.txt", "r");
	a = a | (convertStrToInt(fgetc(fp)) << 2);
	
	system("./pin_in.sh 20 1"); // set GPIO_6 as input - D3
	fp = fopen("out.txt", "r");
	a = a |(convertStrToInt(fgetc(fp)) << 3);
	sleep(0.01);
	// make strobe low
	system("./pin_out_strobe.sh 40 0"); // set GPIO_8 as output - Strobe
	
	sleep(0.01);
	
	//system("./pin_out.sh 16 14 0"); // set GPIO_3 as output - D0
	//system("./pin_out.sh 36 6 0"); // set GPIO_4 as output - D1
	//system("./pin_out.sh 18 0 0"); // set GPIO_5 as output - D2
	//system("./pin_out.sh 20 1 0"); // set GPIO_6 as output - D3
	
	return a;
	
	
}

void init()
{
	system("./pin_out.sh 16 14 0"); // set GPIO_3 as output - D0
	system("./pin_out.sh 36 6 0"); // set GPIO_4 as output - D1
	system("./pin_out.sh 18 0 0"); // set GPIO_5 as output - D2
	system("./pin_out.sh 20 1 0"); // set GPIO_6 as output - D3
	system("./pin_out_strobe.sh 40 0"); // set GPIO_8 as output - Strobe
	
}

void *ldr(void *id)
{
	char cmd[200];
	int option, ack, nibl1, nibl2, nibl3, data = 0;
	pthread_mutex_lock(&mutex);
	init();
	pthread_mutex_unlock(&mutex);
	
	while(1)
	{
		pthread_mutex_lock(&mutex);
		//printf("Enter command\n1.MSG_RESET 2.MSG_PING 3. MSG_GET)");
		//scanf("%s", cmd);
		
		//printf("You have entered %s\n", cmd);
		
		/*if(strcmp("MSG_RESET", cmd) == 0)
		{
			option = 1;
		}
		else if(strcmp("MSG_PING", cmd) == 0)
		{
			option = 2;
		}
		else if(strcmp("MSG_GET", cmd) == 0)
		{
			option = 3;
		}
		else
		{
			option = 0;
		}
		*/
		option = 3;
		switch(option)
		{
			case 1: // reset
				write_gpio(0);
				ack = read_gpio();
				if(1)
				{
					printf("Received Value:%d\n", ack);
				}
			option = 0;
			break;
			
			case 2: // ping
				write_gpio(1);
				ack = read_gpio();
				if(1)
				{
					printf("Received Value:%d\n", ack);
				}
			option =0;
			break;
			
			case 3: // get
				write_gpio(2);
				nibl1 = read_gpio();
				nibl2 = read_gpio();
				nibl3 = read_gpio();
				ack = read_gpio();
				//printf("Nibl1:%d\n", nibl1);
				//printf("Nibl2:%d\n", nibl2);
				//printf("Nibl3:%d\n", nibl3);
				printf("Ack:%d\n", ack);
				
				data = 0;
				data = nibl1;
				data = (nibl2 << 4) | data;
				data = ((nibl3  & 0x3)<< 8)  | data;
      
				if(ack == 14)
				{
				  printf("ADC Result:%d\n", data);
				 // pthread_mutex_lock(&mutex2);
				  ldr_value = data;
				  //pthread_mutex_unlock(&mutex2);
				  
			    }
			   else
			   {
				printf("ADC Result without correct ack:%d\n", data);
				printf("Received ACk:%d\n", ack);
				  
			   }
			option = 0;
			break;


			default:
				option = 0;
			//	printf("ADC Result:%d\n", data);
			break;
		}
		pthread_mutex_unlock(&mutex);


		sleep(2);
	}
	
}

void HTTP_POST(const char* url, const char* image, int size){
	CURL *curl;
	CURLcode res;

	curl = curl_easy_init();
	if(curl){
		curl_easy_setopt(curl, CURLOPT_URL, url);
                curl_easy_setopt(curl, CURLOPT_POST, 1);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE,(long) size);
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, image);
		res = curl_easy_perform(curl);
		if(res != CURLE_OK)
      			fprintf(stderr, "curl_easy_perform() failed: %s\n",
              			curl_easy_strerror(res));
		curl_easy_cleanup(curl);
	}
}

void send_pic(){
	//time stamp
	time_t rawtime;
    struct tm * timeinfo;
    char timebuffer[16];
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    strftime (timebuffer,16,"%G%m%d%H%M%S",timeinfo);
	//tmp buffer
	char tbuffer[20];
	sprintf(tbuffer,"Temp value:%f",temp_result);
	
	const char* hostname="10.253.78.194";
	const int   port=8000;
	const int   id=1;
	const char* password="password";
	const char* name="Team+Awesome";
	const int   adcval=local_ldr_value;
	const char* status="Very+good+thank+you";
	const char* timestamp="20141116-09:12:34";
	const char* filename="img.jpg";

	char buf[1024*1024];
	snprintf(buf, 1024*1024, "http://%s:%d/update?id=%d&password=%s&name=%s&data=%d&status=%s&timestamp=%s&filename=%s",
			hostname,
			port,
			id,
			password,
			name,
			adcval,
			status,
            		timestamp,
			filename);
	FILE *fp;
	
	struct stat num;
	stat(filename, &num);
	int size = num.st_size;
	printf("Image size: %dB\n", size);	

	char *buffer = (char*)malloc(size);

	fp = fopen(filename,"rb");
	int n = fread(buffer, 1, size, fp);

	HTTP_POST(buf, buffer, size);
	fclose(fp);
}
