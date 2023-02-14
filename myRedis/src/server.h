#ifndef __REDIS_H
#define __REDIS_H

#include "fmacros.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <signal.h>
#include <pthread.h> //about threads
#include <sys/socket.h> //connect with client

#include "zmalloc.h" /* total memory usage aware version of malloc/free */
#include "endianconv.h"
#include "crc64.h"
#include "util.h"    /* Misc functions useful in many places */

#include "dict.h"    /* Hash tables */
#include "atomicvar.h"
// #include "sds.h"
#include "adlist.h"  /* Linked lists */
#include "ae.h"      /* Event driven programming library */
#include "anet.h"    /* Networking the easy way */
#include "connection.h" /* Connection abstraction */
// #include "rax.h"     /* Radix tree */
// #include "rio.h"

#include "version.h"

typedef long long mstime_t; /* millisecond time type. */
typedef long long ustime_t; /* microsecond time type. */
#define REDISMODULE_CORE 1 // about redismodule
typedef struct redisObject robj;
#include "redismodule.h"    /* Redis modules API defines. */


/* Objects encoding. Some kind of objects like Strings and Hashes can be
 * internally represented in multiple ways. The 'encoding' field of the object
 * is set to one of this fields for this object. */
#define OBJ_ENCODING_RAW 0     /* Raw representation */
#define OBJ_ENCODING_INT 1     /* Encoded as integer */
#define OBJ_ENCODING_HT 2      /* Encoded as hash table */
#define OBJ_ENCODING_ZIPMAP 3  /* No longer used: old hash encoding. */
#define OBJ_ENCODING_LINKEDLIST 4 /* No longer used: old list encoding. */
#define OBJ_ENCODING_ZIPLIST 5 /* No longer used: old list/hash/zset encoding. */
#define OBJ_ENCODING_INTSET 6  /* Encoded as intset */
#define OBJ_ENCODING_SKIPLIST 7  /* Encoded as skiplist */
#define OBJ_ENCODING_EMBSTR 8  /* Embedded sds string encoding */
#define OBJ_ENCODING_QUICKLIST 9 /* Encoded as linked list of listpacks */
#define OBJ_ENCODING_STREAM 10 /* Encoded as a radix tree of listpacks */
#define OBJ_ENCODING_LISTPACK 11 /* Encoded as a listpack */

#define OBJ_SHARED_REFCOUNT INT_MAX     /* Global object never destroyed. */
#define OBJ_STATIC_REFCOUNT (INT_MAX-1) /* Object allocated in the stack. */
#define OBJ_FIRST_SPECIAL_REFCOUNT OBJ_STATIC_REFCOUNT


/* Protocol and I/O related defines */
#define PROTO_IOBUF_LEN         (1024*16)  /* Generic I/O buffer size */
#define PROTO_REPLY_CHUNK_BYTES (16*1024) /* 16k output buffer */
#define PROTO_INLINE_MAX_SIZE   (1024*64) /* Max size of inline reads */
#define PROTO_MBULK_BIG_ARG     (1024*32)
#define PROTO_RESIZE_THRESHOLD  (1024*32) /* Threshold for determining whether to resize query buffer */
#define PROTO_REPLY_MIN_BYTES   (1024) /* the lower limit on reply buffer size */
#define REDIS_AUTOSYNC_BYTES (1024*1024*4) /* Sync file every 4MB. */

#define REPLY_BUFFER_DEFAULT_PEAK_RESET_TIME 5000 /* 5 seconds */

/* Client block type (btype field in client structure)
 * if CLIENT_BLOCKED flag is set. */
#define BLOCKED_NONE 0    /* Not blocked, no CLIENT_BLOCKED flag set. */
#define BLOCKED_LIST 1    /* BLPOP & co. */
#define BLOCKED_WAIT 2    /* WAIT for synchronous replication. */
#define BLOCKED_MODULE 3  /* Blocked by a loadable module. */
#define BLOCKED_STREAM 4  /* XREAD. */
#define BLOCKED_ZSET 5    /* BZPOP et al. */
#define BLOCKED_POSTPONE 6 /* Blocked by processCommand, re-try processing later. */
#define BLOCKED_SHUTDOWN 7 /* SHUTDOWN. */
#define BLOCKED_NUM 8      /* Number of blocked states. */

/* AIX defines hz to __hz, we don't use this define and in order to allow
 * Redis build on AIX we need to undef it. */
#ifdef _AIX
#undef hz
#endif

#define CHILD_TYPE_NONE 0
#define CHILD_TYPE_RDB 1
#define CHILD_TYPE_AOF 2
#define CHILD_TYPE_LDB 3
#define CHILD_TYPE_MODULE 4

/* Log levels */
#define LL_DEBUG 0
#define LL_VERBOSE 1
#define LL_NOTICE 2
#define LL_WARNING 3
#define LL_RAW (1<<10) /* Modifier to log without timestamp */



#define LRU_BITS 24
#define EMPTYDB_NO_FLAGS 0      /* No flags. */

/* TLS Client Authentication */
#define TLS_CLIENT_AUTH_NO 0
#define TLS_CLIENT_AUTH_YES 1
#define TLS_CLIENT_AUTH_OPTIONAL 2

/* Client flags */
#define CLIENT_SLAVE (1<<0)   /* This client is a replica */
#define CLIENT_MASTER (1<<1)  /* This client is a master */
#define CLIENT_MONITOR (1<<2) /* This client is a slave monitor, see MONITOR */
#define CLIENT_MULTI (1<<3)   /* This client is in a MULTI context */
#define CLIENT_BLOCKED (1<<4) /* The client is waiting in a blocking operation */
#define CLIENT_DIRTY_CAS (1<<5) /* Watched keys modified. EXEC will fail. */
#define CLIENT_CLOSE_AFTER_REPLY (1<<6) /* Close after writing entire reply. */
#define CLIENT_UNBLOCKED (1<<7) /* This client was unblocked and is stored in
                                  server.unblocked_clients */
#define CLIENT_SCRIPT (1<<8) /* This is a non connected client used by Lua */
#define CLIENT_ASKING (1<<9)     /* Client issued the ASKING command */
#define CLIENT_CLOSE_ASAP (1<<10)/* Close this client ASAP */
#define CLIENT_UNIX_SOCKET (1<<11) /* Client connected via Unix domain socket */
#define CLIENT_DIRTY_EXEC (1<<12)  /* EXEC will fail for errors while queueing */
#define CLIENT_MASTER_FORCE_REPLY (1<<13)  /* Queue replies even if is master */
#define CLIENT_FORCE_AOF (1<<14)   /* Force AOF propagation of current cmd. */
#define CLIENT_FORCE_REPL (1<<15)  /* Force replication of current cmd. */
#define CLIENT_PRE_PSYNC (1<<16)   /* Instance don't understand PSYNC. */
#define CLIENT_READONLY (1<<17)    /* Cluster client is in read-only state. */
#define CLIENT_PUBSUB (1<<18)      /* Client is in Pub/Sub mode. */
#define CLIENT_PREVENT_AOF_PROP (1<<19)  /* Don't propagate to AOF. */
#define CLIENT_PREVENT_REPL_PROP (1<<20)  /* Don't propagate to slaves. */
#define CLIENT_PREVENT_PROP (CLIENT_PREVENT_AOF_PROP|CLIENT_PREVENT_REPL_PROP)
#define CLIENT_PENDING_WRITE (1<<21) /* Client has output to send but a write
                                        handler is yet not installed. */
#define CLIENT_REPLY_OFF (1<<22)   /* Don't send replies to client. */
#define CLIENT_REPLY_SKIP_NEXT (1<<23)  /* Set CLIENT_REPLY_SKIP for next cmd */
#define CLIENT_REPLY_SKIP (1<<24)  /* Don't send just this reply. */
#define CLIENT_LUA_DEBUG (1<<25)  /* Run EVAL in debug mode. */
#define CLIENT_LUA_DEBUG_SYNC (1<<26)  /* EVAL debugging without fork() */
#define CLIENT_MODULE (1<<27) /* Non connected client used by some module. */
#define CLIENT_PROTECTED (1<<28) /* Client should not be freed for now. */
/* #define CLIENT_... (1<<29) currently unused, feel free to use in the future */
#define CLIENT_PENDING_COMMAND (1<<30) /* Indicates the client has a fully
                                        * parsed command ready for execution. */
#define CLIENT_TRACKING (1ULL<<31) /* Client enabled keys tracking in order to
                                   perform client side caching. */
#define CLIENT_TRACKING_BROKEN_REDIR (1ULL<<32) /* Target client is invalid. */
#define CLIENT_TRACKING_BCAST (1ULL<<33) /* Tracking in BCAST mode. */
#define CLIENT_TRACKING_OPTIN (1ULL<<34)  /* Tracking in opt-in mode. */
#define CLIENT_TRACKING_OPTOUT (1ULL<<35) /* Tracking in opt-out mode. */
#define CLIENT_TRACKING_CACHING (1ULL<<36) /* CACHING yes/no was given,
                                              depending on optin/optout mode. */
#define CLIENT_TRACKING_NOLOOP (1ULL<<37) /* Don't send invalidation messages
                                             about writes performed by myself.*/
#define CLIENT_IN_TO_TABLE (1ULL<<38) /* This client is in the timeout table. */
#define CLIENT_PROTOCOL_ERROR (1ULL<<39) /* Protocol error chatting with it. */
#define CLIENT_CLOSE_AFTER_COMMAND (1ULL<<40) /* Close after executing commands
                                               * and writing entire reply. */
#define CLIENT_DENY_BLOCKING (1ULL<<41) /* Indicate that the client should not be blocked.
                                           currently, turned on inside MULTI, Lua, RM_Call,
                                           and AOF client */
#define CLIENT_REPL_RDBONLY (1ULL<<42) /* This client is a replica that only wants
                                          RDB without replication buffer. */
#define CLIENT_NO_EVICT (1ULL<<43) /* This client is protected against client
                                      memory eviction. */
#define CLIENT_ALLOW_OOM (1ULL<<44) /* Client used by RM_Call is allowed to fully execute
                                       scripts even when in OOM */

/* Client request types */
#define PROTO_REQ_INLINE 1
#define PROTO_REQ_MULTIBULK 2

/* Error codes */
#define C_OK                    0
#define C_ERR                   -1

/* Static server configuration */
#define CONFIG_DEFAULT_HZ        10             /* Time interrupt calls/sec. */
#define CONFIG_MIN_HZ            1
#define CONFIG_MAX_HZ            500
#define MAX_CLIENTS_PER_CLOCK_TICK 200          /* HZ is adapted based on that. */
#define CRON_DBS_PER_CALL 16
#define NET_MAX_WRITES_PER_EVENT (1024*64)
#define PROTO_SHARED_SELECT_CMDS 10
#define OBJ_SHARED_INTEGERS 10000
#define OBJ_SHARED_BULKHDR_LEN 32
#define OBJ_SHARED_HDR_STRLEN(_len_) (((_len_) < 10) ? 4 : 5) /* see shared.mbulkhdr etc. */
#define LOG_MAX_LEN    1024 /* Default maximum length of syslog messages.*/
#define AOF_REWRITE_ITEMS_PER_CMD 64
#define AOF_ANNOTATION_LINE_MAX_LEN 1024
#define CONFIG_RUN_ID_SIZE 40
#define RDB_EOF_MARK_SIZE 40
#define CONFIG_REPL_BACKLOG_MIN_SIZE (1024*16)          /* 16k */
#define CONFIG_BGSAVE_RETRY_DELAY 5 /* Wait a few secs before trying again. */
#define CONFIG_DEFAULT_PID_FILE "/var/run/redis.pid"
#define CONFIG_DEFAULT_BINDADDR_COUNT 2
#define CONFIG_DEFAULT_BINDADDR { "*", "-::*" }
#define NET_HOST_STR_LEN 256 /* Longest valid hostname */
#define NET_IP_STR_LEN 46 /* INET6_ADDRSTRLEN is 46, but we need to be sure */
#define NET_ADDR_STR_LEN (NET_IP_STR_LEN+32) /* Must be enough for ip:port */
#define NET_HOST_PORT_STR_LEN (NET_HOST_STR_LEN+32) /* Must be enough for hostname:port */
#define CONFIG_BINDADDR_MAX 16
#define CONFIG_MIN_RESERVED_FDS 32
#define CONFIG_DEFAULT_PROC_TITLE_TEMPLATE "{title} {listen-addr} {server-mode}"

