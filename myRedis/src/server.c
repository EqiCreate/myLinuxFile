#include "server.h"
#include "config.h"
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "Main/mt19937-64.h"
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/uio.h>
#include <sys/un.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include <sys/utsname.h>
#include <locale.h>
#include <sys/socket.h>

#ifdef __linux__
#include <sys/mman.h>
#endif

#if defined(HAVE_SYSCTL_KIPC_SOMAXCONN) || defined(HAVE_SYSCTL_KERN_SOMAXCONN)
#include <sys/sysctl.h>
#endif

// #include "functions.h"

struct sharedObjectsStruct shared;
struct redisCommand *lookupSubcommand(struct redisCommand *container, sds sub_name) {
    return dictFetchValue(container->subcommands_dict, sub_name);
}

/* Look up a command by argv and argc
 *
 * If `strict` is not 0 we expect argc to be exact (i.e. argc==2
 * for a subcommand and argc==1 for a top-level command)
 * `strict` should be used every time we want to look up a command
 * name (e.g. in COMMAND INFO) rather than to find the command
 * a user requested to execute (in processCommand).
 */
struct redisCommand *lookupCommandLogic(dict *commands, robj **argv, int argc, int strict) {
    struct redisCommand *base_cmd = dictFetchValue(commands, argv[0]->ptr);
    int has_subcommands = base_cmd && base_cmd->subcommands_dict;
    if (argc == 1 || !has_subcommands) {
        if (strict && argc != 1)
            return NULL;
        /* Note: It is possible that base_cmd->proc==NULL (e.g. CONFIG) */
        return base_cmd;
    } else { /* argc > 1 && has_subcommands */
        if (strict && argc != 2)
            return NULL;
        /* Note: Currently we support just one level of subcommands */
        return lookupSubcommand(base_cmd, argv[1]->ptr);
    }
}
struct redisCommand *lookupCommandBySdsLogic(dict *commands, sds s) {
    int argc, j;
    sds *strings = sdssplitlen(s,sdslen(s),"|",1,&argc);
    if (strings == NULL)
        return NULL;
    if (argc > 2) {
        /* Currently we support just one level of subcommands */
        sdsfreesplitres(strings,argc);
        return NULL;
    }

    robj objects[argc];
    robj *argv[argc];
    for (j = 0; j < argc; j++) {
        initStaticStringObject(objects[j],strings[j]);
        argv[j] = &objects[j];
    }

    struct redisCommand *cmd = lookupCommandLogic(commands,argv,argc,1);
    sdsfreesplitres(strings,argc);
    return cmd;
}
struct redisCommand *lookupCommandBySds(sds s) {
    return lookupCommandBySdsLogic(server.commands,s);
}

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

void version(void) {
    printf("Redis server v=%s sha=%s:%d malloc=%s bits=%d build=%llx\n",
        REDIS_VERSION,
        1111,
        // redisGitSHA1(),a
        // atoi(redisGitDirty()) > 0,
        0,
        ZMALLOC_LIB,
        sizeof(long) == 4 ? 32 : 64,
        REDIS_VERSION_NUM);
        // (unsigned long long) redisBuildId());
    exit(0);
}

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

#pragma region 

/* Make the thread killable at any time, so that kill threads functions
 * can work reliably (default cancelability type is PTHREAD_CANCEL_DEFERRED).
 * Needed for pthread_cancel used by the fast memory test used by the crash report. */
void makeThreadKillable(void) {
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
}


/* Log a fixed message without printf-alike capabilities, in a way that is
 * safe to call from a signal handler.
 *
 * We actually use this only for signals that are not fatal from the point
 * of view of Redis. Signals that are going to kill the server anyway and
 * where we need printf-alike features are served by serverLog(). */
void serverLogFromHandler(int level, const char *msg) {
    int fd;
    int log_to_stdout = server.logfile[0] == '\0';
    char buf[64];

    if ((level&0xff) < server.verbosity || (log_to_stdout && server.daemonize))
        return;
    fd = log_to_stdout ? STDOUT_FILENO :
                         open(server.logfile, O_APPEND|O_CREAT|O_WRONLY, 0644);
    if (fd == -1) return;
    ll2string(buf,sizeof(buf),getpid());
    if (write(fd,buf,strlen(buf)) == -1) goto err;
    if (write(fd,":signal-handler (",17) == -1) goto err;
    ll2string(buf,sizeof(buf),time(NULL));
    if (write(fd,buf,strlen(buf)) == -1) goto err;
    if (write(fd,") ",2) == -1) goto err;
    if (write(fd,msg,strlen(msg)) == -1) goto err;
    if (write(fd,"\n",1) == -1) goto err;
err:
    if (!log_to_stdout) close(fd);
}
static void sigShutdownHandler(int sig) {
    char *msg;

    switch (sig) {
    case SIGINT:
        msg = "Received SIGINT scheduling shutdown...";
        break;
    case SIGTERM:
        msg = "Received SIGTERM scheduling shutdown...";
        break;
    default:
        msg = "Received shutdown signal, scheduling shutdown...";
    };

    /* SIGINT is often delivered via Ctrl+C in an interactive session.
     * If we receive the signal the second time, we interpret this as
     * the user really wanting to quit ASAP without waiting to persist
     * on disk and without waiting for lagging replicas. */
    if (server.shutdown_asap && sig == SIGINT) {
        serverLogFromHandler(LL_WARNING, "You insist... exiting now.");
        // rdbRemoveTempFile(getpid(), 1);
        exit(1); /* Exit with an error since this was not a clean shutdown. */
    } else if (server.loading) {
        msg = "Received shutdown signal during loading, scheduling shutdown.";
    }

    serverLogFromHandler(LL_WARNING, msg);
    server.shutdown_asap = 1;
    server.last_sig_received = sig;
}

void setupSignalHandlers(void) {
    struct sigaction act;

    /* When the SA_SIGINFO flag is set in sa_flags then sa_sigaction is used.
     * Otherwise, sa_handler is used. */
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_handler = sigShutdownHandler;
    sigaction(SIGTERM, &act, NULL);
    sigaction(SIGINT, &act, NULL);

    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_NODEFER | SA_RESETHAND | SA_SIGINFO;
    act.sa_sigaction = sigsegvHandler;
    if(server.crashlog_enabled) {
        sigaction(SIGSEGV, &act, NULL);
        sigaction(SIGBUS, &act, NULL);
        sigaction(SIGFPE, &act, NULL);
        sigaction(SIGILL, &act, NULL);
        sigaction(SIGABRT, &act, NULL);
    }
    return;
}

