#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>
#include<fcntl.h>

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

int main()
{
	char cmd[200];
	int option, ack, nibl1, nibl2, nibl3, data = 0;
	init();
	
	while(1)
	{
		printf("Enter command\n1.MSG_RESET 2.MSG_PING 3. MSG_GET)");
		scanf("%s", cmd);
		
		printf("You have entered %s\n", cmd);
		
		if(strcmp("MSG_RESET", cmd) == 0)
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
				printf("Nibl1:%d\n", nibl1);
				printf("Nibl2:%d\n", nibl2);
				printf("Nibl3:%d\n", nibl3);
				printf("Ack:%d\n", ack);
				
				data = 0;
				data = nibl1;
				data = (nibl2 << 4) | data;
				data = ((nibl3  & 0x3)<< 8)  | data;
      
				if(ack == 14)
				{
				  printf("ADC Result:%d\n", data);
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
	}
	
}