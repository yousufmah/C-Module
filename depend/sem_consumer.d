objects/sem_consumer.o: sem_consumer.c sim_control.h proc.h sim_config.h job.h \
  sem_jobqueue.h ipc_jobqueue.h pri_jobqueue.h ipc.h | objects
	$(CC) -c $(CFLAGS) $< -o $@
