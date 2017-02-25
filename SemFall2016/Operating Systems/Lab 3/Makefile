all:program1 program2 program3 program4

program1:
	gcc firstfit.c -pthread -o firstfit

program2:
	gcc bestfit.c -pthread -o bestfit

program3:
	gcc worstfit.c -pthread -o worstfit

program4:
	gcc firstfit_defrag.c -pthread -o firstfit_defrag


clean:
	rm*.o firstfit bestfit worstfit firstfit_defrag