void createSharedObjects(void) {
    int j;

    /* Shared command responses */
    shared.ok = createObject(OBJ_STRING,sdsnew("+OK\r\n"));
    // shared.emptybulk = createObject(OBJ_STRING,sdsnew("$0\r\n\r\n"));
    // shared.czero = createObject(OBJ_STRING,sdsnew(":0\r\n"));
    // shared.cone = createObject(OBJ_STRING,sdsnew(":1\r\n"));
    // shared.emptyarray = createObject(OBJ_STRING,sdsnew("*0\r\n"));
    // shared.pong = createObject(OBJ_STRING,sdsnew("+PONG\r\n"));
    // shared.queued = createObject(OBJ_STRING,sdsnew("+QUEUED\r\n"));
    // shared.emptyscan = createObject(OBJ_STRING,sdsnew("*2\r\n$1\r\n0\r\n*0\r\n"));
    // shared.space = createObject(OBJ_STRING,sdsnew(" "));
    // shared.plus = createObject(OBJ_STRING,sdsnew("+"));

    // /* Shared command error responses */
    // shared.wrongtypeerr = createObject(OBJ_STRING,sdsnew(
    //     "-WRONGTYPE Operation against a key holding the wrong kind of value\r\n"));
    // shared.err = createObject(OBJ_STRING,sdsnew("-ERR\r\n"));
    // shared.nokeyerr = createObject(OBJ_STRING,sdsnew(
    //     "-ERR no such key\r\n"));
    // shared.syntaxerr = createObject(OBJ_STRING,sdsnew(
    //     "-ERR syntax error\r\n"));
    // shared.sameobjecterr = createObject(OBJ_STRING,sdsnew(
    //     "-ERR source and destination objects are the same\r\n"));
    // shared.outofrangeerr = createObject(OBJ_STRING,sdsnew(
    //     "-ERR index out of range\r\n"));
    // shared.noscripterr = createObject(OBJ_STRING,sdsnew(
    //     "-NOSCRIPT No matching script. Please use EVAL.\r\n"));
    // shared.loadingerr = createObject(OBJ_STRING,sdsnew(
    //     "-LOADING Redis is loading the dataset in memory\r\n"));
    // shared.slowevalerr = createObject(OBJ_STRING,sdsnew(
    //     "-BUSY Redis is busy running a script. You can only call SCRIPT KILL or SHUTDOWN NOSAVE.\r\n"));
    // shared.slowscripterr = createObject(OBJ_STRING,sdsnew(
    //     "-BUSY Redis is busy running a script. You can only call FUNCTION KILL or SHUTDOWN NOSAVE.\r\n"));
    // shared.slowmoduleerr = createObject(OBJ_STRING,sdsnew(
    //     "-BUSY Redis is busy running a module command.\r\n"));
    // shared.masterdownerr = createObject(OBJ_STRING,sdsnew(
    //     "-MASTERDOWN Link with MASTER is down and replica-serve-stale-data is set to 'no'.\r\n"));
    // shared.bgsaveerr = createObject(OBJ_STRING,sdsnew(
    //     "-MISCONF Redis is configured to save RDB snapshots, but it's currently unable to persist to disk. Commands that may modify the data set are disabled, because this instance is configured to report errors during writes if RDB snapshotting fails (stop-writes-on-bgsave-error option). Please check the Redis logs for details about the RDB error.\r\n"));
    // shared.roslaveerr = createObject(OBJ_STRING,sdsnew(
    //     "-READONLY You can't write against a read only replica.\r\n"));
    // shared.noautherr = createObject(OBJ_STRING,sdsnew(
    //     "-NOAUTH Authentication required.\r\n"));
    // shared.oomerr = createObject(OBJ_STRING,sdsnew(
    //     "-OOM command not allowed when used memory > 'maxmemory'.\r\n"));
    // shared.execaborterr = createObject(OBJ_STRING,sdsnew(
    //     "-EXECABORT Transaction discarded because of previous errors.\r\n"));
    // shared.noreplicaserr = createObject(OBJ_STRING,sdsnew(
    //     "-NOREPLICAS Not enough good replicas to write.\r\n"));
    // shared.busykeyerr = createObject(OBJ_STRING,sdsnew(
    //     "-BUSYKEY Target key name already exists.\r\n"));

    // /* The shared NULL depends on the protocol version. */
    // shared.null[0] = NULL;
    // shared.null[1] = NULL;
    // shared.null[2] = createObject(OBJ_STRING,sdsnew("$-1\r\n"));
    // shared.null[3] = createObject(OBJ_STRING,sdsnew("_\r\n"));

    // shared.nullarray[0] = NULL;
    // shared.nullarray[1] = NULL;
    // shared.nullarray[2] = createObject(OBJ_STRING,sdsnew("*-1\r\n"));
    // shared.nullarray[3] = createObject(OBJ_STRING,sdsnew("_\r\n"));

    // shared.emptymap[0] = NULL;
    // shared.emptymap[1] = NULL;
    // shared.emptymap[2] = createObject(OBJ_STRING,sdsnew("*0\r\n"));
    // shared.emptymap[3] = createObject(OBJ_STRING,sdsnew("%0\r\n"));

    // shared.emptyset[0] = NULL;
    // shared.emptyset[1] = NULL;
    // shared.emptyset[2] = createObject(OBJ_STRING,sdsnew("*0\r\n"));
    // shared.emptyset[3] = createObject(OBJ_STRING,sdsnew("~0\r\n"));

    // for (j = 0; j < PROTO_SHARED_SELECT_CMDS; j++) {
    //     char dictid_str[64];
    //     int dictid_len;

    //     dictid_len = ll2string(dictid_str,sizeof(dictid_str),j);
    //     shared.select[j] = createObject(OBJ_STRING,
    //         sdscatprintf(sdsempty(),
    //             "*2\r\n$6\r\nSELECT\r\n$%d\r\n%s\r\n",
    //             dictid_len, dictid_str));
    // }
    // shared.messagebulk = createStringObject("$7\r\nmessage\r\n",13);
    // shared.pmessagebulk = createStringObject("$8\r\npmessage\r\n",14);
    // shared.subscribebulk = createStringObject("$9\r\nsubscribe\r\n",15);
    // shared.unsubscribebulk = createStringObject("$11\r\nunsubscribe\r\n",18);
    // shared.ssubscribebulk = createStringObject("$10\r\nssubscribe\r\n", 17);
    // shared.sunsubscribebulk = createStringObject("$12\r\nsunsubscribe\r\n", 19);
    // shared.smessagebulk = createStringObject("$8\r\nsmessage\r\n", 14);
    // shared.psubscribebulk = createStringObject("$10\r\npsubscribe\r\n",17);
    // shared.punsubscribebulk = createStringObject("$12\r\npunsubscribe\r\n",19);

    // /* Shared command names */
    // shared.del = createStringObject("DEL",3);
    // shared.unlink = createStringObject("UNLINK",6);
    // shared.rpop = createStringObject("RPOP",4);
    // shared.lpop = createStringObject("LPOP",4);
    // shared.lpush = createStringObject("LPUSH",5);
    // shared.rpoplpush = createStringObject("RPOPLPUSH",9);
    // shared.lmove = createStringObject("LMOVE",5);
    // shared.blmove = createStringObject("BLMOVE",6);
    // shared.zpopmin = createStringObject("ZPOPMIN",7);
    // shared.zpopmax = createStringObject("ZPOPMAX",7);
    // shared.multi = createStringObject("MULTI",5);
    // shared.exec = createStringObject("EXEC",4);
    // shared.hset = createStringObject("HSET",4);
    // shared.srem = createStringObject("SREM",4);
    // shared.xgroup = createStringObject("XGROUP",6);
    // shared.xclaim = createStringObject("XCLAIM",6);
    // shared.script = createStringObject("SCRIPT",6);
    // shared.replconf = createStringObject("REPLCONF",8);
    // shared.pexpireat = createStringObject("PEXPIREAT",9);
    // shared.pexpire = createStringObject("PEXPIRE",7);
    // shared.persist = createStringObject("PERSIST",7);
    // shared.set = createStringObject("SET",3);
    // shared.eval = createStringObject("EVAL",4);

    // /* Shared command argument */
    // shared.left = createStringObject("left",4);
    // shared.right = createStringObject("right",5);
    // shared.pxat = createStringObject("PXAT", 4);
    // shared.time = createStringObject("TIME",4);
    // shared.retrycount = createStringObject("RETRYCOUNT",10);
    // shared.force = createStringObject("FORCE",5);
    // shared.justid = createStringObject("JUSTID",6);
    // shared.entriesread = createStringObject("ENTRIESREAD",11);
    // shared.lastid = createStringObject("LASTID",6);
    // shared.default_username = createStringObject("default",7);
    // shared.ping = createStringObject("ping",4);
    // shared.setid = createStringObject("SETID",5);
    // shared.keepttl = createStringObject("KEEPTTL",7);
    // shared.absttl = createStringObject("ABSTTL",6);
    // shared.load = createStringObject("LOAD",4);
    // shared.createconsumer = createStringObject("CREATECONSUMER",14);
    // shared.getack = createStringObject("GETACK",6);
    // shared.special_asterick = createStringObject("*",1);
    // shared.special_equals = createStringObject("=",1);
    // shared.redacted = makeObjectShared(createStringObject("(redacted)",10));

    // for (j = 0; j < OBJ_SHARED_INTEGERS; j++) {
    //     shared.integers[j] =
    //         makeObjectShared(createObject(OBJ_STRING,(void*)(long)j));
    //     shared.integers[j]->encoding = OBJ_ENCODING_INT;
    // }
    // for (j = 0; j < OBJ_SHARED_BULKHDR_LEN; j++) {
    //     shared.mbulkhdr[j] = createObject(OBJ_STRING,
    //         sdscatprintf(sdsempty(),"*%d\r\n",j));
    //     shared.bulkhdr[j] = createObject(OBJ_STRING,
    //         sdscatprintf(sdsempty(),"$%d\r\n",j));
    //     shared.maphdr[j] = createObject(OBJ_STRING,
    //         sdscatprintf(sdsempty(),"%%%d\r\n",j));
    //     shared.sethdr[j] = createObject(OBJ_STRING,
    //         sdscatprintf(sdsempty(),"~%d\r\n",j));
    // }
    // /* The following two shared objects, minstring and maxstring, are not
    //  * actually used for their value but as a special object meaning
    //  * respectively the minimum possible string and the maximum possible
    //  * string in string comparisons for the ZRANGEBYLEX command. */
    // shared.minstring = sdsnew("minstring");
    // shared.maxstring = sdsnew("maxstring");
}

