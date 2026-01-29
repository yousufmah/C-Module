#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include "joblog.h"

static char* new_log_name(proc_t* proc) {
    static char* joblog_name_fmt = "%s/%.31s%07d.txt";
                                // string format for the name of a log file
                                // declared static to have only one instance

    if (!proc)
        return NULL;

    char* log_name;
            
    asprintf(&log_name, joblog_name_fmt, JOBLOG_PATH, proc->type_label,
        proc->id);

    return log_name;
}

int joblog_init(proc_t* proc) {
    if (!proc) {
        errno = EINVAL;
        return -1;
    }
        
    int r = 0;
    if (proc->is_init) {
        struct stat sb;
    
        if (stat(JOBLOG_PATH, &sb) != 0) {
            errno = 0;
            r = mkdir(JOBLOG_PATH, 0777);
        }  else if (!S_ISDIR(sb.st_mode)) {
            unlink(JOBLOG_PATH);
            errno = 0;
            r = mkdir(JOBLOG_PATH, 0777);
        }
    }

    joblog_delete(proc);    // in case log exists for proc
    
    return r;
}

job_t* joblog_read(proc_t* proc, int entry_num, job_t* job) {
    int init_errno = errno;
    char* log_name = new_log_name(proc);
    
    if (entry_num < 0 || !log_name) {
        errno = init_errno;
        return NULL;
    }
    
    FILE* lf = fopen(log_name,  "r");
    
    if (!lf) {
        free(log_name);
        errno = init_errno;
        return NULL;
    }

    job_t* entry_job = NULL;

    if (!fseek(lf, entry_num * JOB_STR_SIZE, SEEK_SET)) {
        char entry[JOB_STR_SIZE];
        
        if (fgets(entry, JOB_STR_SIZE, lf)) {
            entry[JOB_STR_SIZE - 1] = '\0';
            entry_job = str_to_job(entry, job);
        }
    }
           
    fclose(lf);
    free(log_name);
    errno = init_errno;
    
    return entry_job;
}

void joblog_write(proc_t* proc, job_t* job) {
    if (!job)
        return;
    
    int init_errno = errno;
    
    char job_str[JOB_STR_SIZE];
    
    if (job_to_str(job, job_str)) {
        char* log_name = new_log_name(proc);
        
        if (log_name) {
            FILE* lf = fopen(log_name,  "a");
    
            if (lf) {
                fprintf(lf, "%s\n", job_str);
                fclose(lf);
            }
        }

        free(log_name);
    }
    
    errno = init_errno;
    
    return;
}

void joblog_delete(proc_t* proc) {
    int init_errno = errno;
    char* log_name = new_log_name(proc);
    
    if (log_name) {
        unlink(log_name);
    
        free(log_name);
    }  
    
    errno = init_errno;
}