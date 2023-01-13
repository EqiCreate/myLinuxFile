#ifndef __REDIS_H
#define __REDIS_H

#include "config.h"
#include <stdio.h>

struct redisServer {
    /* General */
    // pid_t pid;                  /* Main process pid. */
    // pthread_t main_thread_id;         /* Main thread id */
    char *configfile;           /* Absolute config file path, or NULL */
    char *executable;           /* Absolute executable file path. */
    char **exec_argv;           /* Executable argv vector (copy). */
    int dynamic_hz;             /* Change hz value depending on # of clients. */
    int config_hz;              /* Configured HZ value. May be different than
                                   the actual 'hz' field value if dynamic-hz
                                   is enabled. */
    // mode_t umask;               /* The umask value of the process on startup */
    int hz;                     /* serverCron() calls frequency in hertz */
    int in_fork_child;          /* indication that this is a fork child */
    // redisDb *db;
    // dict *commands;             /* Command table */
    // dict *orig_commands;        /* Command table before command renaming. */
    // aeEventLoop *el;
    // rax *errors;                /* Errors table */
    // redisAtomic unsigned int lruclock; /* Clock for LRU eviction */
    // volatile sig_atomic_t shutdown_asap; /* Shutdown ordered by signal handler. */
    // mstime_t shutdown_mstime;   /* Timestamp to limit graceful shutdown. */
    // int last_sig_received;      /* Indicates the last SIGNAL received, if any (e.g., SIGINT or SIGTERM). */
    // int shutdown_flags;         /* Flags passed to prepareForShutdown(). */
    // int activerehashing;        /* Incremental rehash in serverCron() */
    // int active_defrag_running;  /* Active defragmentation running (holds current scan aggressiveness) */
    // char *pidfile;              /* PID file path */
    // int arch_bits;              /* 32 or 64 depending on sizeof(long) */
    // int cronloops;              /* Number of times the cron function run */
    // char runid[CONFIG_RUN_ID_SIZE+1];  /* ID always different at every exec. */
    // int sentinel_mode;          /* True if this instance is a Sentinel. */
    // size_t initial_memory_usage; /* Bytes used after initialization. */
    // int always_show_logo;       /* Show logo even for non-stdout logging. */
    // int in_exec;                /* Are we inside EXEC? */
    // int busy_module_yield_flags;         /* Are we inside a busy module? (triggered by RM_Yield). see BUSY_MODULE_YIELD_ flags. */
    // const char *busy_module_yield_reply; /* When non-null, we are inside RM_Yield. */
    // int core_propagates;        /* Is the core (in oppose to the module subsystem) is in charge of calling propagatePendingCommands? */
    // int module_ctx_nesting;     /* moduleCreateContext() nesting level */
    // char *ignore_warnings;      /* Config: warnings that should be ignored. */
    // int client_pause_in_transaction; /* Was a client pause executed during this Exec? */
    // int thp_enabled;                 /* If true, THP is enabled. */
    // size_t page_size;                /* The page size of OS. */
    // /* Modules */
    // dict *moduleapi;            /* Exported core APIs dictionary for modules. */
    // dict *sharedapi;            /* Like moduleapi but containing the APIs that
    //                                modules share with each other. */
    // dict *module_configs_queue; /* Dict that stores module configurations from .conf file until after modules are loaded during startup or arguments to loadex. */
    // list *loadmodule_queue;     /* List of modules to load at startup. */
    // int module_pipe[2];         /* Pipe used to awake the event loop by module threads. */
    // pid_t child_pid;            /* PID of current child */
    // int child_type;             /* Type of current child */
    // /* Networking */
    // int port;                   /* TCP listening port */
    // int tls_port;               /* TLS listening port */
    // int tcp_backlog;            /* TCP listen() backlog */
    // char *bindaddr[CONFIG_BINDADDR_MAX]; /* Addresses we should bind to */
    // int bindaddr_count;         /* Number of addresses in server.bindaddr[] */
    // char *bind_source_addr;     /* Source address to bind on for outgoing connections */
    // char *unixsocket;           /* UNIX socket path */
    // unsigned int unixsocketperm; /* UNIX socket permission (see mode_t) */
    // connListener listeners[CONN_TYPE_MAX]; /* TCP/Unix/TLS even more types */
    // uint32_t socket_mark_id;    /* ID for listen socket marking */
    // connListener clistener;     /* Cluster bus listener */
    // list *clients;              /* List of active clients */
    // list *clients_to_close;     /* Clients to close asynchronously */
    // list *clients_pending_write; /* There is to write or install handler. */
    // list *clients_pending_read;  /* Client has pending read socket buffers. */
    // list *slaves, *monitors;    /* List of slaves and MONITORs */
    // client *current_client;     /* Current client executing the command. */

