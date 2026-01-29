objects/sim_control.o: sim_control.c sim_control.h proc.h sim_config.h job.h \
  joblog.h | objects
	$(CC) -c $(CFLAGS) $< -o $@
