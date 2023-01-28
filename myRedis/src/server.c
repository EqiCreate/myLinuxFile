#include "server.h"
#include "config.h"
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "Main/mt19937-64.h"
// #include "functions.h"

/* Returns 1 if there is --sentinel among the arguments or if
 * executable name contains "redis-sentinel". */
int checkForSentinelMode(int argc, char **argv, char *exec_name) {
    if (strstr(exec_name,"redis-sentinel") != NULL) return 1;

    for (int j = 1; j < argc; j++)
        if (!strcmp(argv[j],"--sentinel")) return 1;
    return 0;
}

void initServerConfig(void) {
    int j;
    char *default_bindaddr[CONFIG_DEFAULT_BINDADDR_COUNT] = CONFIG_DEFAULT_BINDADDR;

    initConfigValues();
    // updateCachedTime(1);
    // getRandomHexChars(server.runid,CONFIG_RUN_ID_SIZE);
    // server.runid[CONFIG_RUN_ID_SIZE] = '\0';
    // changeReplicationId();
    // clearReplicationId2();
    server.hz = CONFIG_DEFAULT_HZ; /* Initialize it ASAP, even if it may get
                                      updated later after loading the config.
                                      This value may be used before the server
                                      is initialized. */
    server.timezone = getTimeZone(); /* Initialized by tzset(). */
    server.configfile = NULL;
    server.executable = NULL;
    server.arch_bits = (sizeof(long) == 8) ? 64 : 32;
    server.bindaddr_count = CONFIG_DEFAULT_BINDADDR_COUNT;
    for (j = 0; j < CONFIG_DEFAULT_BINDADDR_COUNT; j++)
        server.bindaddr[j] = zstrdup(default_bindaddr[j]);
    // memset(server.listeners, 0x00, sizeof(server.listeners));
    // server.active_expire_enabled = 1;
    // server.skip_checksum_validation = 0;
    // server.loading = 0;
    // server.async_loading = 0;
    // server.loading_rdb_used_mem = 0;
    // server.aof_state = AOF_OFF;
    // server.aof_rewrite_base_size = 0;
    // server.aof_rewrite_scheduled = 0;
    // server.aof_flush_sleep = 0;
    // server.aof_last_fsync = time(NULL);
    // server.aof_cur_timestamp = 0;
    // atomicSet(server.aof_bio_fsync_status,C_OK);
    // server.aof_rewrite_time_last = -1;
    // server.aof_rewrite_time_start = -1;
    // server.aof_lastbgrewrite_status = C_OK;
    // server.aof_delayed_fsync = 0;
    // server.aof_fd = -1;
    // server.aof_selected_db = -1; /* Make sure the first time will not match */
    // server.aof_flush_postponed_start = 0;
    // server.aof_last_incr_size = 0;
    // server.active_defrag_running = 0;
    // server.notify_keyspace_events = 0;
    // server.blocked_clients = 0;
    // memset(server.blocked_clients_by_type,0,
    //        sizeof(server.blocked_clients_by_type));
    // server.shutdown_asap = 0;
    // server.shutdown_flags = 0;
    // server.shutdown_mstime = 0;
    // server.cluster_module_flags = CLUSTER_MODULE_FLAG_NONE;
    // server.migrate_cached_sockets = dictCreate(&migrateCacheDictType);
    // server.next_client_id = 1; /* Client IDs, start from 1 .*/
    // server.page_size = sysconf(_SC_PAGESIZE);
    // server.pause_cron = 0;

    // server.latency_tracking_info_percentiles_len = 3;
    // server.latency_tracking_info_percentiles = zmalloc(sizeof(double)*(server.latency_tracking_info_percentiles_len));
    // server.latency_tracking_info_percentiles[0] = 50.0;  /* p50 */
    // server.latency_tracking_info_percentiles[1] = 99.0;  /* p99 */
    // server.latency_tracking_info_percentiles[2] = 99.9;  /* p999 */

    // unsigned int lruclock = getLRUClock();
    // atomicSet(server.lruclock,lruclock);
    // resetServerSaveParams();

    // appendServerSaveParams(60*60,1);  /* save after 1 hour and 1 change */
    // appendServerSaveParams(300,100);  /* save after 5 minutes and 100 changes */
    // appendServerSaveParams(60,10000); /* save after 1 minute and 10000 changes */

    // /* Replication related */
    // server.masterhost = NULL;
    // server.masterport = 6379;
    // server.master = NULL;
    // server.cached_master = NULL;
    // server.master_initial_offset = -1;
    // server.repl_state = REPL_STATE_NONE;
    // server.repl_transfer_tmpfile = NULL;
    // server.repl_transfer_fd = -1;
    // server.repl_transfer_s = NULL;
    // server.repl_syncio_timeout = CONFIG_REPL_SYNCIO_TIMEOUT;
    // server.repl_down_since = 0; /* Never connected, repl is down since EVER. */
    // server.master_repl_offset = 0;

    // /* Replication partial resync backlog */
    // server.repl_backlog = NULL;
    // server.repl_no_slaves_since = time(NULL);

    // /* Failover related */
    // server.failover_end_time = 0;
    // server.force_failover = 0;
    // server.target_replica_host = NULL;
    // server.target_replica_port = 0;
    // server.failover_state = NO_FAILOVER;

    // /* Client output buffer limits */
    // for (j = 0; j < CLIENT_TYPE_OBUF_COUNT; j++)
    //     server.client_obuf_limits[j] = clientBufferLimitsDefaults[j];

    // /* Linux OOM Score config */
    // for (j = 0; j < CONFIG_OOM_COUNT; j++)
    //     server.oom_score_adj_values[j] = configOOMScoreAdjValuesDefaults[j];

    // /* Double constants initialization */
    // R_Zero = 0.0;
    // R_PosInf = 1.0/R_Zero;
    // R_NegInf = -1.0/R_Zero;
    // R_Nan = R_Zero/R_Zero;

    // /* Command table -- we initialize it here as it is part of the
    //  * initial configuration, since command names may be changed via
    //  * redis.conf using the rename-command directive. */
    // server.commands = dictCreate(&commandTableDictType);
    // server.orig_commands = dictCreate(&commandTableDictType);
    // populateCommandTable();

    // /* Debugging */
    // server.watchdog_period = 0;
}