    // /* Stuff for client mem eviction */
    // clientMemUsageBucket client_mem_usage_buckets[CLIENT_MEM_USAGE_BUCKETS];

    // rax *clients_timeout_table; /* Radix tree for blocked clients timeouts. */
    // int in_nested_call;         /* If > 0, in a nested call of a call */
    // rax *clients_index;         /* Active clients dictionary by client ID. */
    // uint32_t paused_actions;   /* Bitmask of actions that are currently paused */
    // list *postponed_clients;       /* List of postponed clients */
    // pause_event client_pause_per_purpose[NUM_PAUSE_PURPOSES];
    // char neterr[ANET_ERR_LEN];   /* Error buffer for anet.c */
    // dict *migrate_cached_sockets;/* MIGRATE cached sockets */
    // redisAtomic uint64_t next_client_id; /* Next client unique ID. Incremental. */
    // int protected_mode;         /* Don't accept external connections. */
    // int io_threads_num;         /* Number of IO threads to use. */
    // int io_threads_do_reads;    /* Read and parse from IO threads? */
    // int io_threads_active;      /* Is IO threads currently active? */
    // long long events_processed_while_blocked; /* processEventsWhileBlocked() */
    // int enable_protected_configs;    /* Enable the modification of protected configs, see PROTECTED_ACTION_ALLOWED_* */
    // int enable_debug_cmd;            /* Enable DEBUG commands, see PROTECTED_ACTION_ALLOWED_* */
    // int enable_module_cmd;           /* Enable MODULE commands, see PROTECTED_ACTION_ALLOWED_* */

