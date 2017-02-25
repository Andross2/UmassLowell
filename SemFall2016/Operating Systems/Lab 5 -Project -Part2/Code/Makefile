all:program1 program2 program3

program1:
	gcc adaptrr.c ready_queue.h -o adaptrr -lpthread

program2:
	gcc effectrr.c ready_queue.h -o effectrr -lpthread
	
program3:
	gcc standrr.c ready_queue.h -o standrr -lpthread

clean:
	rm*.o standrr effectrr adaptrr