/* This function will try to raise the max number of open files accordingly to
 * the configured max number of clients. It also reserves a number of file
 * descriptors (CONFIG_MIN_RESERVED_FDS) for extra operations of
 * persistence, listening sockets, log files and so forth.
 *
 * If it will not be possible to set the limit accordingly to the configured
 * max number of clients, the function will do the reverse setting
 * server.maxclients to the value that we can actually handle. */
void adjustOpenFilesLimit(void) {
}

/* This is our timer interrupt, called server.hz times per second.
 * Here is where we do a number of things that need to be done asynchronously.
 * For instance:
 *
 * - Active expired keys collection (it is also performed in a lazy way on
 *   lookup).
 * - Software watchdog.
 * - Update some statistic.
 * - Incremental rehashing of the DBs hash tables.
 * - Triggering BGSAVE / AOF rewrite, and handling of terminated children.
 * - Clients timeout of different kinds.
 * - Replication reconnection.
 * - Many more...
 *
 * Everything directly called here will be called server.hz times per second,
 * so in order to throttle execution of things we want to do less frequently
 * a macro is used: run_with_period(milliseconds) { .... }
 */

int serverCron(struct aeEventLoop *eventLoop, long long id, void *clientData) {
    int j;
    UNUSED(eventLoop);
    UNUSED(id);
    UNUSED(clientData);

    /* Software watchdog: deliver the SIGALRM that will reach the signal
     * handler if we don't return here fast enough. */
    if (server.watchdog_period) watchdogScheduleSignal(server.watchdog_period);

    // server.hz = server.config_hz;

    server.hz = server.config_hz+1;
    /* Adapt the server.hz value to the number of configured clients. If we have
     * many clients, we want to call serverCron() with an higher frequency. */
    // if (server.dynamic_hz) {
    //     while (listLength(server.clients) / server.hz >
    //            MAX_CLIENTS_PER_CLOCK_TICK)
    //     {
    //         server.hz *= 2;
    //         if (server.hz > CONFIG_MAX_HZ) {
    //             server.hz = CONFIG_MAX_HZ;
    //             break;
    //         }
    //     }
    // }

    /* for debug purposes: skip actual cron work if pause_cron is on */
    if (server.pause_cron) return 1000/server.hz;

    // run_with_period(100) {
    //     long long stat_net_input_bytes, stat_net_output_bytes;
    //     long long stat_net_repl_input_bytes, stat_net_repl_output_bytes;
    //     atomicGet(server.stat_net_input_bytes, stat_net_input_bytes);
    //     atomicGet(server.stat_net_output_bytes, stat_net_output_bytes);
    //     atomicGet(server.stat_net_repl_input_bytes, stat_net_repl_input_bytes);
    //     atomicGet(server.stat_net_repl_output_bytes, stat_net_repl_output_bytes);

    //     trackInstantaneousMetric(STATS_METRIC_COMMAND,server.stat_numcommands);
    //     trackInstantaneousMetric(STATS_METRIC_NET_INPUT,
    //             stat_net_input_bytes + stat_net_repl_input_bytes);
    //     trackInstantaneousMetric(STATS_METRIC_NET_OUTPUT,
    //             stat_net_output_bytes + stat_net_repl_output_bytes);
    //     trackInstantaneousMetric(STATS_METRIC_NET_INPUT_REPLICATION,
    //                              stat_net_repl_input_bytes);
    //     trackInstantaneousMetric(STATS_METRIC_NET_OUTPUT_REPLICATION,
    //                              stat_net_repl_output_bytes);
    // }

    /* We have just LRU_BITS bits per object for LRU information.
     * So we use an (eventually wrapping) LRU clock.
     *
     * Note that even if the counter wraps it's not a big problem,
     * everything will still work but some object will appear younger
     * to Redis. However for this to happen a given object should never be
     * touched for all the time needed to the counter to wrap, which is
     * not likely.
     *
     * Note that you can change the resolution altering the
     * LRU_CLOCK_RESOLUTION define. */
    // unsigned int lruclock = getLRUClock();
    // atomicSet(server.lruclock,lruclock);

    // cronUpdateMemoryStats();

    /* We received a SIGTERM or SIGINT, shutting down here in a safe way, as it is
     * not ok doing so inside the signal handler. */
    // if (server.shutdown_asap && !isShutdownInitiated()) {
    //     int shutdownFlags = SHUTDOWN_NOFLAGS;
    //     if (server.last_sig_received == SIGINT && server.shutdown_on_sigint)
    //         shutdownFlags = server.shutdown_on_sigint;
    //     else if (server.last_sig_received == SIGTERM && server.shutdown_on_sigterm)
    //         shutdownFlags = server.shutdown_on_sigterm;

    //     if (prepareForShutdown(shutdownFlags) == C_OK) exit(0);
    // } else if (isShutdownInitiated()) {
    //     if (server.mstime >= server.shutdown_mstime || isReadyToShutdown()) {
    //         if (finishShutdown() == C_OK) exit(0);
    //         /* Shutdown failed. Continue running. An error has been logged. */
    //     }
    // }

    /* Show some info about non-empty databases */
    if (server.verbosity <= LL_VERBOSE) {
        run_with_period(5000) {
            // for (j = 0; j < server.dbnum; j++) {
            //     long long size, used, vkeys;

            //     size = dictSlots(server.db[j].dict);
            //     used = dictSize(server.db[j].dict);
            //     vkeys = dictSize(server.db[j].expires);
            //     if (used || vkeys) {
            //         serverLog(LL_VERBOSE,"DB %d: %lld keys (%lld volatile) in %lld slots HT.",j,used,vkeys,size);
            //     }
            // }
        }
    }

    /* Show information about connected clients */
    if (!server.sentinel_mode) {
        run_with_period(5000) {
            // serverLog(LL_DEBUG,
            //     "%lu clients connected (%lu replicas), %zu bytes in use",
            //     listLength(server.clients)-listLength(server.slaves),
            //     listLength(server.slaves),
            //     zmalloc_used_memory());
        }
    }

    /* We need to do a few operations on clients asynchronously. */
    // clientsCron();

    /* Handle background operations on Redis databases. */
    // databasesCron();

    /* Start a scheduled AOF rewrite if this was requested by the user while
     * a BGSAVE was in progress. */
    // if (!hasActiveChildProcess() &&
    //     server.aof_rewrite_scheduled &&
    //     !aofRewriteLimited())
    // {
    //     rewriteAppendOnlyFileBackground();
    // }

    /* Check if a background saving or AOF rewrite in progress terminated. */
    // if (hasActiveChildProcess() || ldbPendingChildren())
    // {
    //     run_with_period(1000) receiveChildInfo();
    //     checkChildrenDone();
    // } else {
    //     /* If there is not a background saving/rewrite in progress check if
    //      * we have to save/rewrite now. */
    //     for (j = 0; j < server.saveparamslen; j++) {
    //         struct saveparam *sp = server.saveparams+j;

    //         /* Save if we reached the given amount of changes,
    //          * the given amount of seconds, and if the latest bgsave was
    //          * successful or if, in case of an error, at least
    //          * CONFIG_BGSAVE_RETRY_DELAY seconds already elapsed. */
    //         if (server.dirty >= sp->changes &&
    //             server.unixtime-server.lastsave > sp->seconds &&
    //             (server.unixtime-server.lastbgsave_try >
    //              CONFIG_BGSAVE_RETRY_DELAY ||
    //              server.lastbgsave_status == C_OK))
    //         {
    //             serverLog(LL_NOTICE,"%d changes in %d seconds. Saving...",
    //                 sp->changes, (int)sp->seconds);
    //             rdbSaveInfo rsi, *rsiptr;
    //             rsiptr = rdbPopulateSaveInfo(&rsi);
    //             rdbSaveBackground(SLAVE_REQ_NONE,server.rdb_filename,rsiptr);
    //             break;
    //         }
    //     }

    //     /* Trigger an AOF rewrite if needed. */
    //     if (server.aof_state == AOF_ON &&
    //         !hasActiveChildProcess() &&
    //         server.aof_rewrite_perc &&
    //         server.aof_current_size > server.aof_rewrite_min_size)
    //     {
    //         long long base = server.aof_rewrite_base_size ?
    //             server.aof_rewrite_base_size : 1;
    //         long long growth = (server.aof_current_size*100/base) - 100;
    //         if (growth >= server.aof_rewrite_perc && !aofRewriteLimited()) {
    //             serverLog(LL_NOTICE,"Starting automatic rewriting of AOF on %lld%% growth",growth);
    //             rewriteAppendOnlyFileBackground();
    //         }
    //     }
    // }
    /* Just for the sake of defensive programming, to avoid forgetting to
     * call this function when needed. */
    // updateDictResizePolicy();


    /* AOF postponed flush: Try at every cron cycle if the slow fsync
     * completed. */
    // if ((server.aof_state == AOF_ON || server.aof_state == AOF_WAIT_REWRITE) &&
    //     server.aof_flush_postponed_start)
    // {
    //     flushAppendOnlyFile(0);
    // }

    /* AOF write errors: in this case we have a buffer to flush as well and
     * clear the AOF error in case of success to make the DB writable again,
     * however to try every second is enough in case of 'hz' is set to
     * a higher frequency. */
    // run_with_period(1000) {
    //     if ((server.aof_state == AOF_ON || server.aof_state == AOF_WAIT_REWRITE) &&
    //         server.aof_last_write_status == C_ERR) 
    //         {
    //             flushAppendOnlyFile(0);
    //         }
    // }

    /* Clear the paused actions state if needed. */
    // updatePausedActions();

    /* Replication cron function -- used to reconnect to master,
     * detect transfer failures, start background RDB transfers and so forth. 
     * 
     * If Redis is trying to failover then run the replication cron faster so
     * progress on the handshake happens more quickly. */
    // if (server.failover_state != NO_FAILOVER) {
    //     run_with_period(100) replicationCron();
    // } else {
    //     run_with_period(1000) replicationCron();
    // }

    /* Run the Redis Cluster cron. */
    // run_with_period(100) {
    //     if (server.cluster_enabled) clusterCron();
    // }

    /* Run the Sentinel timer if we are in sentinel mode. */
    // if (server.sentinel_mode) sentinelTimer();

    /* Cleanup expired MIGRATE cached sockets. */
    // run_with_period(1000) {
    //     migrateCloseTimedoutSockets();
    // }

    /* Stop the I/O threads if we don't have enough pending work. */
    // stopThreadedIOIfNeeded();

    /* Resize tracking keys table if needed. This is also done at every
     * command execution, but we want to be sure that if the last command
     * executed changes the value via CONFIG SET, the server will perform
     * the operation even if completely idle. */
    // if (server.tracking_clients) trackingLimitUsedSlots();

    /* Start a scheduled BGSAVE if the corresponding flag is set. This is
     * useful when we are forced to postpone a BGSAVE because an AOF
     * rewrite is in progress.
     *
     * Note: this code must be after the replicationCron() call above so
     * make sure when refactoring this file to keep this order. This is useful
     * because we want to give priority to RDB savings for replication. */
    // if (!hasActiveChildProcess() &&
    //     server.rdb_bgsave_scheduled &&
    //     (server.unixtime-server.lastbgsave_try > CONFIG_BGSAVE_RETRY_DELAY ||
    //      server.lastbgsave_status == C_OK))
    // {
    //     rdbSaveInfo rsi, *rsiptr;
    //     rsiptr = rdbPopulateSaveInfo(&rsi);
    //     if (rdbSaveBackground(SLAVE_REQ_NONE,server.rdb_filename,rsiptr) == C_OK)
    //         server.rdb_bgsave_scheduled = 0;
    // }

    // run_with_period(100) {
    //     if (moduleCount()) modulesCron();
    // }

    /* Fire the cron loop modules event. */
    // RedisModuleCronLoopV1 ei = {REDISMODULE_CRON_LOOP_VERSION,server.hz};
    // moduleFireServerEvent(REDISMODULE_EVENT_CRON_LOOP,
    //                       0,
    //                       &ei);

    server.cronloops++;
    // return 1000/server.hz;
    return 1000/server.hz+1;

}

