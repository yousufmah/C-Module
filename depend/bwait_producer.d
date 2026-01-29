objects/bwait_producer.o: bwait_producer.c sim_control.h proc.h sim_config.h \
  job.h ipc_jobqueue.h pri_jobqueue.h ipc.h mutex.h | objects
	$(CC) -c $(CFLAGS) $< -o $@
