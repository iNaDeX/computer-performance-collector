#include "processInfo.h"

// source: https://stackoverflow.com/questions/1540627/what-api-do-i-call-to-get-the-system-uptime
long get_uptime()
{
    struct sysinfo s_info;
    int error = sysinfo(&s_info);
    if(error != 0)
    {
        fprintf(stderr, "error reading uptime= %d\n", error);
    }
    return s_info.uptime;
}

t_processInfo* create_new_processInfo(char* PID) {
    t_processInfo* n = calloc(1, sizeof(t_processInfo)); // create & init at 0
    n->PID = strtol(PID, NULL, 10); // set PID
    n->full_command = NULL;
    return n;
}

void read_statm_file(t_processInfo* p) {
    char filePath[FILENAME_MAX];
    char fileContent[BUFFER_SIZE];
    FILE* fp = NULL;

    sprintf(filePath, "/proc/%lu/%s", p->PID, "statm");
    fp = fopen(filePath, "r"); // open file
    if(fp==NULL) // if error opening
    {
        fprintf(stderr,"error opening data file: %s\n", filePath);
        fprintf(stderr,"%s\n",strerror(errno));
        return;
    }
    if(fgets(fileContent,BUFFER_SIZE,fp)==NULL && errno!=0) // if error reading
    {
        fprintf(stderr,"error reading data file: %s\n", filePath);
        fprintf(stderr,"%s\n",strerror(errno));
        fclose(fp);
        return;
    }

    int pageSize = sysconf(_SC_PAGESIZE); // get page size information
    char * pch;
    unsigned int counter = 0;
    pch = strtok (fileContent," ");
    while (pch != NULL) // parse file and save data
    {
        counter++;
        switch(counter){
            case 2:
            p->resident_set_size = strtol(pch, NULL, 10) * pageSize;
            break;
            case 3:
            p->shared_memory_size = strtol(pch, NULL, 10) * pageSize;
            break;
            case 4:
            p->text_memory_size = strtol(pch, NULL, 10) * pageSize;
            break;
            case 5:
            p->library_memory_size = strtol(pch, NULL, 10) * pageSize;
            break;
            case 6:
            p->data_memory_size = strtol(pch, NULL, 10) * pageSize;
            break;
        }
        pch = strtok (NULL, " ");
    }

    fclose(fp);
}

void read_stat_file(t_processInfo* p) {
    char filePath[FILENAME_MAX];
    char fileContent[BUFFER_SIZE];
    FILE* fp = NULL;

    sprintf(filePath, "/proc/%lu/%s", p->PID, "stat");
    fp = fopen(filePath, "r");
    if(fp==NULL)
    {
        fprintf(stderr,"error opening data file: %s\n", filePath);
        fprintf(stderr,"%s\n",strerror(errno));
        return;
    }
    if(fgets(fileContent,BUFFER_SIZE,fp)==NULL && errno!=0)
    {
        fprintf(stderr,"error reading data file: %s\n", filePath);
        fprintf(stderr,"%s\n",strerror(errno));
        fclose(fp);
        return;
    }

    char * pch;
    unsigned int counter = 0;
    pch = strtok (fileContent," ");
    time_t runningFor;
    while (pch != NULL)
    {
        counter++;
        switch(counter){
            case 4:
            p->PPID = strtol(pch, NULL, 10);
            break;
            case 5:
            p->PGID = strtol(pch, NULL, 10);
            break;
            case 10:
            p->minor_faults = strtol(pch, NULL, 10);
            break;
            case 12:
            p->major_faults = strtol(pch, NULL, 10);
            break;
            case 14:
            p->CPU_user_time = strtof(pch, NULL) / sysconf(_SC_CLK_TCK);
            break;
            case 15:
            p->CPU_system_time = strtof(pch, NULL) / sysconf(_SC_CLK_TCK);
            break;
            case 22:
            runningFor = get_uptime() - ( strtol(pch, NULL, 10) / sysconf(_SC_CLK_TCK) );
            p->start_time = difftime(time(NULL),runningFor);
            break;
        }
        pch = strtok (NULL, " ");
    }

    fclose(fp);
}

