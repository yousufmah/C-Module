#include "mutex_peterson.h"

/* mutex.h contains the specification of functions in this file and
 * the type that mutex_t is an alias for */

mutex_t* mutex_new(proc_t* proc) {
    return ipc_new(proc, "mux_peters", sizeof(mutex_peterson_t));
}

void mutex_enter(mutex_t* mux) {
    if (!mux) return;
        
    mutex_peterson_t* mp = (mutex_peterson_t*) mux->addr;
    
    int norm_pid = mux->proc->id % 2;
    
    int other_pid = 1 - norm_pid;
    mp->interested[norm_pid] = 1;
    mp->turn = mux->proc->id;

    while (mp->turn == mux->proc->id && mp->interested[other_pid]) ;
}

void mutex_leave(mutex_t* mux) {
    if (!mux) return;

    mutex_peterson_t* mp = (mutex_peterson_t*) mux->addr;

    int norm_pid = mux->proc->id % 2;
    
    mp->interested[norm_pid] = 0;
}

void mutex_delete(mutex_t* mux) {
    ipc_delete(mux);
}
