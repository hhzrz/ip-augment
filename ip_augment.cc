#include "ip_augment.h"
#include <string>
#include <iostream>

using namespace std;

static ip2region_entry ip2rEntry;
static datablock_entry datablock;
static string dbFilePath;

bool LoadDbFile() {
    if ( ip2region_create(&ip2rEntry, &dbFilePath[0]) == 0 ) {
        cerr << "Error: Fail to create the ip2region object with db file(" << dbFilePath << ")." << endl;
        return false;
    }
    return true;
}

int GetIp_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    if (argc != 2) return RedisModule_WrongArity(ctx);

    memset(&datablock, 0x00, sizeof(datablock_entry));

    size_t strlen;
    const char *ip = RedisModule_StringPtrLen(argv[1], &strlen);
    auto * ipStr = new char[strlen + 1];
    strncpy(ipStr, ip, strlen);
    ipStr[strlen] = '\0';

    if (ip2region_memory_search_string(&ip2rEntry, ipStr, &datablock)) {
        // success
        RedisModule_ReplyWithArray(ctx, 2);
        RedisModule_ReplyWithLongLong(ctx, datablock.city_id);
        RedisModule_ReplyWithSimpleString(ctx, datablock.region);
    } else {
        // fail to search
        RedisModule_ReplyWithNull(ctx);
    }

    delete[] ipStr;
    return REDISMODULE_OK;
}

int Reload_DbFile_Callback(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    REDISMODULE_NOT_USED(argv);
    REDISMODULE_NOT_USED(argc);
    auto *myint = (int*)RedisModule_GetBlockedClientPrivateData(ctx);
    return RedisModule_ReplyWithLongLong(ctx,*myint);
}

int Timeout_Str_Callback(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    REDISMODULE_NOT_USED(argv);
    REDISMODULE_NOT_USED(argc);
    return RedisModule_ReplyWithSimpleString(ctx,"Request timedout");
}

int ReloadIp_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    REDISMODULE_NOT_USED(argv);
    REDISMODULE_NOT_USED(argc);
    long long timeout = 5000;
    RedisModuleBlockedClient *bc = RedisModule_BlockClient(ctx,Reload_DbFile_Callback,Timeout_Str_Callback, nullptr, timeout);
    auto *r = (int*)RedisModule_Alloc(sizeof(int));
    *r = rand();

    bool status = LoadDbFile();

    RedisModule_UnblockClient(bc,r);
    return status ? REDISMODULE_OK : REDISMODULE_ERR;
}

/* This function must be present on each Redis module. It is used in order to
 * register the commands into the Redis server. */
int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    if (RedisModule_Init(ctx,"ip_augment_module",1,REDISMODULE_APIVER_1)
        == REDISMODULE_ERR) return REDISMODULE_ERR;

    /* Log the list of parameters passing loading the module. */
    for (int j = 0; j < argc; j++) {
        const char *s = RedisModule_StringPtrLen(argv[j], nullptr);
        printf("Module loaded with ARGV[%d] = %s\n", j, s);
    }

    if(argc != 1)
        return REDISMODULE_ERR;

    size_t strlen;
    const char *str = RedisModule_StringPtrLen(argv[0], &strlen);
    dbFilePath = string(str, strlen);
    if (!LoadDbFile())
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx,"getip",
                                  GetIp_RedisCommand,"readonly",0,0,0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx,"reloadip",
                                  ReloadIp_RedisCommand,"",0,0,0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    return REDISMODULE_OK;
}
