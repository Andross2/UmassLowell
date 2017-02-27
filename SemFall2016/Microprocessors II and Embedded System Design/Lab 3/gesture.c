#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <mraa.h>

#define PDATA 0x9C
int main ()
{
        int i;
        int r;
        int fd;
        unsigned char value[2] ={0,0} ;
        useconds_t delay = 2000;

        char *dev = "/dev/i2c-0";
        int addr = 0x39;

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
	char command[2] = {0x80,0b01011111};
	r = write(fd, command ,2);
        if(r != 2)
	  printf("ERROR: Writing I2C device\n");
	
	while(1)
        {      
                for(i=0;i<2;i++)
		{
			
                        value[0] = PDATA;
			value[1] = 0x00;
			r = read(fd, &value[i], 1);
                        if(r != 1)
                        {
                                perror("reading i2c device\n");
                        }
                        usleep(delay);
		}
		FILE *fp;
		fp = fopen("gout.txt", "w");
		fprintf(fp,"%d\n", value[1]);
		fclose(fp);
		//printf("Gesture = %d\n", value[1]);
		//system("echo value[1] >> gout");
        }
        close(fd);
        return 0;
}
