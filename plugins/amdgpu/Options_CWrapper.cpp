#include "Options.h"
#include "Options_CWrapper.h"
#include <stdlib.h>
#include <mutex>  
#include <condition_variable> 
// #include <chrono>
#include <sys/time.h>

using namespace pzstd;

#ifdef __cplusplus
extern "C" {
#endif

static std::mutex mutex_GPU;
static std::mutex mutex_CPU;
static std::condition_variable cond_GPU;
static std::condition_variable cond_CPU;
static int GPU_flag = 1;
static int CPU_flag = 0;
static double write_time = 0;
static threadpool gpu_pool;

// std::chrono::duration<double> dur_write;
// std::chrono::_V2::steady_clock::time_point now_write;

struct timeval compress_start, compress_end;

static int count = 0;

struct Options4C{
    Options* obj;
};

OptionRef getOption(struct Options_Args args){
    OptionRef ref = (OptionRef)malloc(sizeof (struct Options));
    ref->obj = new Options();
    ref->obj->parse(args);
    return ref;
}

void write_wait(int id, int data_type){
    if(data_type == 1){
        std::unique_lock<std::mutex> glk(mutex_GPU);
        cond_GPU.wait(glk, [=] {
            return id == GPU_flag; 
        });
    }
    else if(data_type == 0){
        if(count == 0){
            //获取压缩开始时间
            gettimeofday(&compress_start, NULL);
            count++;
        }
        std::unique_lock<std::mutex> clk(mutex_CPU);
        cond_CPU.wait(clk, [=] {
            return id == CPU_flag; 
        });
    }
    
}

void write_notify(int data_type){
    if(data_type == 1){
        GPU_flag += 1;
        cond_GPU.notify_all();
    }
    else if(data_type == 0){
        CPU_flag += 1;
        cond_CPU.notify_all();
    }
}

void setFlagZero(void){
    // GPU_flag = 1;
    printf("数据总写入时间：%.3fs\n", write_time);
    write_time = 0;
    double execution_time = (compress_end.tv_sec - compress_start.tv_sec) +
                            (compress_end.tv_usec - compress_start.tv_usec) / 1000000.0;
    printf("数据总压缩时间：%.3fs\n\n", execution_time);
    count = 0; 
}

void setLastCompressTime(struct timeval last_time){
    compress_end = last_time;
}

void setWriteTime(double mtime){
    write_time += mtime;
}

int getMyFlag(void){
    return CPU_flag;
}

void getGPUpool(threadpool thpool){
    gpu_pool = thpool;
    if(gpu_pool == NULL){
		printf("1111111\n");
	}
}

struct thpool_* setCPUpool(void){
    return gpu_pool;
}

void setGPUpoolWait(void){
    thpool_wait(gpu_pool);
    thpool_destroy(gpu_pool);
}

#ifdef __cplusplus
}
#endif