/* Our shared "common" objects */

// struct sharedObjectsStruct shared;

// uint64_t dictSdsHash(const void *key) {
//     return dictGenHashFunction((unsigned char*)key, sdslen((char*)key));
// }

uint64_t dictSdsCaseHash(const void *key) {
    return dictGenCaseHashFunction((unsigned char*)key, sdslen((char*)key));
}
dictType sdsHashDictType = {
    dictSdsCaseHash,            /* hash function */
    NULL,                       /* key dup */
    NULL,                       /* val dup */
    NULL,      /* key compare */
    NULL,          /* key destructor */
    NULL,            /* val destructor */
    NULL                        /* allow to expand */
};

// /* Dict hash function for null terminated string */
// uint64_t dictCStrHash(const void *key) {
//     return dictGenHashFunction((unsigned char*)key, strlen((char*)key));
// }

// /* Dict hash function for null terminated string */
// uint64_t dictCStrCaseHash(const void *key) {
//     return dictGenCaseHashFunction((unsigned char*)key, strlen((char*)key));
// }

/* Dict compare function for null terminated string */
// int dictCStrKeyCompare(dict *d, const void *key1, const void *key2) {
//     int l1,l2;
//     UNUSED(d);

//     l1 = strlen((char*)key1);
//     l2 = strlen((char*)key2);
//     if (l1 != l2) return 0;
//     return memcmp(key1, key2, l1) == 0;
// }

/* Dict case insensitive compare function for null terminated string */
// int dictCStrKeyCaseCompare(dict *d, const void *key1, const void *key2) {
//     UNUSED(d);
//     return strcasecmp(key1, key2) == 0;
// }

// int dictEncObjKeyCompare(dict *d, const void *key1, const void *key2)
// {
//     robj *o1 = (robj*) key1, *o2 = (robj*) key2;
//     int cmp;

//     if (o1->encoding == OBJ_ENCODING_INT &&
//         o2->encoding == OBJ_ENCODING_INT)
//             return o1->ptr == o2->ptr;

//     /* Due to OBJ_STATIC_REFCOUNT, we avoid calling getDecodedObject() without
//      * good reasons, because it would incrRefCount() the object, which
//      * is invalid. So we check to make sure dictFind() works with static
//      * objects as well. */
//     if (o1->refcount != OBJ_STATIC_REFCOUNT) o1 = getDecodedObject(o1);
//     if (o2->refcount != OBJ_STATIC_REFCOUNT) o2 = getDecodedObject(o2);
//     cmp = dictSdsKeyCompare(d,o1->ptr,o2->ptr);
//     if (o1->refcount != OBJ_STATIC_REFCOUNT) decrRefCount(o1);
//     if (o2->refcount != OBJ_STATIC_REFCOUNT) decrRefCount(o2);
//     return cmp;
// }

// uint64_t dictEncObjHash(const void *key) {
//     robj *o = (robj*) key;

//     if (sdsEncodedObject(o)) {
//         return dictGenHashFunction(o->ptr, sdslen((sds)o->ptr));
//     } else if (o->encoding == OBJ_ENCODING_INT) {
//         char buf[32];
//         int len;