/* The actual Redis Object */
#define OBJ_STRING 0    /* String object. */
#define OBJ_LIST 1      /* List object. */
#define OBJ_SET 2       /* Set object. */
#define OBJ_ZSET 3      /* Sorted set object. */
#define OBJ_HASH 4      /* Hash object. */
/* The "module" object type is a special one that signals that the object
 * is one directly managed by a Redis module. In this case the value points
 * to a moduleValue struct, which contains the object value (which is only
 * handled by the module itself) and the RedisModuleType struct which lists
 * function pointers in order to serialize, deserialize, AOF-rewrite and
 * free the object.
 *
 * Inside the RDB file, module types are encoded as OBJ_MODULE followed
 * by a 64 bit module type ID, which has a 54 bits module-specific signature
 * in order to dispatch the loading to the right module, plus a 10 bits
 * encoding version. */
#define OBJ_MODULE 5    /* Module object. */
#define OBJ_STREAM 6    /* Stream object. */

/* When configuring the server eventloop, we setup it so that the total number
 * of file descriptors we can handle are server.maxclients + RESERVED_FDS +
 * a few more to stay safe. Since RESERVED_FDS defaults to 32, we add 96
 * in order to make sure of not over provisioning more than 128 fds. */
#define CONFIG_FDSET_INCR (CONFIG_MIN_RESERVED_FDS+96)

/* Redis maxmemory strategies. Instead of using just incremental number
 * for this defines, we use a set of flags so that testing for certain
 * properties common to multiple policies is faster. */
#define MAXMEMORY_FLAG_LRU (1<<0)
#define MAXMEMORY_FLAG_LFU (1<<1)
#define MAXMEMORY_FLAG_ALLKEYS (1<<2)
#define MAXMEMORY_FLAG_NO_SHARED_INTEGERS \
    (MAXMEMORY_FLAG_LRU|MAXMEMORY_FLAG_LFU)

#define MAXMEMORY_VOLATILE_LRU ((0<<8)|MAXMEMORY_FLAG_LRU)
#define MAXMEMORY_VOLATILE_LFU ((1<<8)|MAXMEMORY_FLAG_LFU)
#define MAXMEMORY_VOLATILE_TTL (2<<8)
#define MAXMEMORY_VOLATILE_RANDOM (3<<8)
#define MAXMEMORY_ALLKEYS_LRU ((4<<8)|MAXMEMORY_FLAG_LRU|MAXMEMORY_FLAG_ALLKEYS)
#define MAXMEMORY_ALLKEYS_LFU ((5<<8)|MAXMEMORY_FLAG_LFU|MAXMEMORY_FLAG_ALLKEYS)
#define MAXMEMORY_ALLKEYS_RANDOM ((6<<8)|MAXMEMORY_FLAG_ALLKEYS)
#define MAXMEMORY_NO_EVICTION (7<<8)

/* Macro used to initialize a Redis object allocated on the stack.
 * Note that this macro is taken near the structure definition to make sure
 * we'll update it when the structure is changed, to avoid bugs like
 * bug #85 introduced exactly in this way. */
#define initStaticStringObject(_var,_ptr) do { \
    _var.refcount = OBJ_STATIC_REFCOUNT; \
    _var.type = OBJ_STRING; \
    _var.encoding = OBJ_ENCODING_RAW; \
    _var.ptr = _ptr; \
} while(0)

#define CLIENT_TYPE_NORMAL 0 /* Normal req-reply clients + MONITORs */
#define CLIENT_TYPE_SLAVE 1  /* Slaves. */
#define CLIENT_TYPE_PUBSUB 2 /* Clients subscribed to PubSub channels. */
#define CLIENT_TYPE_MASTER 3 /* Master. */
#define CLIENT_TYPE_COUNT 4  /* Total number of client types. */
#define CLIENT_TYPE_OBUF_COUNT 3 /* Number of clients to expose to output


/* We can print the stacktrace, so our assert is defined this way: */
// #define serverAssertWithInfo(_c,_o,_e) ((_e)?(void)0 : (_serverAssertWithInfo(_c,_o,#_e,__FILE__,__LINE__),redis_unreachable()))
#ifdef __GNUC__
void _serverPanic(const char *file, int line, const char *msg, ...)
    __attribute__ ((format (printf, 3, 4)));
#else
void _serverPanic(const char *file, int line, const char *msg, ...);
#endif
#define serverAssert(_e) ((_e)?(void)0 : (_serverAssert(#_e,__FILE__,__LINE__),redis_unreachable()))
#define serverPanic(...) _serverPanic(__FILE__,__LINE__,__VA_ARGS__),redis_unreachable()

/* This structure can be optionally passed to RDB save/load functions in
 * order to implement additional functionalities, by storing and loading
 * metadata to the RDB file.
 *
 * For example, to use select a DB at load time, useful in
 * replication in order to make sure that chained slaves (slaves of slaves)
 * select the correct DB and are able to accept the stream coming from the
 * top-level master. */
typedef struct rdbSaveInfo {
    /* Used saving and loading. */
    int repl_stream_db;  /* DB to select in server.master client. */

    /* Used only loading. */
    int repl_id_is_set;  /* True if repl_id field is set. */
    char repl_id[CONFIG_RUN_ID_SIZE+1];     /* Replication ID. */
    long long repl_offset;                  /* Replication offset. */
} rdbSaveInfo;

typedef struct {
    sds name;       /* The username as an SDS string. */
    uint32_t flags; /* See USER_FLAG_* */
    list *passwords; /* A list of SDS valid passwords for this user. */
    list *selectors; /* A list of selectors this user validates commands
                        against. This list will always contain at least
                        one selector for backwards compatibility. */
    robj *acl_string; /* cached string represent of ACLs */
} user;

struct RedisModule;
struct RedisModuleIO;
struct RedisModuleDigest;
struct RedisModuleCtx;
struct moduleLoadQueueEntry;
struct RedisModuleKeyOptCtx;
struct RedisModuleCommand;

/* Each module type implementation should export a set of methods in order
 * to serialize and deserialize the value in the RDB file, rewrite the AOF
 * log, create the digest for "DEBUG DIGEST", and free the value when a key
 * is deleted. */
typedef void *(*moduleTypeLoadFunc)(struct RedisModuleIO *io, int encver);
typedef void (*moduleTypeSaveFunc)(struct RedisModuleIO *io, void *value);
typedef int (*moduleTypeAuxLoadFunc)(struct RedisModuleIO *rdb, int encver, int when);
typedef void (*moduleTypeAuxSaveFunc)(struct RedisModuleIO *rdb, int when);
typedef void (*moduleTypeRewriteFunc)(struct RedisModuleIO *io, struct redisObject *key, void *value);
typedef void (*moduleTypeDigestFunc)(struct RedisModuleDigest *digest, void *value);
typedef size_t (*moduleTypeMemUsageFunc)(const void *value);
typedef void (*moduleTypeFreeFunc)(void *value);
typedef size_t (*moduleTypeFreeEffortFunc)(struct redisObject *key, const void *value);
typedef void (*moduleTypeUnlinkFunc)(struct redisObject *key, void *value);
typedef void *(*moduleTypeCopyFunc)(struct redisObject *fromkey, struct redisObject *tokey, const void *value);
typedef int (*moduleTypeDefragFunc)(struct RedisModuleDefragCtx *ctx, struct redisObject *key, void **value);
typedef size_t (*moduleTypeMemUsageFunc2)(struct RedisModuleKeyOptCtx *ctx, const void *value, size_t sample_size);
typedef void (*moduleTypeFreeFunc2)(struct RedisModuleKeyOptCtx *ctx, void *value);
typedef size_t (*moduleTypeFreeEffortFunc2)(struct RedisModuleKeyOptCtx *ctx, const void *value);
typedef void (*moduleTypeUnlinkFunc2)(struct RedisModuleKeyOptCtx *ctx, void *value);
typedef void *(*moduleTypeCopyFunc2)(struct RedisModuleKeyOptCtx *ctx, const void *value);

/* The module type, which is referenced in each value of a given type, defines
 * the methods and links to the module exporting the type. */
typedef struct RedisModuleType {
    uint64_t id; /* Higher 54 bits of type ID + 10 lower bits of encoding ver. */
    struct RedisModule *module;
    moduleTypeLoadFunc rdb_load;
    moduleTypeSaveFunc rdb_save;
    moduleTypeRewriteFunc aof_rewrite;
    moduleTypeMemUsageFunc mem_usage;
    moduleTypeDigestFunc digest;
    moduleTypeFreeFunc free;
    moduleTypeFreeEffortFunc free_effort;
    moduleTypeUnlinkFunc unlink;
    moduleTypeCopyFunc copy;
    moduleTypeDefragFunc defrag;
    moduleTypeAuxLoadFunc aux_load;
    moduleTypeAuxSaveFunc aux_save;
    moduleTypeMemUsageFunc2 mem_usage2;
    moduleTypeFreeEffortFunc2 free_effort2;
    moduleTypeUnlinkFunc2 unlink2;
    moduleTypeCopyFunc2 copy2;
    moduleTypeAuxSaveFunc aux_save2;
    int aux_save_triggers;
    char name[10]; /* 9 bytes name + null term. Charset: A-Z a-z 0-9 _- */
} moduleType;

/* This is a wrapper for the 'rio' streams used inside rdb.c in Redis, so that
 * the user does not have to take the total count of the written bytes nor
 * to care about error conditions. */
struct RedisModuleIO {
    size_t bytes;       /* Bytes read / written so far. */
    // rio *rio;           /* Rio stream. */
    moduleType *type;   /* Module type doing the operation. */
    int error;          /* True if error condition happened. */
    struct RedisModuleCtx *ctx; /* Optional context, see RM_GetContextFromIO()*/
    struct redisObject *key;    /* Optional name of key processed */
    int dbid;            /* The dbid of the key being processed, -1 when unknown. */
    sds pre_flush_buffer; /* A buffer that should be flushed before next write operation
                           * See rdbSaveSingleModuleAux for more details */
};


/* This structure represents a module inside the system. */
struct RedisModule {
    void *handle;   /* Module dlopen() handle. */
    char *name;     /* Module name. */
    int ver;        /* Module version. We use just progressive integers. */
    int apiver;     /* Module API version as requested during initialization.*/
    // list *types;    /* Module data types. */
    // list *usedby;   /* List of modules using APIs from this one. */
    // list *using;    /* List of modules we use some APIs of. */
    // list *filters;  /* List of filters the module has registered. */
    // list *module_configs; /* List of configurations the module has registered */
    int configs_initialized; /* Have the module configurations been initialized? */
    int in_call;    /* RM_Call() nesting level */
    int in_hook;    /* Hooks callback nesting level for this module (0 or 1). */
    int options;    /* Module options and capabilities. */
    int blocked_clients;         /* Count of RedisModuleBlockedClient in this module. */
    // RedisModuleInfoFunc info_cb; /* Callback for module to add INFO fields. */
    // RedisModuleDefragFunc defrag_cb;    /* Callback for global data defrag. */
    struct moduleLoadQueueEntry *loadmod; /* Module load arguments for config rewrite. */
};
typedef struct RedisModule RedisModule;

