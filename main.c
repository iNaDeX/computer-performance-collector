#include "processInfo.h"

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
            print_process_info(newP); // output stats (no need to modify endianness to Network Byte Order because ASCII 1byte strings)
            free_processInfo(&newP);
        }
    }

    closedir(dp);
    return 0;
}