//         len = ll2string(buf,32,(long)o->ptr);
//         return dictGenHashFunction((unsigned char*)buf, len);
//     } else {
//         serverPanic("Unknown string encoding");
//     }
// }

/*============================ Utility functions ============================ */

/* We use a private localtime implementation which is fork-safe. The logging
 * function of Redis may be called from other threads. */
void nolocks_localtime(struct tm *tmp, time_t t, time_t tz, int dst);

/* Low level logging. To use only for very big messages, otherwise
 * serverLog() is to prefer. */
void serverLogRaw(int level, const char *msg)
{
    const int syslogLevelMap[] = {LOG_DEBUG, LOG_INFO, LOG_NOTICE, LOG_WARNING};
    const char *c = ".-*#";
    FILE *fp;
    char buf[64];
    int rawmode = (level & LL_RAW);
    int log_to_stdout = server.logfile[0] == '\0';

    level &= 0xff; /* clear flags */
    if (level < server.verbosity)
        return;

    fp = log_to_stdout ? stdout : fopen(server.logfile, "a");
    if (!fp)
        return;

    if (rawmode)
    {
        fprintf(fp, "%s", msg);
    }
    else
    {
        int off;
        struct timeval tv;
        int role_char;
        pid_t pid = getpid();

        gettimeofday(&tv, NULL);
        struct tm tm;
        nolocks_localtime(&tm, tv.tv_sec, server.timezone, server.daylight_active);
        off = strftime(buf, sizeof(buf), "%d %b %Y %H:%M:%S.", &tm);
        snprintf(buf + off, sizeof(buf) - off, "%03d", (int)tv.tv_usec / 1000);
        if (server.sentinel_mode)
        {
            role_char = 'X'; /* Sentinel. */
        }
        else if (pid != server.pid)
        {
            role_char = 'C'; /* RDB / AOF writing child. */
        }
        else
        {
            role_char = (server.masterhost ? 'S' : 'M'); /* Slave or Master. */
        }
        fprintf(fp, "%d:%c %s %c %s\n",
                (int)getpid(), role_char, buf, c[level], msg);
    }
    fflush(fp);

    if (!log_to_stdout)
        fclose(fp);
    if (server.syslog_enabled)
        syslog(syslogLevelMap[level], "%s", msg);
}
/* Like serverLogRaw() but with printf-alike support. This is the function that
 * is used across the code. The raw version is only used in order to dump
 * the INFO output on crash. */
void _serverLog(int level, const char *fmt, ...)
{
    // va_list ap;
    // char msg[LOG_MAX_LEN];

    // va_start(ap, fmt);
    // vsnprintf(msg, sizeof(msg), fmt, ap);
    // va_end(ap);
    // serverLogRaw(level,msg);

    int level_t = (int)(1 << 10);
    serverLogRaw(level_t, "test");
}
void redisOutOfMemoryHandler(size_t allocation_size)
{
    serverLog(LL_WARNING, "Out Of Memory allocating %zu bytes!",
              allocation_size);
    // serverPanic("Redis aborting for OUT OF MEMORY. Allocating %zu bytes!",
    //     allocation_size);
}
int main(int argc, char **argv)
{
    struct timeval tv;
    int j;
    char config_from_stdin = 0;
    printf("mainserver %d", argc);
    /* We need to initialize our libraries, and the server configuration. */

#ifdef INIT_SETPROCTITLE_REPLACEMENT
    spt_init(argc, argv);
#endif
    tzset(); /* Populates 'timezone' global. */
    zmalloc_set_oom_handler(redisOutOfMemoryHandler);

    /* To achieve entropy, in case of containers, their time() and getpid() can
     * be the same. But value of tv_usec is fast enough to make the difference */
    gettimeofday(&tv, NULL);
    srand(time(NULL) ^ getpid() ^ tv.tv_usec);
    srandom(time(NULL) ^ getpid() ^ tv.tv_usec);
    init_genrand64(((long long)tv.tv_sec * 1000000 + tv.tv_usec) ^ getpid());
    crc64_init();
    // serverLog(LL_WARNING,"serverLog!",0x11);

    /* Store umask value. Because umask(2) only offers a set-and-get API we have
     * to reset it and restore it back. We do this early to avoid a potential
     * race condition with threads that could be creating files or directories.
     */
    umask(server.umask = umask(0777));
    uint8_t hashseed[16];
    getRandomBytes(hashseed, sizeof(hashseed));
    dictSetHashFunctionSeed(hashseed);
    char *exec_name = strrchr(argv[0], '/');
    if (exec_name == NULL) exec_name = argv[0];
    server.sentinel_mode = checkForSentinelMode(argc,argv, exec_name);
    initServerConfig();

}