void read_status_file(t_processInfo* p) {
    char filePath[FILENAME_MAX];
    char fileContent[BUFFER_SIZE];
    FILE* fp = NULL;

    sprintf(filePath, "/proc/%lu/%s", p->PID, "status");
    fp = fopen(filePath, "r");
    if(fp==NULL)
    {
        fprintf(stderr,"error opening data file: %s\n", filePath);
        fprintf(stderr,"%s\n",strerror(errno));
        return;
    }

    while(fgets(fileContent,BUFFER_SIZE,fp)!=NULL)
    {
        char key[BUFFER_SIZE];
        unsigned long value1 = 0;
        unsigned long value2 = 0;
        sscanf(fileContent,"%s %lu\t%lu", key, &value1, &value2);
        if(strcmp(key, "Uid:")==0) {
            p->real_user_ID = value1;
            p->effective_user_ID = value2;
        }
        else if(strcmp(key, "Gid:")==0) {
            p->real_group_ID = value1;
            p->effective_group_ID = value2;
        }
    }

    fclose(fp);
}

void read_cmdline_file(t_processInfo* p) {
    char filePath[FILENAME_MAX];
    char fileContent[BUFFER_SIZE];
    fileContent[0] = '\0';
    FILE* fp = NULL;

    sprintf(filePath, "/proc/%lu/%s", p->PID, "cmdline");
    fp = fopen(filePath, "r");
    if(fp==NULL)
    {
        fprintf(stderr,"error opening data file: %s\n", filePath);
        fprintf(stderr,"%s\n",strerror(errno));
        return;
    }
    if(fgets(fileContent,BUFFER_SIZE,fp)==NULL && errno!=0)
    {
        fprintf(stderr,"error reading data file: %s\n", filePath);
        fprintf(stderr,"%s\n",strerror(errno));
        fclose(fp);
        return;
    }

    p->full_command = strdup(fileContent);
    fclose(fp);
}

void read_io_file(t_processInfo* p) {
    char filePath[FILENAME_MAX];
    char fileContent[BUFFER_SIZE];
    FILE* fp = NULL;

    sprintf(filePath, "/proc/%lu/%s", p->PID, "io");
    fp = fopen(filePath, "r");
    if(fp==NULL)
    {
        fprintf(stderr,"error opening data file: %s\n", filePath);
        fprintf(stderr,"%s\n",strerror(errno));
        return;
    }

    while(fgets(fileContent,BUFFER_SIZE,fp)!=NULL)
    {
        char key[BUFFER_SIZE];
        unsigned long value = 0;
        sscanf(fileContent, "%s %lu", key, &value);
        if(strcmp(key, "read_bytes:")==0) {
            p->read_bytes = value;
        }
        else if(strcmp(key, "write_bytes:")==0) {
            p->write_bytes += value;
        }
        else if(strcmp(key, "cancelled_write_bytes:")==0) {
            p->write_bytes -= value;
        }
    }

    fclose(fp);
}

void print_process_info(t_processInfo* p) {

    char date[BUFFER_SIZE];
    struct tm * pt = localtime(& p->start_time);
    strftime(date, BUFFER_SIZE, "%F %T", pt);

    printf("%lu, %lu, %lu, %lu, %lu, %lu, %lu, %lu, \
%f, %f, %lu, %lu, %lu, %lu, %lu, %lu, %lu, %lu, %lu, %s\n",
    p->PID,
    p->PPID,
    p->PGID,
    p->real_user_ID,
    p->effective_user_ID,
    p->real_group_ID,
    p->effective_group_ID,
    p->start_time,
    p->CPU_user_time,
    p->CPU_system_time,
    p->resident_set_size,
    p->text_memory_size,
    p->data_memory_size,
    p->shared_memory_size,
    p->library_memory_size,
    p->major_faults,
    p->minor_faults,
    p->read_bytes,
    p->write_bytes,
    p->full_command == NULL ? "": p->full_command
    );
}

void free_processInfo(t_processInfo** p) {
	free((*p)->full_command);
	free(*p);
	*p = NULL;
}

int isProcessFile(char* fileName) {
    int i=0;
    for(i=0; i<strlen(fileName); i++)
    {
        if(!isdigit(fileName[i]))
            return FALSE;
    }
    return TRUE;
}