/* This function gets called every time Redis is entering the
 * main loop of the event driven library, that is, before to sleep
 * for ready file descriptors.
 *
 * Note: This function is (currently) called from two functions:
 * 1. aeMain - The main server loop
 * 2. processEventsWhileBlocked - Process clients during RDB/AOF load
 *
 * If it was called from processEventsWhileBlocked we don't want
 * to perform all actions (For example, we don't want to expire
 * keys), but we do need to perform some actions.
 *
 * The most important is freeClientsInAsyncFreeQueue but we also
 * call some other low-risk functions. */
void beforeSleep(struct aeEventLoop *eventLoop) {
    UNUSED(eventLoop);

    size_t zmalloc_used = zmalloc_used_memory();
    if (zmalloc_used > server.stat_peak_memory)
        server.stat_peak_memory = zmalloc_used;

    // /* Just call a subset of vital functions in case we are re-entering
    //  * the event loop from processEventsWhileBlocked(). Note that in this
    //  * case we keep track of the number of events we are processing, since
    //  * processEventsWhileBlocked() wants to stop ASAP if there are no longer
    //  * events to handle. */
    // if (ProcessingEventsWhileBlocked) {
    //     uint64_t processed = 0;
    //     processed += handleClientsWithPendingReadsUsingThreads();
    //     processed += connTypeProcessPendingData();
    //     if (server.aof_state == AOF_ON || server.aof_state == AOF_WAIT_REWRITE)
    //         flushAppendOnlyFile(0);
    //     processed += handleClientsWithPendingWrites();
    //     processed += freeClientsInAsyncFreeQueue();
    //     server.events_processed_while_blocked += processed;
    //     return;
    // }

    // /* Handle precise timeouts of blocked clients. */
    // handleBlockedClientsTimeout();

    // /* We should handle pending reads clients ASAP after event loop. */
    // handleClientsWithPendingReadsUsingThreads();

    // /* Handle pending data(typical TLS). (must be done before flushAppendOnlyFile) */
    // connTypeProcessPendingData();

    // /* If any connection type(typical TLS) still has pending unread data don't sleep at all. */
    // aeSetDontWait(server.el, connTypeHasPendingData());

    // /* Call the Redis Cluster before sleep function. Note that this function
    //  * may change the state of Redis Cluster (from ok to fail or vice versa),
    //  * so it's a good idea to call it before serving the unblocked clients
    //  * later in this function. */
    // if (server.cluster_enabled) clusterBeforeSleep();

    // /* Run a fast expire cycle (the called function will return
    //  * ASAP if a fast cycle is not needed). */
    // if (server.active_expire_enabled && server.masterhost == NULL)
    //     activeExpireCycle(ACTIVE_EXPIRE_CYCLE_FAST);

    // /* Unblock all the clients blocked for synchronous replication
    //  * in WAIT. */
    // if (listLength(server.clients_waiting_acks))
    //     processClientsWaitingReplicas();

    // /* Check if there are clients unblocked by modules that implement
    //  * blocking commands. */
    // if (moduleCount()) {
    //     moduleFireServerEvent(REDISMODULE_EVENT_EVENTLOOP,
    //                           REDISMODULE_SUBEVENT_EVENTLOOP_BEFORE_SLEEP,
    //                           NULL);
    //     moduleHandleBlockedClients();
    // }

    // /* Try to process pending commands for clients that were just unblocked. */
    // if (listLength(server.unblocked_clients))
    //     processUnblockedClients();

    // /* Send all the slaves an ACK request if at least one client blocked
    //  * during the previous event loop iteration. Note that we do this after
    //  * processUnblockedClients(), so if there are multiple pipelined WAITs
    //  * and the just unblocked WAIT gets blocked again, we don't have to wait
    //  * a server cron cycle in absence of other event loop events. See #6623.
    //  * 
    //  * We also don't send the ACKs while clients are paused, since it can
    //  * increment the replication backlog, they'll be sent after the pause
    //  * if we are still the master. */
    // if (server.get_ack_from_slaves && !isPausedActionsWithUpdate(PAUSE_ACTION_REPLICA)) {
    //     sendGetackToReplicas();
    //     server.get_ack_from_slaves = 0;
    // }

    // /* We may have received updates from clients about their current offset. NOTE:
    //  * this can't be done where the ACK is received since failover will disconnect 
    //  * our clients. */
    // updateFailoverStatus();

    // /* Since we rely on current_client to send scheduled invalidation messages
    //  * we have to flush them after each command, so when we get here, the list
    //  * must be empty. */
    // serverAssert(listLength(server.tracking_pending_keys) == 0);

    // /* Send the invalidation messages to clients participating to the
    //  * client side caching protocol in broadcasting (BCAST) mode. */
    // trackingBroadcastInvalidationMessages();

    // /* Try to process blocked clients every once in while.
    //  *
    //  * Example: A module calls RM_SignalKeyAsReady from within a timer callback
    //  * (So we don't visit processCommand() at all).
    //  *
    //  * must be done before flushAppendOnlyFile, in case of appendfsync=always,
    //  * since the unblocked clients may write data. */
    // handleClientsBlockedOnKeys();

    // /* Write the AOF buffer on disk,
    //  * must be done before handleClientsWithPendingWritesUsingThreads,
    //  * in case of appendfsync=always. */
    // if (server.aof_state == AOF_ON || server.aof_state == AOF_WAIT_REWRITE)
    //     flushAppendOnlyFile(0);

    // /* Handle writes with pending output buffers. */
    // handleClientsWithPendingWritesUsingThreads();

    // /* Close clients that need to be closed asynchronous */
    // freeClientsInAsyncFreeQueue();

    // /* Incrementally trim replication backlog, 10 times the normal speed is
    //  * to free replication backlog as much as possible. */
    // if (server.repl_backlog)
    //     incrementalTrimReplicationBacklog(10*REPL_BACKLOG_TRIM_BLOCKS_PER_CALL);

    // /* Disconnect some clients if they are consuming too much memory. */
    // evictClients();

    // /* Before we are going to sleep, let the threads access the dataset by
    //  * releasing the GIL. Redis main thread will not touch anything at this
    //  * time. */
    // if (moduleCount()) moduleReleaseGIL();

    /* Do NOT add anything below moduleReleaseGIL !!! */
}