typedef struct redisObject {
    unsigned type:4;
    unsigned encoding:4;
    unsigned lru:LRU_BITS; /* LRU time (relative to global lru_clock) or
                            * LFU data (least significant 8 bits frequency
                            * and most significant 16 bits access time). */
    int refcount;
    void *ptr;
};

/* This structure holds the blocking operation state for a client.
 * The fields used depend on client->btype. */
typedef struct blockingState {
    /* Generic fields. */
    long count;             /* Elements to pop if count was specified (BLMPOP/BZMPOP), -1 otherwise. */
    mstime_t timeout;       /* Blocking operation timeout. If UNIX current time
                             * is > timeout then the operation timed out. */

    /* BLOCKED_LIST, BLOCKED_ZSET and BLOCKED_STREAM */
    dict *keys;             /* The keys we are waiting to terminate a blocking
                             * operation such as BLPOP or XREAD. Or NULL. */
    robj *target;           /* The key that should receive the element,
                             * for BLMOVE. */
    struct blockPos {
        int wherefrom;      /* Where to pop from */
        int whereto;        /* Where to push to */
    } blockpos;              /* The positions in the src/dst lists/zsets
                             * where we want to pop/push an element
                             * for BLPOP, BRPOP, BLMOVE and BZMPOP. */

    /* BLOCK_STREAM */
    size_t xread_count;     /* XREAD COUNT option. */
    robj *xread_group;      /* XREADGROUP group name. */
    robj *xread_consumer;   /* XREADGROUP consumer name. */
    int xread_group_noack;

    /* BLOCKED_WAIT */
    int numreplicas;        /* Number of replicas we are waiting for ACK. */
    long long reploffset;   /* Replication offset to reach. */

    /* BLOCKED_MODULE */
    void *module_blocked_handle; /* RedisModuleBlockedClient structure.
                                    which is opaque for the Redis core, only
                                    handled in module.c. */
} blockingState;

typedef struct {
    list *clients;
    size_t mem_usage_sum;
} clientMemUsageBucket;

/* Replication error behavior determines the replica behavior
 * when it receives an error over the replication stream. In
 * either case the error is logged. */
typedef enum {
    PROPAGATION_ERR_BEHAVIOR_IGNORE = 0,
    PROPAGATION_ERR_BEHAVIOR_PANIC,
    PROPAGATION_ERR_BEHAVIOR_PANIC_ON_REPLICAS
} replicationErrorBehavior;

typedef struct clientBufferLimitsConfig {
    unsigned long long hard_limit_bytes;
    unsigned long long soft_limit_bytes;
    time_t soft_limit_seconds;
} clientBufferLimitsConfig;


/* Opaque type for the Slot to Key API. */
typedef struct clusterSlotToKeyMapping clusterSlotToKeyMapping;

/* Redis database representation. There are multiple databases identified
 * by integers from 0 (the default database) up to the max configured
 * database. The database number is the 'id' field in the structure. */
typedef struct redisDb {
    // dict *dict;                 /* The keyspace for this DB */
    // dict *expires;              /* Timeout of keys with a timeout set */
    // dict *blocking_keys;        /* Keys with clients waiting for data (BLPOP)*/
    // dict *blocking_keys_unblock_on_nokey;   /* Keys with clients waiting for
    //                                          * data, and should be unblocked if key is deleted (XREADEDGROUP).
    //                                          * This is a subset of blocking_keys*/
    // dict *ready_keys;           /* Blocked keys that received a PUSH */
    // dict *watched_keys;         /* WATCHED keys for MULTI/EXEC CAS */
    int id;                     /* Database ID */
    long long avg_ttl;          /* Average TTL, just for stats */
    unsigned long expires_cursor; /* Cursor of the active expire cycle. */
    // list *defrag_later;         /* List of key names to attempt to defrag one by one, gradually. */
    clusterSlotToKeyMapping *slots_to_keys; /* Array of slots to keys. Only used in cluster mode (db 0). */
} redisDb;

/* Replication backlog is not separate memory, it just is one consumer of
 * the global replication buffer. This structure records the reference of
 * replication buffers. Since the replication buffer block list may be very long,
 * it would cost much time to search replication offset on partial resync, so
 * we use one rax tree to index some blocks every REPL_BACKLOG_INDEX_PER_BLOCKS
 * to make searching offset from replication buffer blocks list faster. */
typedef struct replBacklog {
    listNode *ref_repl_buf_node; /* Referenced node of replication buffer blocks,
                                  * see the definition of replBufBlock. */
    size_t unindexed_count;      /* The count from last creating index block. */
    // rax *blocks_index;           /* The index of recorded blocks of replication
    //                               * buffer for quickly searching replication
    //                               * offset on partial resynchronization. */
    long long histlen;           /* Backlog actual data length */
    long long offset;            /* Replication "master offset" of first
                                  * byte in the replication backlog buffer.*/
} replBacklog;

/* forward declaration for functions ctx */
typedef struct functionsLibCtx functionsLibCtx;

/* Holding object that need to be populated during
 * rdb loading. On loading end it is possible to decide
 * whether not to set those objects on their rightful place.
 * For example: dbarray need to be set as main database on
 *              successful loading and dropped on failure. */
typedef struct rdbLoadingCtx {
    redisDb* dbarray;
    functionsLibCtx* functions_lib_ctx;
}rdbLoadingCtx;

struct sharedObjectsStruct {
    robj *ok, *err, *emptybulk, *czero, *cone, *pong, *space,
    *queued, *null[4], *nullarray[4], *emptymap[4], *emptyset[4],
    *emptyarray, *wrongtypeerr, *nokeyerr, *syntaxerr, *sameobjecterr,
    *outofrangeerr, *noscripterr, *loadingerr,
    *slowevalerr, *slowscripterr, *slowmoduleerr, *bgsaveerr,
    *masterdownerr, *roslaveerr, *execaborterr, *noautherr, *noreplicaserr,
    *busykeyerr, *oomerr, *plus, *messagebulk, *pmessagebulk, *subscribebulk,
    *unsubscribebulk, *psubscribebulk, *punsubscribebulk, *del, *unlink,
    *rpop, *lpop, *lpush, *rpoplpush, *lmove, *blmove, *zpopmin, *zpopmax,
    *emptyscan, *multi, *exec, *left, *right, *hset, *srem, *xgroup, *xclaim,  
    *script, *replconf, *eval, *persist, *set, *pexpireat, *pexpire, 
    *time, *pxat, *absttl, *retrycount, *force, *justid, *entriesread,
    *lastid, *ping, *setid, *keepttl, *load, *createconsumer,
    *getack, *special_asterick, *special_equals, *default_username, *redacted,
    *ssubscribebulk,*sunsubscribebulk, *smessagebulk,
    *select[PROTO_SHARED_SELECT_CMDS],
    *integers[OBJ_SHARED_INTEGERS],
    *mbulkhdr[OBJ_SHARED_BULKHDR_LEN], /* "*<value>\r\n" */
    *bulkhdr[OBJ_SHARED_BULKHDR_LEN],  /* "$<value>\r\n" */
    *maphdr[OBJ_SHARED_BULKHDR_LEN],   /* "%<value>\r\n" */
    *sethdr[OBJ_SHARED_BULKHDR_LEN];   /* "~<value>\r\n" */
    sds minstring, maxstring;
};

/* Redis command structure.
 *
 * Note that the command table is in commands.c and it is auto-generated.
 *
 * This is the meaning of the flags:
 *
 * CMD_WRITE:       Write command (may modify the key space).
 *
 * CMD_READONLY:    Commands just reading from keys without changing the content.
 *                  Note that commands that don't read from the keyspace such as
 *                  TIME, SELECT, INFO, administrative commands, and connection
 *                  or transaction related commands (multi, exec, discard, ...)
 *                  are not flagged as read-only commands, since they affect the
 *                  server or the connection in other ways.
 *
 * CMD_DENYOOM:     May increase memory usage once called. Don't allow if out
 *                  of memory.
 *
 * CMD_ADMIN:       Administrative command, like SAVE or SHUTDOWN.
 *
 * CMD_PUBSUB:      Pub/Sub related command.
 *
 * CMD_NOSCRIPT:    Command not allowed in scripts.
 *
 * CMD_BLOCKING:    The command has the potential to block the client.
 *
 * CMD_LOADING:     Allow the command while loading the database.
 *
 * CMD_NO_ASYNC_LOADING: Deny during async loading (when a replica uses diskless
 *                       sync swapdb, and allows access to the old dataset)
 *
 * CMD_STALE:       Allow the command while a slave has stale data but is not
 *                  allowed to serve this data. Normally no command is accepted
 *                  in this condition but just a few.
 *
 * CMD_SKIP_MONITOR:  Do not automatically propagate the command on MONITOR.
 *
 * CMD_SKIP_SLOWLOG:  Do not automatically propagate the command to the slowlog.
 *
 * CMD_ASKING:      Perform an implicit ASKING for this command, so the
 *                  command will be accepted in cluster mode if the slot is marked
 *                  as 'importing'.
 *
 * CMD_FAST:        Fast command: O(1) or O(log(N)) command that should never
 *                  delay its execution as long as the kernel scheduler is giving
 *                  us time. Note that commands that may trigger a DEL as a side
 *                  effect (like SET) are not fast commands.
 *
 * CMD_NO_AUTH:     Command doesn't require authentication
 *
 * CMD_MAY_REPLICATE:   Command may produce replication traffic, but should be
 *                      allowed under circumstances where write commands are disallowed.
 *                      Examples include PUBLISH, which replicates pubsub messages,and
 *                      EVAL, which may execute write commands, which are replicated,
 *                      or may just execute read commands. A command can not be marked
 *                      both CMD_WRITE and CMD_MAY_REPLICATE
 *
 * CMD_SENTINEL:    This command is present in sentinel mode.
 *
 * CMD_ONLY_SENTINEL: This command is present only when in sentinel mode.
 *                    And should be removed from redis.
 *
 * CMD_NO_MANDATORY_KEYS: This key arguments for this command are optional.
 *
 * CMD_NO_MULTI: The command is not allowed inside a transaction
 *
 * The following additional flags are only used in order to put commands
 * in a specific ACL category. Commands can have multiple ACL categories.
 * See redis.conf for the exact meaning of each.
 *
 * @keyspace, @read, @write, @set, @sortedset, @list, @hash, @string, @bitmap,
 * @hyperloglog, @stream, @admin, @fast, @slow, @pubsub, @blocking, @dangerous,
 * @connection, @transaction, @scripting, @geo.
 *
 * Note that:
 *
 * 1) The read-only flag implies the @read ACL category.
 * 2) The write flag implies the @write ACL category.
 * 3) The fast flag implies the @fast ACL category.
 * 4) The admin flag implies the @admin and @dangerous ACL category.
 * 5) The pub-sub flag implies the @pubsub ACL category.
 * 6) The lack of fast flag implies the @slow ACL category.
 * 7) The non obvious "keyspace" category includes the commands
 *    that interact with keys without having anything to do with
 *    specific data structures, such as: DEL, RENAME, MOVE, SELECT,
 *    TYPE, EXPIRE*, PEXPIRE*, TTL, PTTL, ...
 */