    // /* RDB / AOF loading information */
    // volatile sig_atomic_t loading; /* We are loading data from disk if true */
    // volatile sig_atomic_t async_loading; /* We are loading data without blocking the db being served */
    // off_t loading_total_bytes;
    // off_t loading_rdb_used_mem;
    // off_t loading_loaded_bytes;
    // time_t loading_start_time;
    // off_t loading_process_events_interval_bytes;
    // /* Fields used only for stats */
    // time_t stat_starttime;          /* Server start time */
    // long long stat_numcommands;     /* Number of processed commands */
    // long long stat_numconnections;  /* Number of connections received */
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
    // size_t stat_peak_memory;        /* Max used memory record */
    // long long stat_aof_rewrites;    /* number of aof file rewrites performed */
    // long long stat_aofrw_consecutive_failures; /* The number of consecutive failures of aofrw */
    // long long stat_rdb_saves;       /* number of rdb saves performed */
    // long long stat_fork_time;       /* Time needed to perform latest fork() */
    // double stat_fork_rate;          /* Fork rate in GB/sec. */
    // long long stat_total_forks;     /* Total count of fork. */
    // long long stat_rejected_conn;   /* Clients rejected because of maxclients */
    // long long stat_sync_full;       /* Number of full resyncs with slaves. */
    // long long stat_sync_partial_ok; /* Number of accepted PSYNC requests. */
    // long long stat_sync_partial_err;/* Number of unaccepted PSYNC requests. */
    // list *slowlog;                  /* SLOWLOG list of commands */
    // long long slowlog_entry_id;     /* SLOWLOG current entry ID */
    // long long slowlog_log_slower_than; /* SLOWLOG time limit (to get logged) */
    // unsigned long slowlog_max_len;     /* SLOWLOG max number of items logged */
    // struct malloc_stats cron_malloc_stats; /* sampled in serverCron(). */
    // redisAtomic long long stat_net_input_bytes; /* Bytes read from network. */
    // redisAtomic long long stat_net_output_bytes; /* Bytes written to network. */
    // redisAtomic long long stat_net_repl_input_bytes; /* Bytes read during replication, added to stat_net_input_bytes in 'info'. */
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
    // size_t stat_clients_type_memory[CLIENT_TYPE_COUNT];/* Mem usage by type */
    // size_t stat_cluster_links_memory; /* Mem usage by cluster links */
    // long long stat_unexpected_error_replies; /* Number of unexpected (aof-loading, replica to master, etc.) error replies */
    // long long stat_total_error_replies; /* Total number of issued error replies ( command + rejected errors ) */
    // long long stat_dump_payload_sanitizations; /* Number deep dump payloads integrity validations. */
    // long long stat_io_reads_processed; /* Number of read events processed by IO / Main threads */
    // long long stat_io_writes_processed; /* Number of write events processed by IO / Main threads */
    // redisAtomic long long stat_total_reads_processed; /* Total number of read events processed */
    // redisAtomic long long stat_total_writes_processed; /* Total number of write events processed */
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
    // int verbosity;                  /* Loglevel in redis.conf */
    // int maxidletime;                /* Client timeout in seconds */
    // int tcpkeepalive;               /* Set SO_KEEPALIVE if non-zero. */
    // int active_expire_enabled;      /* Can be disabled for testing purposes. */
    // int active_expire_effort;       /* From 1 (default) to 10, active effort. */
    // int active_defrag_enabled;
    // int sanitize_dump_payload;      /* Enables deep sanitization for ziplist and listpack in RDB and RESTORE. */
    // int skip_checksum_validation;   /* Disable checksum validation for RDB and RESTORE payload. */
    // int jemalloc_bg_thread;         /* Enable jemalloc background thread */
    // size_t active_defrag_ignore_bytes; /* minimum amount of fragmentation waste to start active defrag */
    // int active_defrag_threshold_lower; /* minimum percentage of fragmentation to start active defrag */
    // int active_defrag_threshold_upper; /* maximum percentage of fragmentation at which we use maximum effort */
    // int active_defrag_cycle_min;       /* minimal effort for defrag in CPU percentage */
    // int active_defrag_cycle_max;       /* maximal effort for defrag in CPU percentage */
    // unsigned long active_defrag_max_scan_fields; /* maximum number of fields of set/hash/zset/list to process from within the main dict scan */
    // size_t client_max_querybuf_len; /* Limit for client query buffer length */
    // int dbnum;                      /* Total number of configured DBs */
    // int supervised;                 /* 1 if supervised, 0 otherwise. */
    // int supervised_mode;            /* See SUPERVISED_* */
    // int daemonize;                  /* True if running as a daemon */
    // int set_proc_title;             /* True if change proc title */
    // char *proc_title_template;      /* Process title template format */
    // clientBufferLimitsConfig client_obuf_limits[CLIENT_TYPE_OBUF_COUNT];
    // int pause_cron;                 /* Don't run cron tasks (debug) */
    // int latency_tracking_enabled;   /* 1 if extended latency tracking is enabled, 0 otherwise. */
    // double *latency_tracking_info_percentiles; /* Extended latency tracking info output percentile list configuration. */
    // int latency_tracking_info_percentiles_len;
    // /* AOF persistence */
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
    // long long dirty;                /* Changes to DB from the last save */
    // long long dirty_before_bgsave;  /* Used to restore dirty on failed BGSAVE */
    // long long rdb_last_load_keys_expired;  /* number of expired keys when loading RDB */
    // long long rdb_last_load_keys_loaded;   /* number of loaded keys when loading RDB */
    // struct saveparam *saveparams;   /* Save points array for RDB */
    // int saveparamslen;              /* Number of saving points */
    // char *rdb_filename;             /* Name of RDB file */
    // int rdb_compression;            /* Use compression in RDB? */
    // int rdb_checksum;               /* Use RDB checksum? */
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
    // int replication_allowed;        /* Are we allowed to replicate? */
    // /* Logging */
    // char *logfile;                  /* Path of log file */
    // int syslog_enabled;             /* Is syslog enabled? */
    // char *syslog_ident;             /* Syslog ident */
    // int syslog_facility;            /* Syslog facility */
    // int crashlog_enabled;           /* Enable signal handler for crashlog.
    //                                  * disable for clean core dumps. */
    // int memcheck_enabled;           /* Enable memory check on crash. */
    // int use_exit_on_panic;          /* Use exit() on panic and assert rather than
    //                                  * abort(). useful for Valgrind. */
    // /* Shutdown */
    // int shutdown_timeout;           /* Graceful shutdown time limit in seconds. */
    // int shutdown_on_sigint;         /* Shutdown flags configured for SIGINT. */
    // int shutdown_on_sigterm;        /* Shutdown flags configured for SIGTERM. */

