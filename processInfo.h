#ifndef PROCESSINFO_H_INCLUDED
#define PROCESSINFO_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <dirent.h>
#include <unistd.h>
#include <linux/kernel.h>       /* for struct sysinfo */
#include <sys/sysinfo.h>

#define FALSE 0
#define TRUE 1
#define BUFFER_SIZE 1024

typedef struct processInfo {
    unsigned long PID;
    unsigned long PPID;
    unsigned long PGID;
    unsigned long real_user_ID;
    unsigned long effective_user_ID;
    unsigned long real_group_ID;
    unsigned long effective_group_ID;
    time_t start_time; // (timespec.tv_sec)
    float CPU_user_time; //(floating number in seconds)
    float CPU_system_time; // (floating number in seconds)
    unsigned long resident_set_size; // (in bytes)
    unsigned long text_memory_size; // (in bytes)
    unsigned long data_memory_size;// (in bytes)
    unsigned long shared_memory_size;// (in bytes)
    unsigned long library_memory_size;// (in bytes)
    unsigned long major_faults;
    unsigned long minor_faults;
    unsigned long read_bytes;
    unsigned long write_bytes;
    char* full_command;
} t_processInfo;

t_processInfo* create_new_processInfo(char* PID);
void free_processInfo(t_processInfo** p);
long get_uptime();
void read_statm_file(t_processInfo* p);
void read_stat_file(t_processInfo* p);
void read_status_file(t_processInfo* p);
void read_cmdline_file(t_processInfo* p);
void read_io_file(t_processInfo* p);
void print_process_info(t_processInfo* p);
int isProcessFile(char* fileName);


#endif // PROCESSINFO_H_INCLUDED