struct redisCommand {
    /* Declarative data */
    const char *declared_name; /* A string representing the command declared_name.
                                * It is a const char * for native commands and SDS for module commands. */
    const char *summary; /* Summary of the command (optional). */
    const char *complexity; /* Complexity description (optional). */
    const char *since; /* Debut version of the command (optional). */
    int doc_flags; /* Flags for documentation (see CMD_DOC_*). */
    const char *replaced_by; /* In case the command is deprecated, this is the successor command. */
    const char *deprecated_since; /* In case the command is deprecated, when did it happen? */
    // redisCommandGroup group; /* Command group */
    // commandHistory *history; /* History of the command */
    const char **tips; /* An array of strings that are meant to be tips for clients/proxies regarding this command */
    // redisCommandProc *proc; /* Command implementation */
    int arity; /* Number of arguments, it is possible to use -N to say >= N */
    uint64_t flags; /* Command flags, see CMD_*. */
    uint64_t acl_categories; /* ACl categories, see ACL_CATEGORY_*. */
    // keySpec key_specs_static[STATIC_KEY_SPECS_NUM]; /* Key specs. See keySpec */
    /* Use a function to determine keys arguments in a command line.
     * Used for Redis Cluster redirect (may be NULL) */
    // redisGetKeysProc *getkeys_proc;
    /* Array of subcommands (may be NULL) */
    struct redisCommand *subcommands;
    /* Array of arguments (may be NULL) */
    struct redisCommandArg *args;

    /* Runtime populated data */
    long long microseconds, calls, rejected_calls, failed_calls;
    int id;     /* Command ID. This is a progressive ID starting from 0 that
                   is assigned at runtime, and is used in order to check
                   ACLs. A connection is able to execute a given command if
                   the user associated to the connection has this command
                   bit set in the bitmap of allowed commands. */
    sds fullname; /* A SDS string representing the command fullname. */
    struct hdr_histogram* latency_histogram; /*points to the command latency command histogram (unit of time nanosecond) */
    // keySpec *key_specs;
    // keySpec legacy_range_key_spec; /* The legacy (first,last,step) key spec is
    //                                  * still maintained (if applicable) so that
    //                                  * we can still support the reply format of
    //                                  * COMMAND INFO and COMMAND GETKEYS */
    int num_args;
    int num_history;
    int num_tips;
    int key_specs_num;
    int key_specs_max;
    dict *subcommands_dict; /* A dictionary that holds the subcommands, the key is the subcommand sds name
                            //  * (not the fullname), and the value is the redisCommand structure pointer. */
    struct redisCommand *parent;
    struct RedisModuleCommand *module_cmd; /* A pointer to the module command data (NULL if native command) */
};

/* Client MULTI/EXEC state */
typedef struct multiCmd {
    robj **argv;
    int argv_len;
    int argc;
    struct redisCommand *cmd;
} multiCmd;

typedef struct multiState {
    multiCmd *commands;     /* Array of MULTI commands */
    int count;              /* Total number of MULTI commands */
    int cmd_flags;          /* The accumulated command flags OR-ed together.
                               So if at least a command has a given flag, it
                               will be set in this field. */
    int cmd_inv_flags;      /* Same as cmd_flags, OR-ing the ~flags. so that it
                               is possible to know if all the commands have a
                               certain flag. */
    size_t argv_len_sums;    /* mem used by all commands arguments */
    int alloc_count;         /* total number of multiCmd struct memory reserved. */
} multiState;

typedef struct client {
    uint64_t id;            /* Client incremental unique ID. */
    uint64_t flags;         /* Client flags: CLIENT_* macros. */
    connection *conn;
    int resp;               /* RESP protocol version. Can be 2 or 3. */
    redisDb *db;            /* Pointer to currently SELECTed DB. */
    robj *name;             /* As set by CLIENT SETNAME. */
    sds querybuf;           /* Buffer we use to accumulate client queries. */
    size_t qb_pos;          /* The position we have read in querybuf. */
    size_t querybuf_peak;   /* Recent (100ms or more) peak of querybuf size. */
    int argc;               /* Num of arguments of current command. */
    robj **argv;            /* Arguments of current command. */
    int argv_len;           /* Size of argv array (may be more than argc) */
    int original_argc;      /* Num of arguments of original command if arguments were rewritten. */
    robj **original_argv;   /* Arguments of original command if arguments were rewritten. */
    size_t argv_len_sum;    /* Sum of lengths of objects in argv list. */
    struct redisCommand *cmd, *lastcmd;  /* Last command executed. */
    struct redisCommand *realcmd; /* The original command that was executed by the client,
    //                                  Used to update error stats in case the c->cmd was modified
    //                                  during the command invocation (like on GEOADD for example). */
    user *user;             /* User associated with this connection. If the
    //                            user is set to NULL the connection can do
    //                            anything (admin). */
    int reqtype;            /* Request protocol type: PROTO_REQ_* */
    int multibulklen;       /* Number of multi bulk arguments left to read. */
    long bulklen;           /* Length of bulk argument in multi bulk request. */
    list *reply;            /* List of reply objects to send to the client. */
    unsigned long long reply_bytes; /* Tot bytes of objects in reply list. */
    list *deferred_reply_errors;    /* Used for module thread safe contexts. */
    size_t sentlen;         /* Amount of bytes already sent in the current
    //                            buffer or object being sent. */
    time_t ctime;           /* Client creation time. */
    long duration;          /* Current command duration. Used for measuring latency of blocking/non-blocking cmds */
    int slot;               /* The slot the client is executing against. Set to -1 if no slot is being used */
    time_t lastinteraction; /* Time of the last interaction, used for timeout */
    time_t obuf_soft_limit_reached_time;
    // int authenticated;      /* Needed when the default user requires auth. */
    int replstate;          /* Replication state if this is a slave. */
    int repl_start_cmd_stream_on_ack; /* Install slave write handler on first ACK. */
    int repldbfd;           /* Replication DB file descriptor. */
    // off_t repldboff;        /* Replication DB file offset. */
    // off_t repldbsize;       /* Replication DB file size. */
    // sds replpreamble;       /* Replication DB preamble. */
    long long read_reploff; /* Read replication offset if this is a master. */
    long long reploff;      /* Applied replication offset if this is a master. */
    long long repl_applied; /* Applied replication data count in querybuf, if this is a replica. */
    long long repl_ack_off; /* Replication ack offset, if this is a slave. */
    long long repl_ack_time;/* Replication ack time, if this is a slave. */
    long long repl_last_partial_write; /* The last time the server did a partial write from the RDB child pipe to this replica  */
    long long psync_initial_offset; /* FULLRESYNC reply offset other slaves
    //                                    copying this slave output buffer
    //                                    should use. */
    char replid[CONFIG_RUN_ID_SIZE+1]; /* Master replication ID (if master). */
    int slave_listening_port; /* As configured with: REPLCONF listening-port */
    char *slave_addr;       /* Optionally given by REPLCONF ip-address */
    int slave_capa;         /* Slave capabilities: SLAVE_CAPA_* bitwise OR. */
    int slave_req;          /* Slave requirements: SLAVE_REQ_* */
    multiState mstate;      /* MULTI/EXEC state */
    int btype;              /* Type of blocking op if CLIENT_BLOCKED. */
    blockingState bpop;     /* blocking state */
    long long woff;         /* Last write global replication offset. */
    list *watched_keys;     /* Keys WATCHED for MULTI/EXEC CAS */
    dict *pubsub_channels;  /* channels a client is interested in (SUBSCRIBE) */
    list *pubsub_patterns;  /* patterns a client is interested in (SUBSCRIBE) */
    dict *pubsubshard_channels;  /* shard level channels a client is interested in (SSUBSCRIBE) */
    sds peerid;             /* Cached peer ID. */
    sds sockname;           /* Cached connection target address. */
    listNode *client_list_node; /* list node in client list */
    listNode *postponed_list_node; /* list node within the postponed list */
    listNode *pending_read_list_node; /* list node in clients pending read list */
    RedisModuleUserChangedFunc auth_callback; /* Module callback to execute
    //                                            * when the authenticated user
    //                                            * changes. */
    void *auth_callback_privdata; /* Private data that is passed when the auth
    //                                * changed callback is executed. Opaque for
    //                                * Redis Core. */
    void *auth_module;      /* The module that owns the callback, which is used
    //                          * to disconnect the client if the module is
    //                          * unloaded for cleanup. Opaque for Redis Core.*/

    // /* If this client is in tracking mode and this field is non zero,
    //  * invalidation messages for keys fetched by this client will be send to
    //  * the specified client ID. */
    uint64_t client_tracking_redirection;
    // rax *client_tracking_prefixes; /* A dictionary of prefixes we are already
    //                                   subscribed to in BCAST mode, in the
    //                                   context of client side caching. */
    // /* In updateClientMemUsage() we track the memory usage of
    //  * each client and add it to the sum of all the clients of a given type,
    //  * however we need to remember what was the old contribution of each
    //  * client, and in which category the client was, in order to remove it
    //  * before adding it the new value. */
    size_t last_memory_usage;
    int last_memory_type;

    listNode *mem_usage_bucket_node;
    clientMemUsageBucket *mem_usage_bucket;

    listNode *ref_repl_buf_node; /* Referenced node of replication buffer blocks,
    //                               * see the definition of replBufBlock. */
    size_t ref_block_pos;        /* Access position of referenced buffer block,
    //                               * i.e. the next offset to send. */

    // /* list node in clients_pending_write list */
    listNode clients_pending_write_node;
    // /* Response buffer */
    size_t buf_peak; /* Peak used size of buffer in last 5 sec interval. */
    mstime_t buf_peak_last_reset_time; /* keeps the last time the buffer peak value was reset */
    int bufpos;
    size_t buf_usable_size; /* Usable size of buffer. */
    char *buf;
} client;


