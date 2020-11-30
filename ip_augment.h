#include "header/redismodule.h"
#include "lib/ip2region.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);

#ifdef __cplusplus
}
#endif
