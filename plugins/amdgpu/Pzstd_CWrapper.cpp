#include "Pzstd.h"
#include "Pzstd_CWrapper.h"
#include "Options.h"
#include <stdlib.h>

using namespace pzstd;

#ifdef __cplusplus
extern "C" {
#endif

//TODO: 此处可能会出错，存在重复定义的风险
struct Options4C{
    Options* obj;
};

void pzstdMain_C(void* arg){
    OptionRef options = (OptionRef) arg;
    pzstdMain(*(options->obj));
}

#ifdef __cplusplus
}
#endif