struct redisServer {
    /* General */
    pid_t pid;                  /* Main process pid. */
    pthread_t main_thread_id;         /* Main thread id */
    char *configfile;           /* Absolute config file path, or NULL */
    char *executable;           /* Absolute executable file path. */
    char **exec_argv;           /* Executable argv vector (copy). */
    int dynamic_hz;             /* Change hz value depending on #457 of clients. */
    int config_hz;              /* Configured HZ value. May be different than
                                   the actual 'hz' field value if dynamic-hz
                                   is enabled. */
    mode_t umask;               /* The umask value of the process on startup */
    int hz;                     /* serverCron() calls frequency in hertz */
    int in_fork_child;          /* indication that this is a fork child */
    redisDb *db;
    dict *commands;             /* Command table */
    // dict *orig_commands;        /* Command table before command renaming. */
    aeEventLoop *el;
    // rax *errors;                /* Errors table */
    // redisAtomic unsigned int lruclock; /* Clock for LRU eviction */
    volatile sig_atomic_t shutdown_asap; /* Shutdown ordered by signal handler. */
    mstime_t shutdown_mstime;   /* Timestamp to limit graceful shutdown. */
    int last_sig_received;      /* Indicates the last SIGNAL received, if any (e.g., SIGINT or SIGTERM). */
    int shutdown_flags;         /* Flags passed to prepareForShutdown(). */
    int activerehashing;        /* Incremental rehash in serverCron() */
    int active_defrag_running;  /* Active defragmentation running (holds current scan aggressiveness) */
    char *pidfile;              /* PID file path */
    int arch_bits;              /* 32 or 64 depending on sizeof(long) */
    int cronloops;              /* Number of times the cron function run */
    char runid[CONFIG_RUN_ID_SIZE+1];  /* ID always different at every exec. */
    int sentinel_mode;          /* True if this instance is a Sentinel. */
    size_t initial_memory_usage; /* Bytes used after initialization. */
    // int always_show_logo;       /* Show logo even for non-stdout logging. */
    // int in_exec;                /* Are we inside EXEC? */
    // int busy_module_yield_flags;         /* Are we inside a busy module? (triggered by RM_Yield). see BUSY_MODULE_YIELD_ flags. */
    // const char *busy_module_yield_reply; /* When non-null, we are inside RM_Yield. */
    int core_propagates;        /* Is the core (in oppose to the module subsystem) is in charge of calling propagatePendingCommands? */
    int module_ctx_nesting;     /* moduleCreateContext() nesting level */
    char *ignore_warnings;      /* Config: warnings that should be ignored. */
    int client_pause_in_transaction; /* Was a client pause executed during this Exec? */
    int thp_enabled;                 /* If true, THP is enabled. */
    size_t page_size;                /* The page size of OS. */
    // /* Modules */
    dict *moduleapi;            /* Exported core APIs dictionary for modules. */
    dict *sharedapi;            /* Like moduleapi but containing the APIs that
    //                                modules share with each other. */
    dict *module_configs_queue; /* Dict that stores module configurations from .conf file until after modules are loaded during startup or arguments to loadex. */
    // list *loadmodule_queue;     /* List of modules to load at startup. */
    int module_pipe[2];         /* Pipe used to awake the event loop by module threads. */
    pid_t child_pid;            /* PID of current child */
    int child_type;             /* Type of current child */
    // /* Networking */
    int port;                   /* TCP listening port */
    int tls_port;               /* TLS listening port */
    int tcp_backlog;            /* TCP listen() backlog */
    char *bindaddr[CONFIG_BINDADDR_MAX]; /* Addresses we should bind to */
    int bindaddr_count;         /* Number of addresses in server.bindaddr[] */
    char *bind_source_addr;     /* Source address to bind on for outgoing connections */
    char *unixsocket;           /* UNIX socket path */
    unsigned int unixsocketperm; /* UNIX socket permission (see mode_t) */
    connListener listeners[CONN_TYPE_MAX]; /* TCP/Unix/TLS even more types */
    uint32_t socket_mark_id;    /* ID for listen socket marking */
    connListener clistener;     /* Cluster bus listener */
    list *clients;              /* List of active clients */
    list *clients_to_close;     /* Clients to close asynchronously */
    list *clients_pending_write; /* There is to write or install handler. */
    list *clients_pending_read;  /* Client has pending read socket buffers. */
    list *slaves, *monitors;    /* List of slaves and MONITORs */
    client *current_client;     /* Current client executing the command. */

    // /* Stuff for client mem eviction */
    // clientMemUsageBucket client_mem_usage_buckets[CLIENT_MEM_USAGE_BUCKETS];

    // rax *clients_timeout_table; /* Radix tree for blocked clients timeouts. */
    int in_nested_call;         /* If > 0, in a nested call of a call */
    // rax *clients_index;         /* Active clients dictionary by client ID. */
    uint32_t paused_actions;   /* Bitmask of actions that are currently paused */
    // list *postponed_clients;       /* List of postponed clients */
    // pause_event client_pause_per_purpose[NUM_PAUSE_PURPOSES];
    char neterr[ANET_ERR_LEN];   /* Error buffer for anet.c */
    // dict *migrate_cached_sockets;/* MIGRATE cached sockets */
    redisAtomic uint64_t next_client_id; /* Next client unique ID. Incremental. */
    int protected_mode;         /* Don't accept external connections. */
    int io_threads_num;         /* Number of IO threads to use. */
    int io_threads_do_reads;    /* Read and parse from IO threads? */
    int io_threads_active;      /* Is IO threads currently active? */
    long long events_processed_while_blocked; /* processEventsWhileBlocked() */
    int enable_protected_configs;    /* Enable the modification of protected configs, see PROTECTED_ACTION_ALLOWED_* */
    int enable_debug_cmd;            /* Enable DEBUG commands, see PROTECTED_ACTION_ALLOWED_* */
    int enable_module_cmd;           /* Enable MODULE commands, see PROTECTED_ACTION_ALLOWED_* */

    // /* RDB / AOF loading information */
    volatile sig_atomic_t loading; /* We are loading data from disk if true */
    // volatile sig_atomic_t async_loading; /* We are loading data without blocking the db being served */
    // off_t loading_total_bytes;
    // off_t loading_rdb_used_mem;
    // off_t loading_loaded_bytes;
    // time_t loading_start_time;
    // off_t loading_process_events_interval_bytes;
    // /* Fields used only for stats */
    // time_t stat_starttime;          /* Server start time */
    // long long stat_numcommands;     /* Number of processed commands */
    long long stat_numconnections;  /* Number of connections received */
    // long long stat_expiredkeys;     /* Number of expired keys */
    // double stat_expired_stale_perc; /* Percentage of keys probably expired */
    // long long stat_expired_time_cap_reached_count; /* Early expire cycle stops.*/
    // long long stat_expire_cycle_time_used; /* Cumulative microseconds used. */
    // long long stat_evictedkeys;     /* Number of evicted keys (maxmemory) */
    // long long stat_evictedclients;  /* Number of evicted clients */
    // long long stat_total_eviction_exceeded_time;  /* Total time over the memory limit, unit us */
    // monotime stat_last_eviction_exceeded_time;  /* Timestamp of current eviction start, unit us */
    // long long stat_keyspace_hits;   /* Number of successful lookups of keys */
    // long long stat_keyspace_misses; /* Number of failed lookups of keys */
    // long long stat_active_defrag_hits;      /* number of allocations moved */
    // long long stat_active_defrag_misses;    /* number of allocations scanned but not moved */
    // long long stat_active_defrag_key_hits;  /* number of keys with moved allocations */
    // long long stat_active_defrag_key_misses;/* number of keys scanned and not moved */
    // long long stat_active_defrag_scanned;   /* number of dictEntries scanned */
    // long long stat_total_active_defrag_time; /* Total time memory fragmentation over the limit, unit us */
    // monotime stat_last_active_defrag_time; /* Timestamp of current active defrag start */
    size_t stat_peak_memory;        /* Max used memory record */
    // long long stat_aof_rewrites;    /* number of aof file rewrites performed */
    // long long stat_aofrw_consecutive_failures; /* The number of consecutive failures of aofrw */
    // long long stat_rdb_saves;       /* number of rdb saves performed */
    // long long stat_fork_time;       /* Time needed to perform latest fork() */
    // double stat_fork_rate;          /* Fork rate in GB/sec. */
    // long long stat_total_forks;     /* Total count of fork. */
    long long stat_rejected_conn;   /* Clients rejected because of maxclients */
    // long long stat_sync_full;       /* Number of full resyncs with slaves. */
    // long long stat_sync_partial_ok; /* Number of accepted PSYNC requests. */
    // long long stat_sync_partial_err;/* Number of unaccepted PSYNC requests. */
    // list *slowlog;                  /* SLOWLOG list of commands */
    // long long slowlog_entry_id;     /* SLOWLOG current entry ID */
    // long long slowlog_log_slower_than; /* SLOWLOG time limit (to get logged) */
    // unsigned long slowlog_max_len;     /* SLOWLOG max number of items logged */
    // struct malloc_stats cron_malloc_stats; /* sampled in serverCron(). */
    redisAtomic long long stat_net_input_bytes; /* Bytes read from network. */
    // redisAtomic long long stat_net_output_bytes; /* Bytes written to network. */
    redisAtomic long long stat_net_repl_input_bytes; /* Bytes read during replication, added to stat_net_input_bytes in 'info'. */
    // redisAtomic long long stat_net_repl_output_bytes; /* Bytes written during replication, added to stat_net_output_bytes in 'info'. */
    // size_t stat_current_cow_peak;   /* Peak size of copy on write bytes. */
    // size_t stat_current_cow_bytes;  /* Copy on write bytes while child is active. */
    // monotime stat_current_cow_updated;  /* Last update time of stat_current_cow_bytes */
    // size_t stat_current_save_keys_processed;  /* Processed keys while child is active. */
    // size_t stat_current_save_keys_total;  /* Number of keys when child started. */
    // size_t stat_rdb_cow_bytes;      /* Copy on write bytes during RDB saving. */
    // size_t stat_aof_cow_bytes;      /* Copy on write bytes during AOF rewrite. */
    // size_t stat_module_cow_bytes;   /* Copy on write bytes during module fork. */
    // double stat_module_progress;   /* Module save progress. */
    size_t stat_clients_type_memory[CLIENT_TYPE_COUNT];/* Mem usage by type */
    size_t stat_cluster_links_memory; /* Mem usage by cluster links */
    long long stat_unexpected_error_replies; /* Number of unexpected (aof-loading, replica to master, etc.) error replies */
    long long stat_total_error_replies; /* Total number of issued error replies ( command + rejected errors ) */
    long long stat_dump_payload_sanitizations; /* Number deep dump payloads integrity validations. */
    long long stat_io_reads_processed; /* Number of read events processed by IO / Main threads */
    long long stat_io_writes_processed; /* Number of write events processed by IO / Main threads */
    redisAtomic long long stat_total_reads_processed; /* Total number of read events processed */
    redisAtomic long long stat_total_writes_processed; /* Total number of write events processed */
    // /* The following two are used to track instantaneous metrics, like
    //  * number of operations per second, network traffic. */
    // struct {
    //     long long last_sample_time; /* Timestamp of last sample in ms */
    //     long long last_sample_count;/* Count in last sample */
    //     long long samples[STATS_METRIC_SAMPLES];
    //     int idx;
    // } inst_metric[STATS_METRIC_COUNT];
    // long long stat_reply_buffer_shrinks; /* Total number of output buffer shrinks */
    // long long stat_reply_buffer_expands; /* Total number of output buffer expands */