    // /* Replication (master) */
    // char replid[CONFIG_RUN_ID_SIZE+1];  /* My current replication ID. */
    // char replid2[CONFIG_RUN_ID_SIZE+1]; /* replid inherited from master*/
    // long long master_repl_offset;   /* My current replication offset */
    // long long second_replid_offset; /* Accept offsets up to this for replid2. */
    // int slaveseldb;                 /* Last SELECTed DB in replication output */
    // int repl_ping_slave_period;     /* Master pings the slave every N seconds */
    // replBacklog *repl_backlog;      /* Replication backlog for partial syncs */
    // long long repl_backlog_size;    /* Backlog circular buffer size */
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
    // list *repl_buffer_blocks;       /* Replication buffers blocks list
    //                                  * (serving replica clients and repl backlog) */
    // /* Replication (slave) */
    // char *masteruser;               /* AUTH with this user and masterauth with master */
    // sds masterauth;                 /* AUTH with this password with master */
    // char *masterhost;               /* Hostname of master */
    // int masterport;                 /* Port of master */
    // int repl_timeout;               /* Timeout after N seconds of master idle */
    // client *master;     /* Client that is master for this slave */
    // client *cached_master; /* Cached master to be reused for PSYNC. */
    // int repl_syncio_timeout; /* Timeout for synchronous I/O calls */
    // int repl_state;          /* Replication status if the instance is a slave */
    // off_t repl_transfer_size; /* Size of RDB to read from master during sync. */
    // off_t repl_transfer_read; /* Amount of RDB read from master during sync. */
    // off_t repl_transfer_last_fsync_off; /* Offset when we fsync-ed last time. */
    // connection *repl_transfer_s;     /* Slave -> Master SYNC connection */
    // int repl_transfer_fd;    /* Slave -> Master SYNC temp file descriptor */
    // char *repl_transfer_tmpfile; /* Slave-> master SYNC temp file name */
    // time_t repl_transfer_lastio; /* Unix time of the latest read, for timeout */
    // int repl_serve_stale_data; /* Serve stale data when link is down? */
    // int repl_slave_ro;          /* Slave is read only? */
    // int repl_slave_ignore_maxmemory;    /* If true slaves do not evict. */
    // time_t repl_down_since; /* Unix time at which link with master went down */
    // int repl_disable_tcp_nodelay;   /* Disable TCP_NODELAY after SYNC? */
    // int slave_priority;             /* Reported in INFO and used by Sentinel. */
    // int replica_announced;          /* If true, replica is announced by Sentinel */
    // int slave_announce_port;        /* Give the master this listening port. */
    // char *slave_announce_ip;        /* Give the master this ip address. */
    // int propagation_error_behavior; /* Configures the behavior of the replica
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
    // unsigned int maxclients;            /* Max number of simultaneous clients */
    // unsigned long long maxmemory;   /* Max number of memory bytes to use */
    // ssize_t maxmemory_clients;       /* Memory limit for total client buffers */
    // int maxmemory_policy;           /* Policy for key eviction */
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
    // list *unblocked_clients; /* list of clients to unblock before next loop */
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
    // redisAtomic time_t unixtime; /* Unix time sampled every cron cycle. */
    // time_t timezone;            /* Cached timezone. As set by tzset(). */
    // int daylight_active;        /* Currently in daylight saving time. */
    // mstime_t mstime;            /* 'unixtime' in milliseconds. */
    // ustime_t ustime;            /* 'unixtime' in microseconds. */
    // size_t blocking_op_nesting; /* Nesting level of blocking operation, used to reset blocked_last_cron. */
    // long long blocked_last_cron; /* Indicate the mstime of the last time we did cron jobs from a blocking operation */
    // /* Pubsub */
    // dict *pubsub_channels;  /* Map channels to list of subscribed clients */
    // dict *pubsub_patterns;  /* A dict of pubsub_patterns */
    // int notify_keyspace_events; /* Events to propagate via Pub/Sub. This is an
    //                                xor of NOTIFY_... flags. */
    // dict *pubsubshard_channels;  /* Map shard channels to list of subscribed clients */
    // /* Cluster */
    // int cluster_enabled;      /* Is cluster enabled? */
    // int cluster_port;         /* Set the cluster port for a node. */
    // mstime_t cluster_node_timeout; /* Cluster node timeout. */
    // mstime_t cluster_ping_interval;    /* A debug configuration for setting how often cluster nodes send ping messages. */
    // char *cluster_configfile; /* Cluster auto-generated config file name. */
    // struct clusterState *cluster;  /* State of the cluster */
    // int cluster_migration_barrier; /* Cluster replicas migration barrier. */
    // int cluster_allow_replica_migration; /* Automatic replica migrations to orphaned masters and from empty masters */
    // int cluster_slave_validity_factor; /* Slave max data age for failover. */
    // int cluster_require_full_coverage; /* If true, put the cluster down if
    //                                       there is at least an uncovered slot.*/
    // int cluster_slave_no_failover;  /* Prevent slave from starting a failover
    //                                    if the master is in failure state. */
    // char *cluster_announce_ip;  /* IP address to announce on cluster bus. */
    // char *cluster_announce_hostname;  /* hostname to announce on cluster bus. */
    // int cluster_preferred_endpoint_type; /* Use the announced hostname when available. */
    // int cluster_announce_port;     /* base port to announce on cluster bus. */
    // int cluster_announce_tls_port; /* TLS port to announce on cluster bus. */
    // int cluster_announce_bus_port; /* bus port to announce on cluster bus. */
    // int cluster_module_flags;      /* Set of flags that Redis modules are able
    //                                   to set in order to suppress certain
    //                                   native Redis Cluster features. Check the
    //                                   REDISMODULE_CLUSTER_FLAG_*. */
    // int cluster_allow_reads_when_down; /* Are reads allowed when the cluster
    //                                     is down? */
    // int cluster_config_file_lock_fd;   /* cluster config fd, will be flock */
    // unsigned long long cluster_link_msg_queue_limit_bytes;  /* Memory usage limit on individual link msg queue */
    // int cluster_drop_packet_filter; /* Debug config that allows tactically
    //                                * dropping packets of a specific type */
    // /* Scripting */
    // client *script_caller;       /* The client running script right now, or NULL */
    // mstime_t busy_reply_threshold;  /* Script / module timeout in milliseconds */
    // int pre_command_oom_state;         /* OOM before command (script?) was started */
    // int script_disable_deny_script;    /* Allow running commands marked "no-script" inside a script. */
    // /* Lazy free */
    // int lazyfree_lazy_eviction;
    // int lazyfree_lazy_expire;
    // int lazyfree_lazy_server_del;
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
    // int watchdog_period;  /* Software watchdog period in ms. 0 = off */
    // /* System hardware info */
    // size_t system_memory_size;  /* Total memory in system as reported by OS */
    // /* TLS Configuration */
    // int tls_cluster;
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
    // char *locale_collate;
};

#endif
