#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "job.h"

job_t* job_new(pid_t pid, unsigned int id, unsigned int priority, 
    const char* label) {
    return job_set((job_t*) malloc(sizeof(job_t)), pid, id, priority, label);
}

job_t* job_copy(job_t* src, job_t* dst) {
    if (!src || strnlen(src->label, MAX_NAME_SIZE) != MAX_NAME_SIZE - 1)
        return NULL;
    
    if (dst == src)
        return src;
        
    if (!dst)
        return job_new(src->pid, src->id, src->priority, src->label);
    
    *dst = *src;                   
    
    return dst;
}

// implementation of job_init using memset
void job_init(job_t* job) {
    if (job) {
        memset(job, 0, sizeof(job_t));
        (void) snprintf(job->label, MAX_NAME_SIZE, "%s", PAD_STRING);
    }
}

bool job_is_equal(job_t* j1, job_t* j2) {
    if (j1 == j2)
        return true;
    
    if (!j1 || !j2)
        return false;
    
    return j1->pid == j2->pid && j1->id == j2->id 
            && j1->priority == j2->priority
            && !strncmp(j1->label, j2->label, MAX_NAME_SIZE)
            && strnlen(j1->label, MAX_NAME_SIZE) == MAX_NAME_SIZE - 1;
}

job_t* job_set(job_t* job, pid_t pid, unsigned int id, unsigned int priority,
    const char* label) {
    if (job) {
        job->pid = pid;
        job->id = id;
        job->priority = priority;
        
        if (!label)
            label = "";

        (void) snprintf(job->label, MAX_NAME_SIZE, "%s%s", label, PAD_STRING);        
    }

    return job;
}

char* job_to_str(job_t* job, char* str) {
    if (!job || strnlen(job->label, MAX_NAME_SIZE) != MAX_NAME_SIZE - 1) 
        return NULL;
        
    char* str_buf = str ? str : calloc(JOB_STR_SIZE, sizeof(char)); 
    
    if (str_buf) {
        int r = snprintf(str_buf, JOB_STR_SIZE, JOB_STR_FMT, job->pid, job->id,
                job->priority, job->label);
                
        if (r == -1) {
            if (str_buf != str) {
                free(str_buf);
            }
            
            str_buf = NULL;
        }
    }
    
    return str_buf;
}

job_t* str_to_job(char* str, job_t* job) {
    if (!str || strnlen(str, JOB_STR_SIZE) != JOB_STR_SIZE - 1)
        return NULL;
    
    job_t* job_buf = job ? job : malloc(sizeof(job_t));
    
    if (job_buf) {
        int r = sscanf(str, JOB_STR_FMT, &job_buf->pid, &job_buf->id,
                &job_buf->priority, job_buf->label);
                
        if (r != 4 
            || strnlen(job_buf->label, MAX_NAME_SIZE) != MAX_NAME_SIZE - 1) {
            if (job_buf != job) {
                free(job_buf);
            }
            
            job_buf = NULL;
        }
    }
    
    return job_buf;
}

void job_delete(job_t* job) {
    free(job);
}