    // /* Configuration */
    int verbosity;                  /* Loglevel in redis.conf */
    // int maxidletime;                /* Client timeout in seconds */
    int tcpkeepalive;               /* Set SO_KEEPALIVE if non-zero. */
    // int active_expire_enabled;      /* Can be disabled for testing purposes. */
    // int active_expire_effort;       /* From 1 (default) to 10, active effort. */
    // int active_defrag_enabled;
    // int sanitize_dump_payload;      /* Enables deep sanitization for ziplist and listpack in RDB and RESTORE. */
    int skip_checksum_validation;   /* Disable checksum validation for RDB and RESTORE payload. */
    // int jemalloc_bg_thread;         /* Enable jemalloc background thread */
    // size_t active_defrag_ignore_bytes; /* minimum amount of fragmentation waste to start active defrag */
    // int active_defrag_threshold_lower; /* minimum percentage of fragmentation to start active defrag */
    // int active_defrag_threshold_upper; /* maximum percentage of fragmentation at which we use maximum effort */
    // int active_defrag_cycle_min;       /* minimal effort for defrag in CPU percentage */
    // int active_defrag_cycle_max;       /* maximal effort for defrag in CPU percentage */
    // unsigned long active_defrag_max_scan_fields; /* maximum number of fields of set/hash/zset/list to process from within the main dict scan */
    size_t client_max_querybuf_len; /* Limit for client query buffer length */
    int dbnum;                      /* Total number of configured DBs */
    int supervised;                 /* 1 if supervised, 0 otherwise. */
    int supervised_mode;            /* See SUPERVISED_* */
    int daemonize;                  /* True if running as a daemon */
    int set_proc_title;             /* True if change proc title */
    char *proc_title_template;      /* Process title template format */
    clientBufferLimitsConfig client_obuf_limits[CLIENT_TYPE_OBUF_COUNT];
    int pause_cron;                 /* Don't run cron tasks (debug) */
    int latency_tracking_enabled;   /* 1 if extended latency tracking is enabled, 0 otherwise. */
    double *latency_tracking_info_percentiles; /* Extended latency tracking info output percentile list configuration. */
    int latency_tracking_info_percentiles_len;
    /* AOF persistence */
    // int aof_enabled;                /* AOF configuration */
    // int aof_state;                  /* AOF_(ON|OFF|WAIT_REWRITE) */
    // int aof_fsync;                  /* Kind of fsync() policy */
    // char *aof_filename;             /* Basename of the AOF file and manifest file */
    // char *aof_dirname;              /* Name of the AOF directory */
    // int aof_no_fsync_on_rewrite;    /* Don't fsync if a rewrite is in prog. */
    // int aof_rewrite_perc;           /* Rewrite AOF if % growth is > M and... */
    // off_t aof_rewrite_min_size;     /* the AOF file is at least N bytes. */
    // off_t aof_rewrite_base_size;    /* AOF size on latest startup or rewrite. */
    // off_t aof_current_size;         /* AOF current size (Including BASE + INCRs). */
    // off_t aof_last_incr_size;       /* The size of the latest incr AOF. */
    // off_t aof_fsync_offset;         /* AOF offset which is already synced to disk. */
    // int aof_flush_sleep;            /* Micros to sleep before flush. (used by tests) */
    // int aof_rewrite_scheduled;      /* Rewrite once BGSAVE terminates. */
    // sds aof_buf;      /* AOF buffer, written before entering the event loop */
    // int aof_fd;       /* File descriptor of currently selected AOF file */
    // int aof_selected_db; /* Currently selected DB in AOF */
    // time_t aof_flush_postponed_start; /* UNIX time of postponed AOF flush */
    // time_t aof_last_fsync;            /* UNIX time of last fsync() */
    // time_t aof_rewrite_time_last;   /* Time used by last AOF rewrite run. */
    // time_t aof_rewrite_time_start;  /* Current AOF rewrite start time. */
    // time_t aof_cur_timestamp;       /* Current record timestamp in AOF */
    // int aof_timestamp_enabled;      /* Enable record timestamp in AOF */
    // int aof_lastbgrewrite_status;   /* C_OK or C_ERR */
    // unsigned long aof_delayed_fsync;  /* delayed AOF fsync() counter */
    // int aof_rewrite_incremental_fsync;/* fsync incrementally while aof rewriting? */
    // int rdb_save_incremental_fsync;   /* fsync incrementally while rdb saving? */
    // int aof_last_write_status;      /* C_OK or C_ERR */
    // int aof_last_write_errno;       /* Valid if aof write/fsync status is ERR */
    // int aof_load_truncated;         /* Don't stop on unexpected AOF EOF. */
    // int aof_use_rdb_preamble;       /* Specify base AOF to use RDB encoding on AOF rewrites. */
    // redisAtomic int aof_bio_fsync_status; /* Status of AOF fsync in bio job. */
    // redisAtomic int aof_bio_fsync_errno;  /* Errno of AOF fsync in bio job. */
    // aofManifest *aof_manifest;       /* Used to track AOFs. */
    // int aof_disable_auto_gc;         /* If disable automatically deleting HISTORY type AOFs?
    //                                     default no. (for testings). */

    // /* RDB persistence */
    long long dirty;                /* Changes to DB from the last save */
    // long long dirty_before_bgsave;  /* Used to restore dirty on failed BGSAVE */
    // long long rdb_last_load_keys_expired;  /* number of expired keys when loading RDB */
    // long long rdb_last_load_keys_loaded;   /* number of loaded keys when loading RDB */
    struct saveparam *saveparams;   /* Save points array for RDB */
    // int saveparamslen;              /* Number of saving points */
    // char *rdb_filename;             /* Name of RDB file */
    // int rdb_compression;            /* Use compression in RDB? */
    int rdb_checksum;               /* Use RDB checksum? */
    // int rdb_del_sync_files;         /* Remove RDB files used only for SYNC if
    //                                    the instance does not use persistence. */
    // time_t lastsave;                /* Unix time of last successful save */
    // time_t lastbgsave_try;          /* Unix time of last attempted bgsave */
    // time_t rdb_save_time_last;      /* Time used by last RDB save run. */
    // time_t rdb_save_time_start;     /* Current RDB save start time. */
    // int rdb_bgsave_scheduled;       /* BGSAVE when possible if true. */
    // int rdb_child_type;             /* Type of save by active child. */
    // int lastbgsave_status;          /* C_OK or C_ERR */
    // int stop_writes_on_bgsave_err;  /* Don't allow writes if can't BGSAVE */
    // int rdb_pipe_read;              /* RDB pipe used to transfer the rdb data */
    //                                 /* to the parent process in diskless repl. */
    // int rdb_child_exit_pipe;        /* Used by the diskless parent allow child exit. */
    // connection **rdb_pipe_conns;    /* Connections which are currently the */
    // int rdb_pipe_numconns;          /* target of diskless rdb fork child. */
    // int rdb_pipe_numconns_writing;  /* Number of rdb conns with pending writes. */
    // char *rdb_pipe_buff;            /* In diskless replication, this buffer holds data */
    // int rdb_pipe_bufflen;           /* that was read from the rdb pipe. */
    // int rdb_key_save_delay;         /* Delay in microseconds between keys while
    //                                  * writing the RDB. (for testings). negative
    //                                  * value means fractions of microseconds (on average). */
    // int key_load_delay;             /* Delay in microseconds between keys while
    //                                  * loading aof or rdb. (for testings). negative
    //                                  * value means fractions of microseconds (on average). */
    // /* Pipe and data structures for child -> parent info sharing. */
    // int child_info_pipe[2];         /* Pipe used to write the child_info_data. */
    // int child_info_nread;           /* Num of bytes of the last read from pipe */
    // /* Propagation of commands in AOF / replication */
    // redisOpArray also_propagate;    /* Additional command to propagate. */
    int replication_allowed;        /* Are we allowed to replicate? */
    // /* Logging */
    char *logfile;                  /* Path of log file */
    int syslog_enabled;             /* Is syslog enabled? */
    char *syslog_ident;             /* Syslog ident */
    int syslog_facility;            /* Syslog facility */
    int crashlog_enabled;           /* Enable signal handler for crashlog.
    //                                  * disable for clean core dumps. */
    // int memcheck_enabled;           /* Enable memory check on crash. */
    // int use_exit_on_panic;          /* Use exit() on panic and assert rather than
    //                                  * abort(). useful for Valgrind. */
    // /* Shutdown */
    int shutdown_timeout;           /* Graceful shutdown time limit in seconds. */
    int shutdown_on_sigint;         /* Shutdown flags configured for SIGINT. */
    int shutdown_on_sigterm;        /* Shutdown flags configured for SIGTERM. */

