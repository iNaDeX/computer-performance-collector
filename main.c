#include "processInfo.h"

// how to represent start_time
// is it normal to have almost only 0's for processes < 300 ?
// handle error codes for missing files/impossible to read / empty file / missing values / cast conversions

int main(int argc, char *argv[]) {
    // variables
    DIR* dp = NULL;
    struct dirent* dirp = NULL;
    char* path = "/proc";

    // init
    if ((dp = opendir(path)) == NULL) {
        fprintf(stderr, "can’t open %s", path);
        exit(EXIT_FAILURE);
    }

    time_t raw_time;
    time(&raw_time);
    printf("SAMPLE TIME: %ld\n", raw_time); // printing raw timestamp solves Timezone issue and allows easy comparisons between different TZ data
    /*printf("PID,PPID,PGID,r_UID,e_UID,r_GID,\
e_GID,start_time,utime,stime,resident_set_size,\
text_mem_size,data_mem_size,shared_mem_size,library_memy_size,maj_faults,\
min_faults,read_bytes,write_bytes,full_command\n");*/

    // iterate through each process
    while ((dirp = readdir(dp)) != NULL)
    {
        if(isProcessFile(dirp->d_name)) // if the file is a process directory
        {
            t_processInfo* newP = create_new_processInfo(dirp->d_name); // create new data structure
            read_stat_file(newP);// read files
            read_statm_file(newP);
            read_status_file(newP);
            read_io_file(newP);
            read_cmdline_file(newP);
            print_process_info(newP); // output stats
            free_processInfo(&newP);
        }
    }

    closedir(dp);
    return 0;
}