/* This function is called immediately after the event loop multiplexing
 * API returned, and the control is going to soon return to Redis by invoking
 * the different events callbacks. */
void afterSleep(struct aeEventLoop *eventLoop) {
    UNUSED(eventLoop);

    /* Update the time cache. */
    // updateCachedTime(1);

    // /* Do NOT add anything above moduleAcquireGIL !!! */

    // /* Acquire the modules GIL so that their threads won't touch anything. */
    // if (!ProcessingEventsWhileBlocked) {
    //     if (moduleCount()) {
    //         mstime_t latency;
    //         latencyStartMonitor(latency);

    //         moduleAcquireGIL();
    //         moduleFireServerEvent(REDISMODULE_EVENT_EVENTLOOP,
    //                               REDISMODULE_SUBEVENT_EVENTLOOP_AFTER_SLEEP,
    //                               NULL);
    //         latencyEndMonitor(latency);
    //         latencyAddSampleIfNeeded("module-acquire-GIL",latency);
    //     }
    // }
}


void initServer(void) {
    int j;

    signal(SIGHUP, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    setupSignalHandlers();
    makeThreadKillable();

    if (server.syslog_enabled) {
        openlog(server.syslog_ident, LOG_PID | LOG_NDELAY | LOG_NOWAIT,
            server.syslog_facility);
    }

    /* Initialization after setting defaults from the config system. */
    // server.aof_state = server.aof_enabled ? AOF_ON : AOF_OFF;
    server.hz = server.config_hz;
    server.pid = getpid();
    server.in_fork_child = CHILD_TYPE_NONE;
    server.main_thread_id = pthread_self();
    // server.current_client = NULL;
    // server.errors = raxNew();
    server.in_nested_call = 0;
    // server.clients = listCreate();
    // server.clients_index = raxNew();
    // server.clients_to_close = listCreate();
    // server.slaves = listCreate();
    // server.monitors = listCreate();
    // server.clients_pending_write = listCreate();
    // server.clients_pending_read = listCreate();
    // server.clients_timeout_table = raxNew();
    server.replication_allowed = 1;
    // server.slaveseldb = -1; /* Force to emit the first SELECT command. */
    // server.unblocked_clients = listCreate();
    // server.ready_keys = listCreate();
    // server.tracking_pending_keys = listCreate();
    // server.clients_waiting_acks = listCreate();
    // server.get_ack_from_slaves = 0;
    server.paused_actions = 0;
    // memset(server.client_pause_per_purpose, 0,
    //        sizeof(server.client_pause_per_purpose));
    // server.postponed_clients = listCreate();
    server.events_processed_while_blocked = 0;
    server.system_memory_size = zmalloc_get_memory_size();
    server.blocked_last_cron = 0;
    server.blocking_op_nesting = 0;
    // server.thp_enabled = 0;
    // server.cluster_drop_packet_filter = -1;
    // server.reply_buffer_peak_reset_time = REPLY_BUFFER_DEFAULT_PEAK_RESET_TIME;
    // server.reply_buffer_resizing_enabled = 1;
    // resetReplicationBuffer();

    /* Make sure the locale is set on startup based on the config file. */
    // if (setlocale(LC_COLLATE,server.locale_collate) == NULL) {
    //     serverLog(LL_WARNING, "Failed to configure LOCALE for invalid locale name.");
    //     exit(1);
    // }

    // for (j = 0; j < CLIENT_MEM_USAGE_BUCKETS; j++) {
    //     server.client_mem_usage_buckets[j].mem_usage_sum = 0;
    //     server.client_mem_usage_buckets[j].clients = listCreate();
    // }

    createSharedObjects();
    adjustOpenFilesLimit();
    const char *clk_msg = monotonicInit();
    serverLog(LL_NOTICE, "monotonic clock: %s", clk_msg);
    server.el = aeCreateEventLoop(server.maxclients+CONFIG_FDSET_INCR);
    if (server.el == NULL) {
        serverLog(LL_WARNING,
            "Failed creating the event loop. Error message: '%s'",
            strerror(errno));
        exit(1);
    }
    // server.db = zmalloc(sizeof(redisDb)*server.dbnum);

    /* Create the Redis databases, and initialize other internal state. */
    // for (j = 0; j < server.dbnum; j++) {
    //     server.db[j].dict = dictCreate(&dbDictType);
    //     server.db[j].expires = dictCreate(&dbExpiresDictType);
    //     server.db[j].expires_cursor = 0;
    //     server.db[j].blocking_keys = dictCreate(&keylistDictType);
    //     server.db[j].blocking_keys_unblock_on_nokey = dictCreate(&objectKeyPointerValueDictType);
    //     server.db[j].ready_keys = dictCreate(&objectKeyPointerValueDictType);
    //     server.db[j].watched_keys = dictCreate(&keylistDictType);
    //     server.db[j].id = j;
    //     server.db[j].avg_ttl = 0;
    //     server.db[j].defrag_later = listCreate();
    //     server.db[j].slots_to_keys = NULL; /* Set by clusterInit later on if necessary. */
    //     listSetFreeMethod(server.db[j].defrag_later,(void (*)(void*))sdsfree);
    // }
    // evictionPoolAlloc(); /* Initialize the LRU keys pool. */
    // server.pubsub_channels = dictCreate(&keylistDictType);
    // server.pubsub_patterns = dictCreate(&keylistDictType);
    // server.pubsubshard_channels = dictCreate(&keylistDictType);
    // server.cronloops = 0;
    // server.in_exec = 0;
    // server.busy_module_yield_flags = BUSY_MODULE_YIELD_NONE;
    // server.busy_module_yield_reply = NULL;
    // server.core_propagates = 0;
    // server.module_ctx_nesting = 0;
    // server.client_pause_in_transaction = 0;
    // server.child_pid = -1;
    // server.child_type = CHILD_TYPE_NONE;
    // server.rdb_child_type = RDB_CHILD_TYPE_NONE;
    // server.rdb_pipe_conns = NULL;
    // server.rdb_pipe_numconns = 0;
    // server.rdb_pipe_numconns_writing = 0;
    // server.rdb_pipe_buff = NULL;
    // server.rdb_pipe_bufflen = 0;
    // server.rdb_bgsave_scheduled = 0;
    // server.child_info_pipe[0] = -1;
    // server.child_info_pipe[1] = -1;
    // server.child_info_nread = 0;
    // server.aof_buf = sdsempty();
    // server.lastsave = time(NULL); /* At startup we consider the DB saved. */
    // server.lastbgsave_try = 0;    /* At startup we never tried to BGSAVE. */
    // server.rdb_save_time_last = -1;
    // server.rdb_save_time_start = -1;
    // server.rdb_last_load_keys_expired = 0;
    // server.rdb_last_load_keys_loaded = 0;
    // server.dirty = 0;
    // resetServerStats();
    // /* A few stats we don't want to reset: server startup time, and peak mem. */
    // server.stat_starttime = time(NULL);
    // server.stat_peak_memory = 0;
    // server.stat_current_cow_peak = 0;
    // server.stat_current_cow_bytes = 0;
    // server.stat_current_cow_updated = 0;
    // server.stat_current_save_keys_processed = 0;
    // server.stat_current_save_keys_total = 0;
    // server.stat_rdb_cow_bytes = 0;
    // server.stat_aof_cow_bytes = 0;
    // server.stat_module_cow_bytes = 0;
    // server.stat_module_progress = 0;
    // for (int j = 0; j < CLIENT_TYPE_COUNT; j++)
    //     server.stat_clients_type_memory[j] = 0;
    // server.stat_cluster_links_memory = 0;
    // server.cron_malloc_stats.zmalloc_used = 0;
    // server.cron_malloc_stats.process_rss = 0;
    // server.cron_malloc_stats.allocator_allocated = 0;
    // server.cron_malloc_stats.allocator_active = 0;
    // server.cron_malloc_stats.allocator_resident = 0;
    // server.lastbgsave_status = C_OK;
    // server.aof_last_write_status = C_OK;
    // server.aof_last_write_errno = 0;
    // server.repl_good_slaves_count = 0;
    // server.last_sig_received = 0;

    // /* Initiate acl info struct */
    // server.acl_info.invalid_cmd_accesses = 0;
    // server.acl_info.invalid_key_accesses  = 0;
    // server.acl_info.user_auth_failures = 0;
    // server.acl_info.invalid_channel_accesses = 0;

    /* Create the timer callback, this is our way to process many background
     * operations incrementally, like clients timeout, eviction of unaccessed
     * expired keys and so forth. */
    if (aeCreateTimeEvent(server.el, 1, serverCron, NULL, NULL) == AE_ERR) {
        // serverPanic("Can't create event loop timers.");
        serverLog(LL_WARNING,"test --> Can't create event loop timers.");
        exit(1);
    }

    /* Register a readable event for the pipe used to awake the event loop
     * from module threads. */
    // if (aeCreateFileEvent(server.el, server.module_pipe[0], AE_READABLE,
    //     modulePipeReadable,NULL) == AE_ERR) {
    //         serverPanic(
    //             "Error registering the readable event for the module pipe.");
    // }

    /* Register before and after sleep handlers (note this needs to be done
     * before loading persistence since it is used by processEventsWhileBlocked. */
    aeSetBeforeSleepProc(server.el,beforeSleep);
    aeSetAfterSleepProc(server.el,afterSleep);

    /* 32 bit instances are limited to 4GB of address space, so if there is
     * no explicit limit in the user provided configuration we set a limit
     * at 3 GB using maxmemory with 'noeviction' policy'. This avoids
     * useless crashes of the Redis instance for out of memory. */
    if (server.arch_bits == 32 && server.maxmemory == 0) {
        serverLog(LL_WARNING,"Warning: 32 bit instance detected but no memory limit set. Setting 3 GB maxmemory limit with 'noeviction' policy now.");
        server.maxmemory = 3072LL*(1024*1024); /* 3 GB */
        server.maxmemory_policy = MAXMEMORY_NO_EVICTION;
    }

    // scriptingInit(1);
    // functionsInit();
    // slowlogInit();
    // latencyMonitorInit();

    /* Initialize ACL default password if it exists */
    // ACLUpdateDefaultUserPassword(server.requirepass);

    applyWatchdogPeriod();
}

#pragma endregion


# pragma region

/* Return the UNIX time in microseconds */
long long ustime(void) {
    struct timeval tv;
    long long ust;

    gettimeofday(&tv, NULL);
    ust = ((long long)tv.tv_sec)*1000000;
    ust += tv.tv_usec;
    return ust;
}
/* Return the UNIX time in milliseconds */
mstime_t mstime(void) {
    return ustime()/1000;
}
# pragma endregion
int main(int argc, char **argv)
{
    struct timeval tv;
    int j;
    char config_from_stdin = 0;
    printf("mainserver %d", argc);
    /* We need to initialize our libraries, and the server configuration. */

    //debug
    server.logfile="/var/log/redis/mylog.log";
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
    char *exec_name = strrchr(argv[0], '/');// strchr 返回参数在字符串中的最后一次的位置
    if (exec_name == NULL) exec_name = argv[0];
    server.sentinel_mode = checkForSentinelMode(argc,argv, exec_name);
    initServerConfig();
    ACLInit(); /* The ACL subsystem must be initialized ASAP because the
                  basic networking code and client creation depends on it. */
    // moduleInitModulesSystem();
    // connTypeInitialize();

    /* Store the executable path and arguments in a safe place in order
     * to be able to restart the server later. */
    server.executable = getAbsolutePath(argv[0]);
    server.exec_argv = zmalloc(sizeof(char*)*(argc+1));
    server.exec_argv[argc] = NULL;
    for (j = 0; j < argc; j++) server.exec_argv[j] = zstrdup(argv[j]);

      /* We need to init sentinel right now as parsing the configuration file
     * in sentinel mode will have the effect of populating the sentinel
     * data structures with master nodes to monitor. */
    if (server.sentinel_mode) {
        // initSentinelConfig();//哨兵模式，便于主从切换
        // initSentinel();
    }

     /* Check if we need to start in redis-check-rdb/aof mode. We just execute
     * the program main. However the program is part of the Redis executable
     * so that we can easily execute an RDB check on loading errors. */
    // if (strstr(exec_name,"redis-check-rdb") != NULL)
    //     redis_check_rdb_main(argc,argv,NULL);
    // else if (strstr(exec_name,"redis-check-aof") != NULL)
    //     redis_check_aof_main(argc,argv);

    if (argc >= 2) {
            j = 1; /* First option to parse in argv[] */
            sds options = sdsempty();
              /* Handle special options --help and --version */
    if (strcmp(argv[1], "-v") == 0 ||
        strcmp(argv[1], "--version") == 0) version();
    /* Parse command line options
        * Precedence wise, File, stdin, explicit options -- last config is the one that matters.
        *
        * First argument is the config file name? */
    if (argv[1][0] != '-') {
        /* Replace the config file in server.exec_argv with its absolute path. */
        server.configfile = getAbsolutePath(argv[1]);
        zfree(server.exec_argv[1]);
        server.exec_argv[1] = zstrdup(server.configfile);
        j = 2; // Skip this arg when parsing options
        }
    sds *argv_tmp;
    int argc_tmp;
    int handled_last_config_arg = 1;
    while(j < argc) {
            /* Either first or last argument - Should we read config from stdin? */
            if (argv[j][0] == '-' && argv[j][1] == '\0' && (j == 1 || j == argc-1)) {

            }
            /* All the other options are parsed and conceptually appended to the
             * configuration file. For instance --port 6380 will generate the
             * string "port 6380\n" to be parsed after the actual config file
             * and stdin input are parsed (if they exist).
             * Only consider that if the last config has at least one argument. */
            else if (handled_last_config_arg && argv[j][0] == '-' && argv[j][1] == '-'){

            }
            else{
                 /* Option argument */
                options = sdscatrepr(options,argv[j],strlen(argv[j]));
                options = sdscat(options," ");
                handled_last_config_arg = 1;
            }
            j++;

        }
        loadServerConfig(server.configfile, config_from_stdin, options);
        // if (server.sentinel_mode) loadSentinelConfigFromQueue();
        sdsfree(options);

    }
    

    if (argc == 1) {
        serverLog(LL_WARNING, "Warning: no config file specified, using the default config. In order to specify a config file use %s /path/to/redis.conf", argv[0]);
    } else {
        serverLog(LL_WARNING, "Configuration loaded");
    }
    initServer();

    aeMain(server.el);

}