    // /* Replication (master) */
    // char replid[CONFIG_RUN_ID_SIZE+1];  /* My current replication ID. */
    // char replid2[CONFIG_RUN_ID_SIZE+1]; /* replid inherited from master*/
    long long master_repl_offset;   /* My current replication offset */
    // long long second_replid_offset; /* Accept offsets up to this for replid2. */
    // int slaveseldb;                 /* Last SELECTed DB in replication output */
    int repl_ping_slave_period;     /* Master pings the slave every N seconds */
    replBacklog *repl_backlog;      /* Replication backlog for partial syncs */
    long long repl_backlog_size;    /* Backlog circular buffer size */
    // time_t repl_backlog_time_limit; /* Time without slaves after the backlog
    //                                    gets released. */
    // time_t repl_no_slaves_since;    /* We have no slaves since that time.
    //                                    Only valid if server.slaves len is 0. */
    // int repl_min_slaves_to_write;   /* Min number of slaves to write. */
    // int repl_min_slaves_max_lag;    /* Max lag of <count> slaves to write. */
    // int repl_good_slaves_count;     /* Number of slaves with lag <= max_lag. */
    // int repl_diskless_sync;         /* Master send RDB to slaves sockets directly. */
    // int repl_diskless_load;         /* Slave parse RDB directly from the socket.
    //                                  * see REPL_DISKLESS_LOAD_* enum */
    // int repl_diskless_sync_delay;   /* Delay to start a diskless repl BGSAVE. */
    // int repl_diskless_sync_max_replicas;/* Max replicas for diskless repl BGSAVE
    //                                      * delay (start sooner if they all connect). */
    // size_t repl_buffer_mem;         /* The memory of replication buffer. */
    list *repl_buffer_blocks;       /* Replication buffers blocks list
    //                                  * (serving replica clients and repl backlog) */
    // /* Replication (slave) */
    // char *masteruser;               /* AUTH with this user and masterauth with master */
    // sds masterauth;                 /* AUTH with this password with master */
    char *masterhost;               /* Hostname of master */
    // int masterport;                 /* Port of master */
    // int repl_timeout;               /* Timeout after N seconds of master idle */
    client *master;     /* Client that is master for this slave */
    // client *cached_master; /* Cached master to be reused for PSYNC. */
    // int repl_syncio_timeout; /* Timeout for synchronous I/O calls */
    int repl_state;          /* Replication status if the instance is a slave */
    // off_t repl_transfer_size; /* Size of RDB to read from master during sync. */
    // off_t repl_transfer_read; /* Amount of RDB read from master during sync. */
    // off_t repl_transfer_last_fsync_off; /* Offset when we fsync-ed last time. */
    // connection *repl_transfer_s;     /* Slave -> Master SYNC connection */
    // int repl_transfer_fd;    /* Slave -> Master SYNC temp file descriptor */
    // char *repl_transfer_tmpfile; /* Slave-> master SYNC temp file name */
    // time_t repl_transfer_lastio; /* Unix time of the latest read, for timeout */
    // int repl_serve_stale_data; /* Serve stale data when link is down? */
    int repl_slave_ro;          /* Slave is read only? */
    // int repl_slave_ignore_maxmemory;    /* If true slaves do not evict. */
    time_t repl_down_since; /* Unix time at which link with master went down */
    // int repl_disable_tcp_nodelay;   /* Disable TCP_NODELAY after SYNC? */
    // int slave_priority;             /* Reported in INFO and used by Sentinel. */
    // int replica_announced;          /* If true, replica is announced by Sentinel */
    // int slave_announce_port;        /* Give the master this listening port. */
    // char *slave_announce_ip;        /* Give the master this ip address. */
    int propagation_error_behavior; /* Configures the behavior of the replica
    //                                  * when it receives an error on the replication stream */
    // int repl_ignore_disk_write_error;   /* Configures whether replicas panic when unable to
    //                                      * persist writes to AOF. */
    // /* The following two fields is where we store master PSYNC replid/offset
    //  * while the PSYNC is in progress. At the end we'll copy the fields into
    //  * the server->master client structure. */
    // char master_replid[CONFIG_RUN_ID_SIZE+1];  /* Master PSYNC runid. */
    // long long master_initial_offset;           /* Master PSYNC offset. */
    // int repl_slave_lazy_flush;          /* Lazy FLUSHALL before loading DB? */
    // /* Synchronous replication. */
    // list *clients_waiting_acks;         /* Clients waiting in WAIT command. */
    // int get_ack_from_slaves;            /* If true we send REPLCONF GETACK. */
    // /* Limits */
    unsigned int maxclients;            /* Max number of simultaneous clients */
    unsigned long long maxmemory;   /* Max number of memory bytes to use */
    // ssize_t maxmemory_clients;       /* Memory limit for total client buffers */
    int maxmemory_policy;           /* Policy for key eviction */
    // int maxmemory_samples;          /* Precision of random sampling */
    // int maxmemory_eviction_tenacity;/* Aggressiveness of eviction processing */
    // int lfu_log_factor;             /* LFU logarithmic counter factor. */
    // int lfu_decay_time;             /* LFU counter decay factor. */
    // long long proto_max_bulk_len;   /* Protocol bulk length maximum size. */
    // int oom_score_adj_values[CONFIG_OOM_COUNT];   /* Linux oom_score_adj configuration */
    // int oom_score_adj;                            /* If true, oom_score_adj is managed */
    // int disable_thp;                              /* If true, disable THP by syscall */
    // /* Blocked clients */
    // unsigned int blocked_clients;   /* # of clients executing a blocking cmd.*/
    // unsigned int blocked_clients_by_type[BLOCKED_NUM];
    list *unblocked_clients; /* list of clients to unblock before next loop */
    // list *ready_keys;        /* List of readyList structures for BLPOP & co */
    // /* Client side caching. */
    // unsigned int tracking_clients;  /* # of clients with tracking enabled.*/
    // size_t tracking_table_max_keys; /* Max number of keys in tracking table. */
    // list *tracking_pending_keys; /* tracking invalidation keys pending to flush */
    // /* Sort parameters - qsort_r() is only available under BSD so we
    //  * have to take this state global, in order to pass it to sortCompare() */
    // int sort_desc;
    // int sort_alpha;
    // int sort_bypattern;
    // int sort_store;
    // /* Zip structure config, see redis.conf for more information  */
    // size_t hash_max_listpack_entries;
    // size_t hash_max_listpack_value;
    // size_t set_max_intset_entries;
    // size_t set_max_listpack_entries;
    // size_t set_max_listpack_value;
    // size_t zset_max_listpack_entries;
    // size_t zset_max_listpack_value;
    // size_t hll_sparse_max_bytes;
    // size_t stream_node_max_bytes;
    // long long stream_node_max_entries;
    // /* List parameters */
    // int list_max_listpack_size;
    // int list_compress_depth;
    // /* time cache */
    redisAtomic time_t unixtime; /* Unix time sampled every cron cycle. */
    time_t timezone;            /* Cached timezone. As set by tzset(). */
    int daylight_active;        /* Currently in daylight saving time. */
    mstime_t mstime;            /* 'unixtime' in milliseconds. */
    // ustime_t ustime;            /* 'unixtime' in microseconds. */
    size_t blocking_op_nesting; /* Nesting level of blocking operation, used to reset blocked_last_cron. */
    long long blocked_last_cron; /* Indicate the mstime of the last time we did cron jobs from a blocking operation */
    // /* Pubsub */
    // dict *pubsub_channels;  /* Map channels to list of subscribed clients */
    // dict *pubsub_patterns;  /* A dict of pubsub_patterns */
    // int notify_keyspace_events; /* Events to propagate via Pub/Sub. This is an
    //                                xor of NOTIFY_... flags. */
    // dict *pubsubshard_channels;  /* Map shard channels to list of subscribed clients */
    // /* Cluster */
    int cluster_enabled;      /* Is cluster enabled? */
    int cluster_port;         /* Set the cluster port for a node. */
    mstime_t cluster_node_timeout; /* Cluster node timeout. */
    mstime_t cluster_ping_interval;    /* A debug configuration for setting how often cluster nodes send ping messages. */
    char *cluster_configfile; /* Cluster auto-generated config file name. */
    struct clusterState *cluster;  /* State of the cluster */
    int cluster_migration_barrier; /* Cluster replicas migration barrier. */
    int cluster_allow_replica_migration; /* Automatic replica migrations to orphaned masters and from empty masters */
    int cluster_slave_validity_factor; /* Slave max data age for failover. */
    int cluster_require_full_coverage; /* If true, put the cluster down if
    //                                       there is at least an uncovered slot.*/
    int cluster_slave_no_failover;  /* Prevent slave from starting a failover
    //                                    if the master is in failure state. */
    char *cluster_announce_ip;  /* IP address to announce on cluster bus. */
    char *cluster_announce_hostname;  /* hostname to announce on cluster bus. */
    int cluster_preferred_endpoint_type; /* Use the announced hostname when available. */
    int cluster_announce_port;     /* base port to announce on cluster bus. */
    int cluster_announce_tls_port; /* TLS port to announce on cluster bus. */
    int cluster_announce_bus_port; /* bus port to announce on cluster bus. */
    int cluster_module_flags;      /* Set of flags that Redis modules are able
    //                                   to set in order to suppress certain
    //                                   native Redis Cluster features. Check the
    //                                   REDISMODULE_CLUSTER_FLAG_*. */
    // int cluster_allow_reads_when_down; /* Are reads allowed when the cluster
    //                                     is down? */
    int cluster_config_file_lock_fd;   /* cluster config fd, will be flock */
    unsigned long long cluster_link_msg_queue_limit_bytes;  /* Memory usage limit on individual link msg queue */
    int cluster_drop_packet_filter; /* Debug config that allows tactically
    //                                * dropping packets of a specific type */
    // /* Scripting */
    // client *script_caller;       /* The client running script right now, or NULL */
    // mstime_t busy_reply_threshold;  /* Script / module timeout in milliseconds */
    // int pre_command_oom_state;         /* OOM before command (script?) was started */
    // int script_disable_deny_script;    /* Allow running commands marked "no-script" inside a script. */
    // /* Lazy free */
    // int lazyfree_lazy_eviction;
    // int lazyfree_lazy_expire;
    int lazyfree_lazy_server_del;
    // int lazyfree_lazy_user_del;
    // int lazyfree_lazy_user_flush;
    // /* Latency monitor */
    // long long latency_monitor_threshold;
    // dict *latency_events;
    // /* ACLs */
    // char *acl_filename;           /* ACL Users file. NULL if not configured. */
    // unsigned long acllog_max_len; /* Maximum length of the ACL LOG list. */
    // sds requirepass;              /* Remember the cleartext password set with
    //                                  the old "requirepass" directive for
    //                                  backward compatibility with Redis <= 5. */
    // int acl_pubsub_default;      /* Default ACL pub/sub channels flag */
    // aclInfo acl_info; /* ACL info */
    // /* Assert & bug reporting */
    int watchdog_period;  /* Software watchdog period in ms. 0 = off */
    // /* System hardware info */
    size_t system_memory_size;  /* Total memory in system as reported by OS */
    // /* TLS Configuration */
    int tls_cluster;
    // int tls_replication;
    // int tls_auth_clients;
    // redisTLSContextConfig tls_ctx_config;
    // /* cpu affinity */
    // char *server_cpulist; /* cpu affinity list of redis server main/io thread. */
    // char *bio_cpulist; /* cpu affinity list of bio thread. */
    // char *aof_rewrite_cpulist; /* cpu affinity list of aof rewrite process. */
    // char *bgsave_cpulist; /* cpu affinity list of bgsave process. */
    // /* Sentinel config */
    // struct sentinelConfig *sentinel_config; /* sentinel config to load at startup time. */
    // /* Coordinate failover info */
    // mstime_t failover_end_time; /* Deadline for failover command. */
    // int force_failover; /* If true then failover will be forced at the
    //                      * deadline, otherwise failover is aborted. */
    // char *target_replica_host; /* Failover target host. If null during a
    //                             * failover then any replica can be used. */
    // int target_replica_port; /* Failover target port */
    // int failover_state; /* Failover state */
    // int cluster_allow_pubsubshard_when_down; /* Is pubsubshard allowed when the cluster
    //                                             is down, doesn't affect pubsub global. */
    // long reply_buffer_peak_reset_time; /* The amount of time (in milliseconds) to wait between reply buffer peak resets */
    // int reply_buffer_resizing_enabled; /* Is reply buffer resizing enabled (1 by default) */
    // /* Local environment */
    char *locale_collate;
};

/* Global vars */
struct redisServer server; /* Server global state */


#ifdef __GNUC__
void _serverLog(int level, const char *fmt, ...)
    __attribute__((format(printf, 2, 3)));
#else
void _serverLog(int level, const char *fmt, ...);
#endif

/* Use macro for checking log level to avoid evaluating arguments in cases log
 * should be ignored due to low level. */
#define serverLog(level, ...) do {\
        if (((level)&0xff) < server.verbosity) break;\
        printf("serverLog");\
        _serverLog(level, __VA_ARGS__);\
    } while(0)

#define serverAssertWithInfo(_c,_o,_e) ((_e)?(void)0 : (_serverAssertWithInfo(_c,_o,#_e,__FILE__,__LINE__),redis_unreachable()))

/* Anti-warning macro... */
#define UNUSED(V) ((void) V)

/* Using the following macro you can run code inside serverCron() with the
 * specified period, specified in milliseconds.
 * The actual resolution depends on server.hz. */
#define run_with_period(_ms_) if (((_ms_) <= 1000/server.hz) || !(server.cronloops%((_ms_)/(1000/server.hz))))

#define sdsEncodedObject(objptr) (objptr->encoding == OBJ_ENCODING_RAW || objptr->encoding == OBJ_ENCODING_EMBSTR)

void getRandomBytes(unsigned char *p, size_t len);
 
/* The error code set by various library functions.  */
extern int *__errno_location (void) __THROW __attribute_const__;
# define errno (*__errno_location ())

/* Client pause purposes. Each purpose has its own end time and pause type. */
typedef enum {
    PAUSE_BY_CLIENT_COMMAND = 0,
    PAUSE_DURING_SHUTDOWN,
    PAUSE_DURING_FAILOVER,
    NUM_PAUSE_PURPOSES /* This value is the number of purposes above. */
} pause_purpose;


struct saveparam {
    time_t seconds;
    int changes;
};

/* afterErrorReply flags */
#define ERR_REPLY_FLAG_NO_STATS_UPDATE (1ULL<<0) /* Indicating that we should not update
                                                    error stats after sending error reply */

#define CLIENT_ID_AOF (UINT64_MAX) /* Reserved ID for the AOF client. If you
                                      need more reserved IDs use UINT64_MAX-1,
                                      -2, ... and so forth. */


/* Actions pause types */
#define PAUSE_ACTION_CLIENT_WRITE     (1<<0)
#define PAUSE_ACTION_CLIENT_ALL       (1<<1) /* must be bigger than PAUSE_ACTION_CLIENT_WRITE */
#define PAUSE_ACTION_EXPIRE           (1<<2)
#define PAUSE_ACTION_EVICT            (1<<3)
#define PAUSE_ACTION_REPLICA          (1<<4) /* pause replica traffic */
/* common sets of actions to pause/unpause */
#define PAUSE_ACTIONS_CLIENT_WRITE_SET (PAUSE_ACTION_CLIENT_WRITE|\
                                        PAUSE_ACTION_EXPIRE|\
                                        PAUSE_ACTION_EVICT|\
                                        PAUSE_ACTION_REPLICA)
#define PAUSE_ACTIONS_CLIENT_ALL_SET   (PAUSE_ACTION_CLIENT_ALL|\
                                        PAUSE_ACTION_EXPIRE|\
                                        PAUSE_ACTION_EVICT|\
                                        PAUSE_ACTION_REPLICA)

/* Slave replication state. Used in server.repl_state for slaves to remember
 * what to do next. */
typedef enum {
    REPL_STATE_NONE = 0,            /* No active replication */
    REPL_STATE_CONNECT,             /* Must connect to master */
    REPL_STATE_CONNECTING,          /* Connecting to master */
    /* --- Handshake states, must be ordered --- */
    REPL_STATE_RECEIVE_PING_REPLY,  /* Wait for PING reply */
    REPL_STATE_SEND_HANDSHAKE,      /* Send handshake sequence to master */
    REPL_STATE_RECEIVE_AUTH_REPLY,  /* Wait for AUTH reply */
    REPL_STATE_RECEIVE_PORT_REPLY,  /* Wait for REPLCONF reply */
    REPL_STATE_RECEIVE_IP_REPLY,    /* Wait for REPLCONF reply */
    REPL_STATE_RECEIVE_CAPA_REPLY,  /* Wait for REPLCONF reply */
    REPL_STATE_SEND_PSYNC,          /* Send PSYNC */
    REPL_STATE_RECEIVE_PSYNC_REPLY, /* Wait for PSYNC reply */
    /* --- End of handshake states --- */
    REPL_STATE_TRANSFER,        /* Receiving .rdb from master */
    REPL_STATE_CONNECTED,       /* Connected to master */
} repl_state;

