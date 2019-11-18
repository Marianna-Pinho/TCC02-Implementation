#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<stdio.h>


/*==============================================
FUNCTION: CPU_usage
INPUT: void
OUTPUT: it returns a float
DESCRIPTION: this function returns the use of the
cpu since the boot time, read from the /proc/stat
file. To get the percentage, the returned number
needs to be multiplied by 100.
================================================*/
float CPU_usage(){
    int total = 0, idle = 0, cpu = 0, i = 0;
    float pidle = 0;
    char line[105];
    
    FILE *p = fopen("/proc/stat","r");
    fscanf(p, "%[^\n]", line);
    
    strtok(line, " ");
    for(i = 0; i < 7; i ++){
        if(i == 3){
            idle = atoi(strtok(NULL, " "));
            total+=idle;
            continue;
        }
        
        total += atoi(strtok(NULL, " "));
    }

    fclose(p);
    pidle = (float)idle/total;

    return 1.0 - pidle;
}

/*==============================================
FUNCTION: MEM_usage
INPUT: void
OUTPUT: it returns a float
DESCRIPTION: this function returns the use of memory,
which is read from the /proc/meminfo file. To get the
percentage, the returned number needs to be multiplied
by 100.
================================================*/
float MEM_usage(){
    int total = 0, mfree = 0, mbuf = 0, mcache = 0, mused = 0, i = 0;
    size_t  len = 0;
    float pidle = 0;
    char *line;
    
    FILE *p = fopen("/proc/meminfo","r");
    
    if(getline(&line,&len,p) != -1){
        strtok(line," ");
        total = atoi(strtok(NULL, " "));
        //printf("Total: %d\n", total);
    }

    if(getline(&line,&len,p) != -1){
        strtok(line," ");
        mfree = atoi(strtok(NULL, " "));
        //printf("MemFree: %d\n", mfree);
    }

    getline(&line,&len,p);
    if(getline(&line,&len,p) != -1){
        strtok(line," ");
        mbuf = atoi(strtok(NULL, " "));
        //printf("Buffers: %d\n", mbuf);
    }

    if(getline(&line,&len,p) != -1){
        strtok(line," ");
        mcache = atoi(strtok(NULL, " "));
        //printf("Caches: %d\n", mcache);
    }

    fclose(p);
    return total - (mfree + mbuf + mcache); //KB
}