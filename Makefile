tinytop:	tinytop.c tt_process_monitor.c tt_utilities.c
	gcc tinytop.c -c
	gcc tinytop.o tt_process_monitor.c tt_utilities.c -o tinytop

