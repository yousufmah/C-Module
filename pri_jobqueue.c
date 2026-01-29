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
 * O(1) implementation of dequeue.
 */
job_t* pri_jobqueue_dequeue(pri_jobqueue_t* pjq, job_t* dst) {
    dst = pri_jobqueue_peek(pjq, dst);
    
    if (dst) {       
        job_init(&pjq->jobs[pjq->size - 1]);
        pjq->size--;
    }
    
    return dst;
}

/* 
 * O(N) implementation of enqueue.
 */
void pri_jobqueue_enqueue(pri_jobqueue_t* pjq, job_t* job) {
    job_t job_to_nq; // to copy the job to test success early
    
    if (!job || !job->priority || pri_jobqueue_is_full(pjq)
        || !job_copy(job, &job_to_nq))
        return;
        
    int top = pjq->size;
    
    for (int i = 0; i < pjq->size; i++) {
        if (pjq->jobs[i].priority <= job->priority) {
            top = i;
            break;
        }
    }
        
    for (int i = pjq->size - 1; i > top - 1; i--)
        pjq->jobs[i+1] = pjq->jobs[i];

    pjq->jobs[top] = job_to_nq; // uses job copied at line 43 
    
    pjq->size++;
}
   
bool pri_jobqueue_is_empty(pri_jobqueue_t* pjq) {
    return !pjq || !pjq->size;
}

bool pri_jobqueue_is_full(pri_jobqueue_t* pjq) {
    return !pjq || pjq->size == pjq->buf_size;
}

/* 
 * O(1) implementation of peek.
 */
job_t* pri_jobqueue_peek(pri_jobqueue_t* pjq, job_t* dst) {
    return !pri_jobqueue_is_empty(pjq) && pjq->jobs[pjq->size - 1].priority
            ? job_copy(&pjq->jobs[pjq->size - 1], dst)
            : NULL;
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