/* Ways that a clusters endpoint can be described */
typedef enum {
    CLUSTER_ENDPOINT_TYPE_IP = 0,          /* Show IP address */
    CLUSTER_ENDPOINT_TYPE_HOSTNAME,        /* Show hostname */
    CLUSTER_ENDPOINT_TYPE_UNKNOWN_ENDPOINT /* Show NULL or empty */
} cluster_endpoint_type;





/* Keyspace changes notification classes. Every class is associated with a
 * character for configuration purposes. */
#define NOTIFY_KEYSPACE (1<<0)    /* K */
#define NOTIFY_KEYEVENT (1<<1)    /* E */
#define NOTIFY_GENERIC (1<<2)     /* g */
#define NOTIFY_STRING (1<<3)      /* $ */
#define NOTIFY_LIST (1<<4)        /* l */
#define NOTIFY_SET (1<<5)         /* s */
#define NOTIFY_HASH (1<<6)        /* h */
#define NOTIFY_ZSET (1<<7)        /* z */
#define NOTIFY_EXPIRED (1<<8)     /* x */
#define NOTIFY_EVICTED (1<<9)     /* e */
#define NOTIFY_STREAM (1<<10)     /* t */
#define NOTIFY_KEY_MISS (1<<11)   /* m (Note: This one is excluded from NOTIFY_ALL on purpose) */
#define NOTIFY_LOADED (1<<12)     /* module only key space notification, indicate a key loaded from rdb */
#define NOTIFY_MODULE (1<<13)     /* d, module key space notification */
#define NOTIFY_NEW (1<<14)        /* n, new key notification */
#define NOTIFY_ALL (NOTIFY_GENERIC | NOTIFY_STRING | NOTIFY_LIST | NOTIFY_SET | NOTIFY_HASH | NOTIFY_ZSET | NOTIFY_EXPIRED | NOTIFY_EVICTED | NOTIFY_STREAM | NOTIFY_MODULE) /* A flag */

/* This structure represents a Redis user. This is useful for ACLs, the
 * user is associated to the connection after the connection is authenticated.
 * If there is no associated user, the connection uses the default user. */
#define USER_COMMAND_BITS_COUNT 1024    /* The total number of command bits
                                           in the user structure. The last valid
                                           command ID we can set in the user
                                           is USER_COMMAND_BITS_COUNT-1. */
#define USER_FLAG_ENABLED (1<<0)        /* The user is active. */
#define USER_FLAG_DISABLED (1<<1)       /* The user is disabled. */
#define USER_FLAG_NOPASS (1<<2)         /* The user requires no password, any
                                           provided password will work. For the
                                           default user, this also means that
                                           no AUTH is needed, and every
                                           connection is immediately
                                           authenticated. */
#define USER_FLAG_SANITIZE_PAYLOAD (1<<3)       /* The user require a deep RESTORE
                                                 * payload sanitization. */
#define USER_FLAG_SANITIZE_PAYLOAD_SKIP (1<<4)  /* The user should skip the
                                                 * deep sanitization of RESTORE
                                                 * payload. */

#define SELECTOR_FLAG_ROOT (1<<0)           /* This is the root user permission
                                             * selector. */
#define SELECTOR_FLAG_ALLKEYS (1<<1)        /* The user can mention any key. */
#define SELECTOR_FLAG_ALLCOMMANDS (1<<2)    /* The user can run all commands. */
#define SELECTOR_FLAG_ALLCHANNELS (1<<3)    /* The user can mention any Pub/Sub
                                               channel. */

                                               /* Client classes for client limits, currently used only for
 * the max-client-output-buffer limit implementation. */

                                    // buffer configuration. Just the first
                                    // three: normal, slave, pubsub. */

/* This structure is used in order to represent the output buffer of a client,
 * which is actually a linked list of blocks like that, that is: client->reply. */
typedef struct clientReplyBlock {
    size_t size, used;
    char buf[];
} clientReplyBlock;




/* In Redis objects 'robj' structures of type OBJ_MODULE, the value pointer
 * is set to the following structure, referencing the moduleType structure
 * in order to work with the value, and at the same time providing a raw
 * pointer to the value, as created by the module commands operating with
 * the module type.
 *
 * So for example in order to free such a value, it is possible to use
 * the following code:
 *
 *  if (robj->type == OBJ_MODULE) {
 *      moduleValue *mt = robj->ptr;
 *      mt->type->free(mt->value);
 *      zfree(mt); // We need to release this in-the-middle struct as well.
 *  }
 */
typedef struct moduleValue {
    moduleType *type;
    void *value;
} moduleValue;

extern struct redisServer server;
extern struct sharedObjectsStruct shared;
extern dictType sdsHashDictType;

extern user *DefaultUser;
extern dictType objectKeyHeapPointerValueDictType;
extern dictType objectKeyPointerValueDictType;



/* Keys hashing / comparison functions for dict.c hash tables. */
uint64_t dictSdsHash(const void *key);
uint64_t dictSdsCaseHash(const void *key);
robj *getDecodedObject(robj *o);
robj *createStringObject(const char *ptr, size_t len);
robj *createEmbeddedStringObject(const char *ptr, size_t len);
robj *createRawStringObject(const char *ptr, size_t len);
void incrRefCount(robj *o);


// int dictSdsKeyCompare(dict *d, const void *key1, const void *key2);
// int dictSdsKeyCaseCompare(dict *d, const void *key1, const void *key2);
// void dictSdsDestructor(dict *d, void *val);
// void dictListDestructor(dict *d, void *val);
// void *dictSdsDup(dict *d, const void *key);
/* Type of configuration. */

/* Configuration Flags */
#define MODIFIABLE_CONFIG 0 /* This is the implied default for a standard 
                             * config, which is mutable. */
#define IMMUTABLE_CONFIG (1ULL<<0) /* Can this value only be set at startup? */
#define SENSITIVE_CONFIG (1ULL<<1) /* Does this value contain sensitive information */
#define DEBUG_CONFIG (1ULL<<2) /* Values that are useful for debugging. */
#define MULTI_ARG_CONFIG (1ULL<<3) /* This config receives multiple arguments. */
#define HIDDEN_CONFIG (1ULL<<4) /* This config is hidden in `config get <pattern>` (used for tests/debugging) */
#define PROTECTED_CONFIG (1ULL<<5) /* Becomes immutable if enable-protected-configs is enabled. */
#define DENY_LOADING_CONFIG (1ULL<<6) /* This config is forbidden during loading. */
#define ALIAS_CONFIG (1ULL<<7) /* For configs with multiple names, this flag is set on the alias. */
#define MODULE_CONFIG (1ULL<<8) /* This config is a module config */
#define VOLATILE_CONFIG (1ULL<<9) /* The config is a reference to the config data and not the config data itself (ex.
                                   * a file name containing more configuration like a tls key). In this case we want
                                   * to apply the configuration change even if the new config value is the same as
                                   * the old. */

#define INTEGER_CONFIG 0 /* No flags means a simple integer configuration */
#define MEMORY_CONFIG (1<<0) /* Indicates if this value can be loaded as a memory value */
#define PERCENT_CONFIG (1<<1) /* Indicates if this value can be loaded as a percent (and stored as a negative int) */
#define OCTAL_CONFIG (1<<2) /* This value uses octal representation */

#define OBJ_HASH_KEY 1
#define OBJ_HASH_VALUE 2

#define IO_THREADS_OP_IDLE 0
#define IO_THREADS_OP_READ 1
#define IO_THREADS_OP_WRITE 2
extern int io_threads_op;

/* Slave capabilities. */
#define SLAVE_CAPA_NONE 0
#define SLAVE_CAPA_EOF (1<<0)    /* Can parse the RDB EOF streaming format. */
#define SLAVE_CAPA_PSYNC2 (1<<1) /* Supports PSYNC2 protocol. */

/* State of slaves from the POV of the master. Used in client->replstate.
 * In SEND_BULK and ONLINE state the slave receives new updates
 * in its output queue. In the WAIT_BGSAVE states instead the server is waiting
 * to start the next background saving in order to send updates to it. */
#define SLAVE_STATE_WAIT_BGSAVE_START 6 /* We need to produce a new RDB file. */
#define SLAVE_STATE_WAIT_BGSAVE_END 7 /* Waiting RDB file creation to finish. */
#define SLAVE_STATE_SEND_BULK 8 /* Sending RDB file to slave. */
#define SLAVE_STATE_ONLINE 9 /* RDB file transmitted, sending just updates. */
#define SLAVE_STATE_RDB_TRANSMITTED 10 /* RDB file transmitted - This state is used only for
                                        * a replica that only wants RDB without replication buffer  */


/* Similar with 'clientReplyBlock', it is used for shared buffers between
 * all replica clients and replication backlog. */
typedef struct replBufBlock {
    int refcount;           /* Number of replicas or repl backlog using. */
    long long id;           /* The unique incremental number. */
    long long repl_offset;  /* Start replication offset of the block. */
    size_t size, used;
    char buf[];
} replBufBlock;

typedef enum {
    BOOL_CONFIG,
    NUMERIC_CONFIG,
    STRING_CONFIG,
    SDS_CONFIG,
    ENUM_CONFIG,
    SPECIAL_CONFIG,
} configType;

void _serverAssert(const char *estr, const char *file, int line);
/* acl.c -- Authentication related prototypes. */
// extern rax *Users;
// extern user *DefaultUser;
void ACLInit(void);

/* Core functions */
void loadServerConfig(char *filename, char config_from_stdin, char *options);
struct redisCommand *lookupCommandBySdsLogic(dict *commands, sds s);
struct redisCommand *lookupCommandBySds(sds s);
int ACLAppendUserForLoading(sds *argv, int argc, int *argc_err);
const char *ACLSetUserStringError(void);
robj *createObject(int type, void *ptr);
/* evict.c -- maxmemory handling and LRU eviction. */
#define LFU_INIT_VAL 5
void evictionPoolAlloc(void);
void watchdogScheduleSignal(int period);
void sigsegvHandler(int sig, siginfo_t *info, void *secret);

mstime_t mstime(void);
long long ustime(void);
sds catClientInfoString(sds s, client *client);

/* Synchronous I/O with timeout */
ssize_t syncWrite(int fd, char *ptr, ssize_t size, long long timeout);
ssize_t syncRead(int fd, char *ptr, ssize_t size, long long timeout);
ssize_t syncReadLine(int fd, char *ptr, ssize_t size, long long timeout);

/* networking.c -- Networking and Client related operations */
void acceptCommonHandler(connection *conn, int flags, char *ip);
void decrRefCountVoid(void *o);
void freeClient(client *c);
void decrRefCount(robj *o);
void freeClientMultiState(client *c);
int islocalClient(client *c);


//track.c
void disableTracking(client *c);

void readQueryFromClient(connection *conn);


void addReplyErrorArity(client *c);

/* Commands prototypes */
void addReply(client *c, robj *obj);
void authCommand(client *c);
void pingCommand(client *c);

void incrementErrorCount(const char *fullerr, size_t namelen);
// void showLatestBacklog(void);
size_t stringObjectLen(robj *o);
size_t getClientMemoryUsage(client *c, size_t *output_buffer_mem_usage);





#include "rdb.h"

#endif
