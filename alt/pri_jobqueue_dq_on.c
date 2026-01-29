#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "pri_jobqueue.h"

pri_jobqueue_t* pri_jobqueue_new() {
    pri_jobqueue_t* pjq = (pri_jobqueue_t*) malloc(sizeof(pri_jobqueue_t));
    
    pri_jobqueue_init(pjq);
    
    return pjq;
}

void pri_jobqueue_init(pri_jobqueue_t* pjq) {
    pjq->buf_size = JOB_BUFFER_SIZE;
    pjq->size = 0;
    
    for (int i = 0; i < pjq->buf_size; i++)
        job_init(&pjq->jobs[i]);
}

/* 
 * O(N) implementation of dequeue.
 */
job_t* pri_jobqueue_dequeue(pri_jobqueue_t* pjq, job_t* dst) {
    if (pri_jobqueue_is_empty(pjq) || !pjq->jobs[0].priority) 
        return NULL;
        
    unsigned int curr_priority = pjq->jobs[0].priority;
    int top_idx = 0;
    
    for (int i = 0; i < pjq->size; i++) {
        if (pjq->jobs[i].priority < curr_priority) {
            curr_priority = pjq->jobs[i].priority;
            top_idx = i;
        }
    }
    
    dst = job_copy(&pjq->jobs[top_idx], dst);
    
    if (dst) {
        for (int i = top_idx; i < pjq->size - 1; i++)
            pjq->jobs[i] = pjq->jobs[i+1];
    
        job_init(&pjq->jobs[pjq->size - 1]);
        
        pjq->size--;
    }
    
    return dst;
}

/* 
 * O(1) implementation of enqueue.
 */
void pri_jobqueue_enqueue(pri_jobqueue_t* pjq, job_t* job) {
    if (!job || !job->priority || pri_jobqueue_is_full(pjq)
        || !job_copy(job, &pjq->jobs[pjq->size]))
        return;
        
    pjq->size++;
}

bool pri_jobqueue_is_empty(pri_jobqueue_t* pjq) {
    return !pjq || !pjq->size;
}

bool pri_jobqueue_is_full(pri_jobqueue_t* pjq) {
    return !pjq || pjq->size == pjq->buf_size;
}

/* 
 * O(N) implementation of peek.
 */
job_t* pri_jobqueue_peek(pri_jobqueue_t* pjq, job_t* dst) {
    if (pri_jobqueue_is_empty(pjq) || !pjq->jobs[0].priority) 
        return NULL;
        
    unsigned int curr_priority = pjq->jobs[0].priority;
    int top_idx = 0;
    
    for (int i = 0; i < pjq->size; i++) {
        if (pjq->jobs[i].priority < curr_priority) {
            curr_priority = pjq->jobs[i].priority;
            top_idx = i;
        }
    }
    
    return job_copy(&pjq->jobs[top_idx], dst);
}

int pri_jobqueue_size(pri_jobqueue_t* pjq) {
    return pjq ? pjq->size : 0;
}

int pri_jobqueue_space(pri_jobqueue_t* pjq) {
    return pjq ? pjq->buf_size - pjq->size : 0;
}

void pri_jobqueue_delete(pri_jobqueue_t* pjq) {
    free(pjq);
}
