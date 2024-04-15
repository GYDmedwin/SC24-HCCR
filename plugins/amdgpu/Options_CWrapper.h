#pragma once

#include <stddef.h>
#include <sys/time.h>
#include "thpool.h"

#ifdef __cplusplus  
extern "C"{   
#endif

struct Options4C;
typedef struct  Options4C* OptionRef;

struct Options_Args
{   
    enum WriteMode { Regular, Auto, Sparse } writeMode;

    unsigned numThreads;
    unsigned maxWindowLog;
    unsigned compressionLevel;
    bool decompress;
    void* input_buffer;
    uint64_t input_len;
    FILE* outputFile;
    bool overwrite;
    bool keepSource;
    bool checksum;
    int verbosity;
    int id;
    void* isLast;
    int data_type; // 0为主存数据，1为显存数据
};


OptionRef getOption(struct Options_Args args);

int getMyFlag(void);
void getGPUpool(threadpool thpool);
void setGPUpoolWait(void);
struct thpool_* setCPUpool(void);

void write_wait(int id, int data_type);
void write_notify(int data_type);
void setFlagZero(void);
void setLastCompressTime(struct timeval last_time);
void setWriteTime(double mtime);


#ifdef __cplusplus  
}  
#endif