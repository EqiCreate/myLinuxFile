#include "fmacros.h"
#include "version.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <assert.h>
#include <fcntl.h>
#include <limits.h>
#include <math.h>

#include <hiredis.h>
#include <sdscompat.h> /* Use hiredis' sds compat header that maps sds calls to their hi_ variants */
#include <sds.h> /* use sds.h from hiredis, so that only one set of sds functions will be present in the binary */
#include "dict.h"
#include "adlist.h"
#include "zmalloc.h"
#include "linenoise.h"
#include "help.h" /* Used for backwards-compatibility with pre-7.0 servers that don't support COMMAND DOCS. */
#include "anet.h"
#include "ae.h"
#include "connection.h"
#include "cli_common.h"
#include "Main/mt19937-64.h"

#define UNUSED(V) ((void) V)

#define OUTPUT_STANDARD 0
#define OUTPUT_RAW 1
#define OUTPUT_CSV 2
#define OUTPUT_JSON 3
#define OUTPUT_QUOTED_JSON 4
#define REDIS_CLI_KEEPALIVE_INTERVAL 15 /* seconds */
#define REDIS_CLI_DEFAULT_PIPE_TIMEOUT 30 /* seconds */
#define REDIS_CLI_HISTFILE_ENV "REDISCLI_HISTFILE"
#define REDIS_CLI_HISTFILE_DEFAULT ".rediscli_history"
#define REDIS_CLI_RCFILE_ENV "REDISCLI_RCFILE"
#define REDIS_CLI_RCFILE_DEFAULT ".redisclirc"
#define REDIS_CLI_AUTH_ENV "REDISCLI_AUTH"
#define REDIS_CLI_CLUSTER_YES_ENV "REDISCLI_CLUSTER_YES"

#define CLUSTER_MANAGER_FLAG_MYSELF     1 << 0
#define CLUSTER_MANAGER_FLAG_SLAVE      1 << 1
#define CLUSTER_MANAGER_FLAG_FRIEND     1 << 2
#define CLUSTER_MANAGER_FLAG_NOADDR     1 << 3
#define CLUSTER_MANAGER_FLAG_DISCONNECT 1 << 4
#define CLUSTER_MANAGER_FLAG_FAIL       1 << 5

#define CLUSTER_MANAGER_CMD_FLAG_FIX            1 << 0
#define CLUSTER_MANAGER_CMD_FLAG_SLAVE          1 << 1
#define CLUSTER_MANAGER_CMD_FLAG_YES            1 << 2
#define CLUSTER_MANAGER_CMD_FLAG_AUTOWEIGHTS    1 << 3
#define CLUSTER_MANAGER_CMD_FLAG_EMPTYMASTER    1 << 4
#define CLUSTER_MANAGER_CMD_FLAG_SIMULATE       1 << 5
#define CLUSTER_MANAGER_CMD_FLAG_REPLACE        1 << 6
#define CLUSTER_MANAGER_CMD_FLAG_COPY           1 << 7
#define CLUSTER_MANAGER_CMD_FLAG_COLOR          1 << 8
#define CLUSTER_MANAGER_CMD_FLAG_CHECK_OWNERS   1 << 9
#define CLUSTER_MANAGER_CMD_FLAG_FIX_WITH_UNREACHABLE_MASTERS 1 << 10
#define CLUSTER_MANAGER_CMD_FLAG_MASTERS_ONLY   1 << 11
#define CLUSTER_MANAGER_CMD_FLAG_SLAVES_ONLY    1 << 12

#define CLUSTER_MANAGER_OPT_GETFRIENDS  1 << 0
#define CLUSTER_MANAGER_OPT_COLD        1 << 1
#define CLUSTER_MANAGER_OPT_UPDATE      1 << 2
#define CLUSTER_MANAGER_OPT_QUIET       1 << 6
#define CLUSTER_MANAGER_OPT_VERBOSE     1 << 7

#define CLUSTER_MANAGER_LOG_LVL_INFO    1
#define CLUSTER_MANAGER_LOG_LVL_WARN    2
#define CLUSTER_MANAGER_LOG_LVL_ERR     3
#define CLUSTER_MANAGER_LOG_LVL_SUCCESS 4

#define CLUSTER_JOIN_CHECK_AFTER        20

#define LOG_COLOR_BOLD      "29;1m"
#define LOG_COLOR_RED       "31;1m"
#define LOG_COLOR_GREEN     "32;1m"
#define LOG_COLOR_YELLOW    "33;1m"
#define LOG_COLOR_RESET     "0m"

#define CLI_HELP_COMMAND 1
#define CLI_HELP_GROUP 2


/* Command documentation info used for help output */
struct commandDocs {
    char *name;
    char *params; /* A string describing the syntax of the command arguments. */
    char *summary;
    char *group;
    char *since;
};
typedef struct {
    int type;
    int argc;
    sds *argv;
    sds full;

    /* Only used for help on commands */
    struct commandDocs org;
} helpEntry;

static helpEntry *helpEntries = NULL;
static int helpEntriesLen = 0;

/* cliConnect() flags. */
#define CC_FORCE (1<<0)         /* Re-connect if already connected. */
#define CC_QUIET (1<<1)         /* Don't log connecting errors. */

/* DNS lookup */
#define NET_IP_STR_LEN 46       /* INET6_ADDRSTRLEN is 46 */

/* --latency-dist palettes. */
int spectrum_palette_color_size = 19;
int spectrum_palette_color[] = {0,233,234,235,237,239,241,243,245,247,144,143,142,184,226,214,208,202,196};

int spectrum_palette_mono_size = 13;
int spectrum_palette_mono[] = {0,233,234,235,237,239,241,243,245,247,249,251,253};

/* The actual palette in use. */
int *spectrum_palette;
int spectrum_palette_size;

#define CLUSTER_MANAGER_MODE() (config.cluster_manager_command.name != NULL)
#define CLUSTER_MANAGER_MASTERS_COUNT(nodes, replicas) ((nodes)/((replicas) + 1))

typedef struct clusterManagerCommand {
    char *name;
    int argc;
    char **argv;
    sds stdin_arg; /* arg from stdin. (-X option) */
    int flags;
    int replicas;
    char *from;
    char *to;
    char **weight;
    int weight_argc;
    char *master_id;
    int slots;
    int timeout;
    int pipeline;
    float threshold;
    char *backup_dir;
    char *from_user;
    char *from_pass;
    int from_askpass;
} clusterManagerCommand;

static struct config {
    cliConnInfo conn_info;
    char *hostsocket;
    int tls;
    cliSSLconfig sslconfig;
    long repeat;
    long interval;
    int dbnum; /* db num currently selected */
    int interactive;
    int shutdown;
    int monitor_mode;
    int pubsub_mode;
    int blocking_state_aborted; /* used to abort monitor_mode and pubsub_mode. */
    int latency_mode;
    int latency_dist_mode;
    int latency_history;
    int lru_test_mode;
    long long lru_test_sample_size;
    int cluster_mode;
    int cluster_reissue_command;
    int cluster_send_asking;
    int slave_mode;
    int pipe_mode;
    int pipe_timeout;
    int getrdb_mode;
    int get_functions_rdb_mode;
    int stat_mode;
    int scan_mode;
    int intrinsic_latency_mode;
    int intrinsic_latency_duration;
    sds pattern;
    char *rdb_filename;
    int bigkeys;
    int memkeys;
    unsigned memkeys_samples;
    int hotkeys;
    int stdin_lastarg; /* get last arg from stdin. (-x option) */
    int stdin_tag_arg; /* get <tag> arg from stdin. (-X option) */
    char *stdin_tag_name; /* Placeholder(tag name) for user input. */
    int askpass;
    int quoted_input;   /* Force input args to be treated as quoted strings */
    int output; /* output mode, see OUTPUT_* defines */
    int push_output; /* Should we display spontaneous PUSH replies */
    sds mb_delim;
    sds cmd_delim;
    char prompt[128];
    char *eval;
    int eval_ldb;
    int eval_ldb_sync;  /* Ask for synchronous mode of the Lua debugger. */
    int eval_ldb_end;   /* Lua debugging session ended. */
    int enable_ldb_on_eval; /* Handle manual SCRIPT DEBUG + EVAL commands. */
    int last_cmd_type;
    int verbose;
    int set_errcode;
    clusterManagerCommand cluster_manager_command;
    int no_auth_warning;
    int resp2;
    int resp3; /* value of 1: specified explicitly, value of 2: implicit like --json option */
    int in_multi;
    int pre_multi_dbnum;
} config;

/* User preferences. */
static struct pref {
    int hints;
} pref;

static void usage(int err);

static int createClusterManagerCommand(char *cmdname, int argc, char **argv);
static redisContext *context;
static int cliConnect(int flags);
static void cliPushHandler(void *, void *);

static long long ustime(void) {
    struct timeval tv;
    long long ust;

    gettimeofday(&tv, NULL);
    ust = ((long long)tv.tv_sec)*1000000;
    ust += tv.tv_usec;
    return ust;
}
static long long mstime(void) {
    return ustime()/1000;
}


/*------------------------------------------------------------------------------
 * User interface
 *--------------------------------------------------------------------------- */

static sds cliVersion(void) {
    sds version;
    version = sdscatprintf(sdsempty(), "%s", REDIS_VERSION);

    /* Add git commit and working tree status when available */
    // if (strtoll(redisGitSHA1(),NULL,16)) {
    //     version = sdscatprintf(version, " (git:%s", redisGitSHA1());
    //     if (strtoll(redisGitDirty(),NULL,10))
    //         version = sdscatprintf(version, "-dirty");
    //     version = sdscat(version, ")");
    // } //debug michael
    return version;
}
static void usage(int err) {
    sds version = cliVersion();
    FILE *target = err ? stderr: stdout;
    fprintf(target,
"redis-cli %s\n"
"\n"
"Usage: redis-cli [OPTIONS] [cmd [arg [arg ...]]]\n"
"  -h <hostname>      Server hostname (default: 127.0.0.1).\n"
"  -p <port>          Server port (default: 6379).\n"
"  -s <socket>        Server socket (overrides hostname and port).\n"
"  -a <password>      Password to use when connecting to the server.\n"
"                     You can also use the " REDIS_CLI_AUTH_ENV " environment\n"
"                     variable to pass this password more safely\n"
"                     (if both are used, this argument takes precedence).\n"
"  --user <username>  Used to send ACL style 'AUTH username pass'. Needs -a.\n"
"  --pass <password>  Alias of -a for consistency with the new --user option.\n"
"  --askpass          Force user to input password with mask from STDIN.\n"
"                     If this argument is used, '-a' and " REDIS_CLI_AUTH_ENV "\n"
"                     environment variable will be ignored.\n"
"  -u <uri>           Server URI.\n"
"  -r <repeat>        Execute specified command N times.\n"
"  -i <interval>      When -r is used, waits <interval> seconds per command.\n"
"                     It is possible to specify sub-second times like -i 0.1.\n"
"                     This interval is also used in --scan and --stat per cycle.\n"
"                     and in --bigkeys, --memkeys, and --hotkeys per 100 cycles.\n"
"  -n <db>            Database number.\n"
"  -2                 Start session in RESP2 protocol mode.\n"
"  -3                 Start session in RESP3 protocol mode.\n"
"  -x                 Read last argument from STDIN (see example below).\n"
"  -X                 Read <tag> argument from STDIN (see example below).\n"
"  -d <delimiter>     Delimiter between response bulks for raw formatting (default: \\n).\n"
"  -D <delimiter>     Delimiter between responses for raw formatting (default: \\n).\n"
"  -c                 Enable cluster mode (follow -ASK and -MOVED redirections).\n"
"  -e                 Return exit error code when command execution fails.\n"
#ifdef USE_OPENSSL
"  --tls              Establish a secure TLS connection.\n"
"  --sni <host>       Server name indication for TLS.\n"
"  --cacert <file>    CA Certificate file to verify with.\n"
"  --cacertdir <dir>  Directory where trusted CA certificates are stored.\n"
"                     If neither cacert nor cacertdir are specified, the default\n"
"                     system-wide trusted root certs configuration will apply.\n"
"  --insecure         Allow insecure TLS connection by skipping cert validation.\n"
"  --cert <file>      Client certificate to authenticate with.\n"
"  --key <file>       Private key file to authenticate with.\n"
"  --tls-ciphers <list> Sets the list of preferred ciphers (TLSv1.2 and below)\n"
"                     in order of preference from highest to lowest separated by colon (\":\").\n"
"                     See the ciphers(1ssl) manpage for more information about the syntax of this string.\n"
#ifdef TLS1_3_VERSION
"  --tls-ciphersuites <list> Sets the list of preferred ciphersuites (TLSv1.3)\n"
"                     in order of preference from highest to lowest separated by colon (\":\").\n"
"                     See the ciphers(1ssl) manpage for more information about the syntax of this string,\n"
"                     and specifically for TLSv1.3 ciphersuites.\n"
#endif
#endif
"  --raw              Use raw formatting for replies (default when STDOUT is\n"
"                     not a tty).\n"
"  --no-raw           Force formatted output even when STDOUT is not a tty.\n"
"  --quoted-input     Force input to be handled as quoted strings.\n"
"  --csv              Output in CSV format.\n"
"  --json             Output in JSON format (default RESP3, use -2 if you want to use with RESP2).\n"
"  --quoted-json      Same as --json, but produce ASCII-safe quoted strings, not Unicode.\n"
"  --show-pushes <yn> Whether to print RESP3 PUSH messages.  Enabled by default when\n"
"                     STDOUT is a tty but can be overridden with --show-pushes no.\n"
"  --stat             Print rolling stats about server: mem, clients, ...\n",version);

    fprintf(target,
"  --latency          Enter a special mode continuously sampling latency.\n"
"                     If you use this mode in an interactive session it runs\n"
"                     forever displaying real-time stats. Otherwise if --raw or\n"
"                     --csv is specified, or if you redirect the output to a non\n"
"                     TTY, it samples the latency for 1 second (you can use\n"
"                     -i to change the interval), then produces a single output\n"
"                     and exits.\n"
"  --latency-history  Like --latency but tracking latency changes over time.\n"
"                     Default time interval is 15 sec. Change it using -i.\n"
"  --latency-dist     Shows latency as a spectrum, requires xterm 256 colors.\n"
"                     Default time interval is 1 sec. Change it using -i.\n"
"  --lru-test <keys>  Simulate a cache workload with an 80-20 distribution.\n"
"  --replica          Simulate a replica showing commands received from the master.\n"
"  --rdb <filename>   Transfer an RDB dump from remote server to local file.\n"
"                     Use filename of \"-\" to write to stdout.\n"
" --functions-rdb <filename> Like --rdb but only get the functions (not the keys)\n"
"                     when getting the RDB dump file.\n"
"  --pipe             Transfer raw Redis protocol from stdin to server.\n"
"  --pipe-timeout <n> In --pipe mode, abort with error if after sending all data.\n"
"                     no reply is received within <n> seconds.\n"
"                     Default timeout: %d. Use 0 to wait forever.\n",
    REDIS_CLI_DEFAULT_PIPE_TIMEOUT);
    fprintf(target,
"  --bigkeys          Sample Redis keys looking for keys with many elements (complexity).\n"
"  --memkeys          Sample Redis keys looking for keys consuming a lot of memory.\n"
"  --memkeys-samples <n> Sample Redis keys looking for keys consuming a lot of memory.\n"
"                     And define number of key elements to sample\n"
"  --hotkeys          Sample Redis keys looking for hot keys.\n"
"                     only works when maxmemory-policy is *lfu.\n"
"  --scan             List all keys using the SCAN command.\n"
"  --pattern <pat>    Keys pattern when using the --scan, --bigkeys or --hotkeys\n"
"                     options (default: *).\n"
"  --quoted-pattern <pat> Same as --pattern, but the specified string can be\n"
"                         quoted, in order to pass an otherwise non binary-safe string.\n"
"  --intrinsic-latency <sec> Run a test to measure intrinsic system latency.\n"
"                     The test will run for the specified amount of seconds.\n"
"  --eval <file>      Send an EVAL command using the Lua script at <file>.\n"
"  --ldb              Used with --eval enable the Redis Lua debugger.\n"
"  --ldb-sync-mode    Like --ldb but uses the synchronous Lua debugger, in\n"
"                     this mode the server is blocked and script changes are\n"
"                     not rolled back from the server memory.\n"
"  --cluster <command> [args...] [opts...]\n"
"                     Cluster Manager command and arguments (see below).\n"
"  --verbose          Verbose mode.\n"
"  --no-auth-warning  Don't show warning message when using password on command\n"
"                     line interface.\n"
"  --help             Output this help and exit.\n"
"  --version          Output version and exit.\n"
"\n");
    /* Using another fprintf call to avoid -Woverlength-strings compile warning */
    fprintf(target,
"Cluster Manager Commands:\n"
"  Use --cluster help to list all available cluster manager commands.\n"
"\n"
"Examples:\n"
"  cat /etc/passwd | redis-cli -x set mypasswd\n"
"  redis-cli -D \"\" --raw dump key > key.dump && redis-cli -X dump_tag restore key2 0 dump_tag replace < key.dump\n"
"  redis-cli -r 100 lpush mylist x\n"
"  redis-cli -r 100 -i 1 info | grep used_memory_human:\n"
"  redis-cli --quoted-input set '\"null-\\x00-separated\"' value\n"
"  redis-cli --eval myscript.lua key1 key2 , arg1 arg2 arg3\n"
"  redis-cli --scan --pattern '*:12345*'\n"
"\n"
"  (Note: when using --eval the comma separates KEYS[] from ARGV[] items)\n"
"\n"
"When no command is given, redis-cli starts in interactive mode.\n"
"Type \"help\" in interactive mode for information on available commands\n"
"and settings.\n"
"\n");
    sdsfree(version);
    exit(err);
}
int isColorTerm(void) {
    char *t = getenv("TERM");
    return t != NULL && strstr(t,"xterm") != NULL;
}

/* Comparator for sorting help table entries. */
int helpEntryCompare(const void *entry1, const void *entry2) {
    helpEntry *i1 = (helpEntry *)entry1;
    helpEntry *i2 = (helpEntry *)entry2;
    return strcmp(i1->full, i2->full);
}
static int createClusterManagerCommand(char *cmdname, int argc, char **argv) {
    clusterManagerCommand *cmd = &config.cluster_manager_command;
    cmd->name = cmdname;
    cmd->argc = argc;
    cmd->argv = argc ? argv : NULL;
    if (isColorTerm()) cmd->flags |= CLUSTER_MANAGER_CMD_FLAG_COLOR;

    if (config.stdin_lastarg) {
        char **new_argv = zmalloc(sizeof(char*) * (cmd->argc+1));
        memcpy(new_argv, cmd->argv, sizeof(char*) * cmd->argc);

        cmd->stdin_arg = readArgFromStdin();
        new_argv[cmd->argc++] = cmd->stdin_arg;
        cmd->argv = new_argv;
    } else if (config.stdin_tag_arg) {
        int i = 0, tag_match = 0;
        cmd->stdin_arg = readArgFromStdin();

        for (; i < argc; i++) {
            if (strcmp(argv[i], config.stdin_tag_name) != 0) continue;

            tag_match = 1;
            cmd->argv[i] = (char *)cmd->stdin_arg;
            break;
        }

        if (!tag_match) {
            sdsfree(cmd->stdin_arg);
            fprintf(stderr, "Using -X option but stdin tag not match.\n");
            return 1;
        }
    }

    return 0;
}

static int parseOptions(int argc, char **argv) {
    int i;

    for (i = 1; i < argc; i++) {
        int lastarg = i==argc-1;

        if (!strcmp(argv[i],"-h") && !lastarg) {
            sdsfree(config.conn_info.hostip);
            config.conn_info.hostip = sdsnew(argv[++i]);
        } else if (!strcmp(argv[i],"-h") && lastarg) {
            usage(0);
        } else if (!strcmp(argv[i],"--help")) {
            usage(0);
        } else if (!strcmp(argv[i],"-x")) {
            config.stdin_lastarg = 1;
        } else if (!strcmp(argv[i], "-X") && !lastarg) {
            config.stdin_tag_arg = 1;
            config.stdin_tag_name = argv[++i];
        } else if (!strcmp(argv[i],"-p") && !lastarg) {
            config.conn_info.hostport = atoi(argv[++i]);
            if (config.conn_info.hostport < 0 || config.conn_info.hostport > 65535) {
                fprintf(stderr, "Invalid server port.\n");
                exit(1);
            }
        } else if (!strcmp(argv[i],"-s") && !lastarg) {
            config.hostsocket = argv[++i];
        } else if (!strcmp(argv[i],"-r") && !lastarg) {
            config.repeat = strtoll(argv[++i],NULL,10);
        } else if (!strcmp(argv[i],"-i") && !lastarg) {
            double seconds = atof(argv[++i]);
            config.interval = seconds*1000000;
        } else if (!strcmp(argv[i],"-n") && !lastarg) {
            config.conn_info.input_dbnum = atoi(argv[++i]);
        } else if (!strcmp(argv[i], "--no-auth-warning")) {
            config.no_auth_warning = 1;
        } else if (!strcmp(argv[i], "--askpass")) {
            config.askpass = 1;
        } else if ((!strcmp(argv[i],"-a") || !strcmp(argv[i],"--pass"))
                   && !lastarg)
        {
            config.conn_info.auth = sdsnew(argv[++i]);
        } else if (!strcmp(argv[i],"--user") && !lastarg) {
            config.conn_info.user = sdsnew(argv[++i]);
        } else if (!strcmp(argv[i],"-u") && !lastarg) {
            parseRedisUri(argv[++i],"redis-cli",&config.conn_info,&config.tls);
            if (config.conn_info.hostport < 0 || config.conn_info.hostport > 65535) {
                fprintf(stderr, "Invalid server port.\n");
                exit(1);
            }
        } else if (!strcmp(argv[i],"--raw")) {
            config.output = OUTPUT_RAW;
        } else if (!strcmp(argv[i],"--no-raw")) {
            config.output = OUTPUT_STANDARD;
        } else if (!strcmp(argv[i],"--quoted-input")) {
            config.quoted_input = 1;
        } else if (!strcmp(argv[i],"--csv")) {
            config.output = OUTPUT_CSV;
        } else if (!strcmp(argv[i],"--json")) {
            /* Not overwrite explicit value by -3 */
            if (config.resp3 == 0) {
                config.resp3 = 2;
            }
            config.output = OUTPUT_JSON;
        } else if (!strcmp(argv[i],"--quoted-json")) {
            /* Not overwrite explicit value by -3*/
            if (config.resp3 == 0) {
                config.resp3 = 2;
            }
            config.output = OUTPUT_QUOTED_JSON;
        } else if (!strcmp(argv[i],"--latency")) {
            config.latency_mode = 1;
        } else if (!strcmp(argv[i],"--latency-dist")) {
            config.latency_dist_mode = 1;
        } else if (!strcmp(argv[i],"--mono")) {
            spectrum_palette = spectrum_palette_mono;
            spectrum_palette_size = spectrum_palette_mono_size;
        } else if (!strcmp(argv[i],"--latency-history")) {
            config.latency_mode = 1;
            config.latency_history = 1;
        } else if (!strcmp(argv[i],"--lru-test") && !lastarg) {
            config.lru_test_mode = 1;
            config.lru_test_sample_size = strtoll(argv[++i],NULL,10);
        } else if (!strcmp(argv[i],"--slave")) {
            config.slave_mode = 1;
        } else if (!strcmp(argv[i],"--replica")) {
            config.slave_mode = 1;
        } else if (!strcmp(argv[i],"--stat")) {
            config.stat_mode = 1;
        } else if (!strcmp(argv[i],"--scan")) {
            config.scan_mode = 1;
        } else if (!strcmp(argv[i],"--pattern") && !lastarg) {
            sdsfree(config.pattern);
            config.pattern = sdsnew(argv[++i]);
        } else if (!strcmp(argv[i],"--quoted-pattern") && !lastarg) {
            sdsfree(config.pattern);
            config.pattern = unquoteCString(argv[++i]);
            if (!config.pattern) {
                fprintf(stderr,"Invalid quoted string specified for --quoted-pattern.\n");
                exit(1);
            }
        } else if (!strcmp(argv[i],"--intrinsic-latency") && !lastarg) {
            config.intrinsic_latency_mode = 1;
            config.intrinsic_latency_duration = atoi(argv[++i]);
        } else if (!strcmp(argv[i],"--rdb") && !lastarg) {
            config.getrdb_mode = 1;
            config.rdb_filename = argv[++i];
        } else if (!strcmp(argv[i],"--functions-rdb") && !lastarg) {
            config.get_functions_rdb_mode = 1;
            config.rdb_filename = argv[++i];
        } else if (!strcmp(argv[i],"--pipe")) {
            config.pipe_mode = 1;
        } else if (!strcmp(argv[i],"--pipe-timeout") && !lastarg) {
            config.pipe_timeout = atoi(argv[++i]);
        } else if (!strcmp(argv[i],"--bigkeys")) {
            config.bigkeys = 1;
        } else if (!strcmp(argv[i],"--memkeys")) {
            config.memkeys = 1;
            config.memkeys_samples = 0; /* use redis default */
        } else if (!strcmp(argv[i],"--memkeys-samples") && !lastarg) {
            config.memkeys = 1;
            config.memkeys_samples = atoi(argv[++i]);
        } else if (!strcmp(argv[i],"--hotkeys")) {
            config.hotkeys = 1;
        } else if (!strcmp(argv[i],"--eval") && !lastarg) {
            config.eval = argv[++i];
        } else if (!strcmp(argv[i],"--ldb")) {
            config.eval_ldb = 1;
            config.output = OUTPUT_RAW;
        } else if (!strcmp(argv[i],"--ldb-sync-mode")) {
            config.eval_ldb = 1;
            config.eval_ldb_sync = 1;
            config.output = OUTPUT_RAW;
        } else if (!strcmp(argv[i],"-c")) {
            config.cluster_mode = 1;
        } else if (!strcmp(argv[i],"-d") && !lastarg) {
            sdsfree(config.mb_delim);
            config.mb_delim = sdsnew(argv[++i]);
        } else if (!strcmp(argv[i],"-D") && !lastarg) {
            sdsfree(config.cmd_delim);
            config.cmd_delim = sdsnew(argv[++i]);
        } else if (!strcmp(argv[i],"-e")) {
            config.set_errcode = 1;
        } else if (!strcmp(argv[i],"--verbose")) {
            config.verbose = 1;
        } else if (!strcmp(argv[i],"--cluster") && !lastarg) {
            if (CLUSTER_MANAGER_MODE()) usage(1);
            char *cmd = argv[++i];
            int j = i;
            while (j < argc && argv[j][0] != '-') j++;
            if (j > i) j--;
            int err = createClusterManagerCommand(cmd, j - i, argv + i + 1);
            if (err) exit(err);
            i = j;
        } else if (!strcmp(argv[i],"--cluster") && lastarg) {
            usage(1);
        } else if ((!strcmp(argv[i],"--cluster-only-masters"))) {
            config.cluster_manager_command.flags |=
                    CLUSTER_MANAGER_CMD_FLAG_MASTERS_ONLY;
        } else if ((!strcmp(argv[i],"--cluster-only-replicas"))) {
            config.cluster_manager_command.flags |=
                    CLUSTER_MANAGER_CMD_FLAG_SLAVES_ONLY;
        } else if (!strcmp(argv[i],"--cluster-replicas") && !lastarg) {
            config.cluster_manager_command.replicas = atoi(argv[++i]);
        } else if (!strcmp(argv[i],"--cluster-master-id") && !lastarg) {
            config.cluster_manager_command.master_id = argv[++i];
        } else if (!strcmp(argv[i],"--cluster-from") && !lastarg) {
            config.cluster_manager_command.from = argv[++i];
        } else if (!strcmp(argv[i],"--cluster-to") && !lastarg) {
            config.cluster_manager_command.to = argv[++i];
        } else if (!strcmp(argv[i],"--cluster-from-user") && !lastarg) {
            config.cluster_manager_command.from_user = argv[++i];
        } else if (!strcmp(argv[i],"--cluster-from-pass") && !lastarg) {
            config.cluster_manager_command.from_pass = argv[++i];
        } else if (!strcmp(argv[i], "--cluster-from-askpass")) {
            config.cluster_manager_command.from_askpass = 1;
        } else if (!strcmp(argv[i],"--cluster-weight") && !lastarg) {
            if (config.cluster_manager_command.weight != NULL) {
                fprintf(stderr, "WARNING: you cannot use --cluster-weight "
                                "more than once.\n"
                                "You can set more weights by adding them "
                                "as a space-separated list, ie:\n"
                                "--cluster-weight n1=w n2=w\n");
                exit(1);
            }
            int widx = i + 1;
            char **weight = argv + widx;
            int wargc = 0;
            for (; widx < argc; widx++) {
                if (strstr(argv[widx], "--") == argv[widx]) break;
                if (strchr(argv[widx], '=') == NULL) break;
                wargc++;
            }
            if (wargc > 0) {
                config.cluster_manager_command.weight = weight;
                config.cluster_manager_command.weight_argc = wargc;
                i += wargc;
            }
        } else if (!strcmp(argv[i],"--cluster-slots") && !lastarg) {
            config.cluster_manager_command.slots = atoi(argv[++i]);
        } else if (!strcmp(argv[i],"--cluster-timeout") && !lastarg) {
            config.cluster_manager_command.timeout = atoi(argv[++i]);
        } else if (!strcmp(argv[i],"--cluster-pipeline") && !lastarg) {
            config.cluster_manager_command.pipeline = atoi(argv[++i]);
        } else if (!strcmp(argv[i],"--cluster-threshold") && !lastarg) {
            config.cluster_manager_command.threshold = atof(argv[++i]);
        } else if (!strcmp(argv[i],"--cluster-yes")) {
            config.cluster_manager_command.flags |=
                CLUSTER_MANAGER_CMD_FLAG_YES;
        } else if (!strcmp(argv[i],"--cluster-simulate")) {
            config.cluster_manager_command.flags |=
                CLUSTER_MANAGER_CMD_FLAG_SIMULATE;
        } else if (!strcmp(argv[i],"--cluster-replace")) {
            config.cluster_manager_command.flags |=
                CLUSTER_MANAGER_CMD_FLAG_REPLACE;
        } else if (!strcmp(argv[i],"--cluster-copy")) {
            config.cluster_manager_command.flags |=
                CLUSTER_MANAGER_CMD_FLAG_COPY;
        } else if (!strcmp(argv[i],"--cluster-slave")) {
            config.cluster_manager_command.flags |=
                CLUSTER_MANAGER_CMD_FLAG_SLAVE;
        } else if (!strcmp(argv[i],"--cluster-use-empty-masters")) {
            config.cluster_manager_command.flags |=
                CLUSTER_MANAGER_CMD_FLAG_EMPTYMASTER;
        } else if (!strcmp(argv[i],"--cluster-search-multiple-owners")) {
            config.cluster_manager_command.flags |=
                CLUSTER_MANAGER_CMD_FLAG_CHECK_OWNERS;
        } else if (!strcmp(argv[i],"--cluster-fix-with-unreachable-masters")) {
            config.cluster_manager_command.flags |=
                CLUSTER_MANAGER_CMD_FLAG_FIX_WITH_UNREACHABLE_MASTERS;
#ifdef USE_OPENSSL
        } else if (!strcmp(argv[i],"--tls")) {
            config.tls = 1;
        } else if (!strcmp(argv[i],"--sni") && !lastarg) {
            config.sslconfig.sni = argv[++i];
        } else if (!strcmp(argv[i],"--cacertdir") && !lastarg) {
            config.sslconfig.cacertdir = argv[++i];
        } else if (!strcmp(argv[i],"--cacert") && !lastarg) {
            config.sslconfig.cacert = argv[++i];
        } else if (!strcmp(argv[i],"--cert") && !lastarg) {
            config.sslconfig.cert = argv[++i];
        } else if (!strcmp(argv[i],"--key") && !lastarg) {
            config.sslconfig.key = argv[++i];
        } else if (!strcmp(argv[i],"--tls-ciphers") && !lastarg) {
            config.sslconfig.ciphers = argv[++i];
        } else if (!strcmp(argv[i],"--insecure")) {
            config.sslconfig.skip_cert_verify = 1;
        #ifdef TLS1_3_VERSION
        } else if (!strcmp(argv[i],"--tls-ciphersuites") && !lastarg) {
            config.sslconfig.ciphersuites = argv[++i];
        #endif
#endif
        } else if (!strcmp(argv[i],"-v") || !strcmp(argv[i], "--version")) {
            sds version = cliVersion();
            printf("redis-cli %s\n", version);
            sdsfree(version);
            exit(0);
        } else if (!strcmp(argv[i],"-2")) {
            config.resp2 = 1;
        } else if (!strcmp(argv[i],"-3")) {
            config.resp3 = 1;
        } else if (!strcmp(argv[i],"--show-pushes") && !lastarg) {
            char *argval = argv[++i];
            if (!strncasecmp(argval, "n", 1)) {
                config.push_output = 0;
            } else if (!strncasecmp(argval, "y", 1)) {
                config.push_output = 1;
            } else {
                fprintf(stderr, "Unknown --show-pushes value '%s' "
                        "(valid: '[y]es', '[n]o')\n", argval);
            }
        } else if (CLUSTER_MANAGER_MODE() && argv[i][0] != '-') {
            if (config.cluster_manager_command.argc == 0) {
                int j = i + 1;
                while (j < argc && argv[j][0] != '-') j++;
                int cmd_argc = j - i;
                config.cluster_manager_command.argc = cmd_argc;
                config.cluster_manager_command.argv = argv + i;
                if (cmd_argc > 1) i = j - 1;
            }
        } else {
            if (argv[i][0] == '-') {
                fprintf(stderr,
                    "Unrecognized option or bad number of args for: '%s'\n",
                    argv[i]);
                exit(1);
            } else {
                /* Likely the command name, stop here. */
                break;
            }
        }
    }

    if (config.hostsocket && config.cluster_mode) {
        fprintf(stderr,"Options -c and -s are mutually exclusive.\n");
        exit(1);
    }

    if (config.resp2 && config.resp3 == 1) {
        fprintf(stderr,"Options -2 and -3 are mutually exclusive.\n");
        exit(1);
    }

    /* --ldb requires --eval. */
    if (config.eval_ldb && config.eval == NULL) {
        fprintf(stderr,"Options --ldb and --ldb-sync-mode require --eval.\n");
        fprintf(stderr,"Try %s --help for more information.\n", argv[0]);
        exit(1);
    }

    if (!config.no_auth_warning && config.conn_info.auth != NULL) {
        fputs("Warning: Using a password with '-a' or '-u' option on the command"
              " line interface may not be safe.\n", stderr);
    }

    if (config.get_functions_rdb_mode && config.getrdb_mode) {
        fprintf(stderr,"Option --functions-rdb and --rdb are mutually exclusive.\n");
        exit(1);
    }
 
    if (config.stdin_lastarg && config.stdin_tag_arg) {
        fprintf(stderr, "Options -x and -X are mutually exclusive.\n");
        exit(1);
    }

    return i;
}
static void parseEnv() {
    /* Set auth from env, but do not overwrite CLI arguments if passed */
    char *auth = getenv(REDIS_CLI_AUTH_ENV);
    if (auth != NULL && config.conn_info.auth == NULL) {
        config.conn_info.auth = auth;
    }

    char *cluster_yes = getenv(REDIS_CLI_CLUSTER_YES_ENV);
    if (cluster_yes != NULL && !strcmp(cluster_yes, "1")) {
        config.cluster_manager_command.flags |= CLUSTER_MANAGER_CMD_FLAG_YES;
    }
}
static void sigIntHandler(int s) {
    UNUSED(s);

    if (config.monitor_mode || config.pubsub_mode) {
        close(context->fd);
        context->fd = REDIS_INVALID_FD;
        config.blocking_state_aborted = 1;
    } else {
        exit(1);
    }
}


static void cliRefreshPrompt(void) {
    if (config.eval_ldb) return;

    sds prompt = sdsempty();
    if (config.hostsocket != NULL) {
        prompt = sdscatfmt(prompt,"redis %s",config.hostsocket);
    } else {
        char addr[256];
        formatAddr(addr, sizeof(addr), config.conn_info.hostip, config.conn_info.hostport);
        prompt = sdscatlen(prompt,addr,strlen(addr));
    }

    /* Add [dbnum] if needed */
    if (config.dbnum != 0)
        prompt = sdscatfmt(prompt,"[%i]",config.dbnum);

    /* Add TX if in transaction state*/
    if (config.in_multi)  
        prompt = sdscatlen(prompt,"(TX)",4);

    /* Copy the prompt in the static buffer. */
    prompt = sdscatlen(prompt,"> ",2);
    snprintf(config.prompt,sizeof(config.prompt),"%s",prompt);
    sdsfree(prompt);
}

static int cliAuth(redisContext *ctx, char *user, char *auth) {
    redisReply *reply;
    if (auth == NULL) return REDIS_OK;

    if (user == NULL)
        reply = redisCommand(ctx,"AUTH %s",auth);
    else
        reply = redisCommand(ctx,"AUTH %s %s",user,auth);

    if (reply == NULL) {
        fprintf(stderr, "\nI/O error\n");
        return REDIS_ERR;
    }

    int result = REDIS_OK;
    if (reply->type == REDIS_REPLY_ERROR) {
        result = REDIS_ERR;
        fprintf(stderr, "AUTH failed: %s\n", reply->str);
    }
    freeReplyObject(reply);
    return result;
}

/* Send SELECT input_dbnum to the server */
static int cliSelect(void) {
    redisReply *reply;
    if (config.conn_info.input_dbnum == config.dbnum) return REDIS_OK;

    reply = redisCommand(context,"SELECT %d",config.conn_info.input_dbnum);
    if (reply == NULL) {
        fprintf(stderr, "\nI/O error\n");
        return REDIS_ERR;
    }

    int result = REDIS_OK;
    if (reply->type == REDIS_REPLY_ERROR) {
        result = REDIS_ERR;
        fprintf(stderr,"SELECT %d failed: %s\n",config.conn_info.input_dbnum,reply->str);
    } else {
        config.dbnum = config.conn_info.input_dbnum;
        cliRefreshPrompt();
    }
    freeReplyObject(reply);
    return result;
}


/* Select RESP3 mode if redis-cli was started with the -3 option.  */
static int cliSwitchProto(void) {
    redisReply *reply;
    if (!config.resp3 || config.resp2) return REDIS_OK;

    reply = redisCommand(context,"HELLO 3");
    if (reply == NULL) {
        fprintf(stderr, "\nI/O error\n");
        return REDIS_ERR;
    }

    int result = REDIS_OK;
    if (reply->type == REDIS_REPLY_ERROR) {
        fprintf(stderr,"HELLO 3 failed: %s\n",reply->str);
        if (config.resp3 == 1) {
            result = REDIS_ERR;
        } else if (config.resp3 == 2) {
            result = REDIS_OK;
        }
    }
    freeReplyObject(reply);
    return result;
}

static int isInvalidateReply(redisReply *reply) {
    return reply->type == REDIS_REPLY_PUSH && reply->elements == 2 &&
        reply->element[0]->type == REDIS_REPLY_STRING &&
        !strncmp(reply->element[0]->str, "invalidate", 10) &&
        reply->element[1]->type == REDIS_REPLY_ARRAY;
}



/* Special display handler for RESP3 'invalidate' messages.
 * This function does not validate the reply, so it should
 * already be confirmed correct */
static sds cliFormatInvalidateTTY(redisReply *r) {
    sds out = sdsnew("-> invalidate: ");

    for (size_t i = 0; i < r->element[1]->elements; i++) {
        redisReply *key = r->element[1]->element[i];
        assert(key->type == REDIS_REPLY_STRING);

        out = sdscatfmt(out, "'%s'", key->str, key->len);
        if (i < r->element[1]->elements - 1)
            out = sdscatlen(out, ", ", 2);
    }

    return sdscatlen(out, "\n", 1);
}

/* Returns non-zero if cliFormatReplyTTY renders the reply in multiple lines. */
static int cliIsMultilineValueTTY(redisReply *r) {
    switch (r->type) {
    case REDIS_REPLY_ARRAY:
    case REDIS_REPLY_SET:
    case REDIS_REPLY_PUSH:
        if (r->elements == 0) return 0;
        if (r->elements > 1) return 1;
        return cliIsMultilineValueTTY(r->element[0]);
    case REDIS_REPLY_MAP:
        if (r->elements == 0) return 0;
        if (r->elements > 2) return 1;
        return cliIsMultilineValueTTY(r->element[1]);
    default:
        return 0;
    }
}

static sds cliFormatReplyTTY(redisReply *r, char *prefix) {
    sds out = sdsempty();
    switch (r->type) {
    case REDIS_REPLY_ERROR:
        out = sdscatprintf(out,"(error) %s\n", r->str);
    break;
    case REDIS_REPLY_STATUS:
        out = sdscat(out,r->str);
        out = sdscat(out,"\n");
    break;
    case REDIS_REPLY_INTEGER:
        out = sdscatprintf(out,"(integer) %lld\n",r->integer);
    break;
    case REDIS_REPLY_DOUBLE:
        out = sdscatprintf(out,"(double) %s\n",r->str);
    break;
    case REDIS_REPLY_STRING:
    case REDIS_REPLY_VERB:
        /* If you are producing output for the standard output we want
        * a more interesting output with quoted characters and so forth,
        * unless it's a verbatim string type. */
        if (r->type == REDIS_REPLY_STRING) {
            out = sdscatrepr(out,r->str,r->len);
            out = sdscat(out,"\n");
        } else {
            out = sdscatlen(out,r->str,r->len);
            out = sdscat(out,"\n");
        }
    break;
    case REDIS_REPLY_NIL:
        out = sdscat(out,"(nil)\n");
    break;
    case REDIS_REPLY_BOOL:
        out = sdscat(out,r->integer ? "(true)\n" : "(false)\n");
    break;
    case REDIS_REPLY_ARRAY:
    case REDIS_REPLY_MAP:
    case REDIS_REPLY_SET:
    case REDIS_REPLY_PUSH:
        if (r->elements == 0) {
            if (r->type == REDIS_REPLY_ARRAY)
                out = sdscat(out,"(empty array)\n");
            else if (r->type == REDIS_REPLY_MAP)
                out = sdscat(out,"(empty hash)\n");
            else if (r->type == REDIS_REPLY_SET)
                out = sdscat(out,"(empty set)\n");
            else if (r->type == REDIS_REPLY_PUSH)
                out = sdscat(out,"(empty push)\n");
            else
                out = sdscat(out,"(empty aggregate type)\n");
        } else {
            unsigned int i, idxlen = 0;
            char _prefixlen[16];
            char _prefixfmt[16];
            sds _prefix;
            sds tmp;

            /* Calculate chars needed to represent the largest index */
            i = r->elements;
            if (r->type == REDIS_REPLY_MAP) i /= 2;
            do {
                idxlen++;
                i /= 10;
            } while(i);

            /* Prefix for nested multi bulks should grow with idxlen+2 spaces */
            memset(_prefixlen,' ',idxlen+2);
            _prefixlen[idxlen+2] = '\0';
            _prefix = sdscat(sdsnew(prefix),_prefixlen);

            /* Setup prefix format for every entry */
            char numsep;
            if (r->type == REDIS_REPLY_SET) numsep = '~';
            else if (r->type == REDIS_REPLY_MAP) numsep = '#';
            else numsep = ')';
            snprintf(_prefixfmt,sizeof(_prefixfmt),"%%s%%%ud%c ",idxlen,numsep);

            for (i = 0; i < r->elements; i++) {
                unsigned int human_idx = (r->type == REDIS_REPLY_MAP) ?
                                         i/2 : i;
                human_idx++; /* Make it 1-based. */

                /* Don't use the prefix for the first element, as the parent
                 * caller already prepended the index number. */
                out = sdscatprintf(out,_prefixfmt,i == 0 ? "" : prefix,human_idx);

                /* Format the multi bulk entry */
                tmp = cliFormatReplyTTY(r->element[i],_prefix);
                out = sdscatlen(out,tmp,sdslen(tmp));
                sdsfree(tmp);

                /* For maps, format the value as well. */
                if (r->type == REDIS_REPLY_MAP) {
                    i++;
                    sdsrange(out,0,-2);
                    out = sdscat(out," => ");
                    if (cliIsMultilineValueTTY(r->element[i])) {
                        /* linebreak before multiline value to fix alignment */
                        out = sdscat(out, "\n");
                        out = sdscat(out, _prefix);
                    }
                    tmp = cliFormatReplyTTY(r->element[i],_prefix);
                    out = sdscatlen(out,tmp,sdslen(tmp));
                    sdsfree(tmp);
                }
            }
            sdsfree(_prefix);
        }
    break;
    default:
        fprintf(stderr,"Unknown reply type: %d\n", r->type);
        exit(1);
    }
    return out;
}

/* Helper function for sdsCatColorizedLdbReply() appending colorize strings
 * to an SDS string. */
sds sdscatcolor(sds o, char *s, size_t len, char *color) {
    if (!isColorTerm()) return sdscatlen(o,s,len);

    int bold = strstr(color,"bold") != NULL;
    int ccode = 37; /* Defaults to white. */
    if (strstr(color,"red")) ccode = 31;
    else if (strstr(color,"green")) ccode = 32;
    else if (strstr(color,"yellow")) ccode = 33;
    else if (strstr(color,"blue")) ccode = 34;
    else if (strstr(color,"magenta")) ccode = 35;
    else if (strstr(color,"cyan")) ccode = 36;
    else if (strstr(color,"white")) ccode = 37;

    o = sdscatfmt(o,"\033[%i;%i;49m",bold,ccode);
    o = sdscatlen(o,s,len);
    o = sdscat(o,"\033[0m");
    return o;
}

/* Colorize Lua debugger status replies according to the prefix they
 * have. */
sds sdsCatColorizedLdbReply(sds o, char *s, size_t len) {
    char *color = "white";

    if (strstr(s,"<debug>")) color = "bold";
    if (strstr(s,"<redis>")) color = "green";
    if (strstr(s,"<reply>")) color = "cyan";
    if (strstr(s,"<error>")) color = "red";
    if (strstr(s,"<hint>")) color = "bold";
    if (strstr(s,"<value>") || strstr(s,"<retval>")) color = "magenta";
    if (len > 4 && isdigit(s[3])) {
        if (s[1] == '>') color = "yellow"; /* Current line. */
        else if (s[2] == '#') color = "bold"; /* Break point. */
    }
    return sdscatcolor(o,s,len,color);
}

static sds cliFormatReplyRaw(redisReply *r) {
    sds out = sdsempty(), tmp;
    size_t i;

    switch (r->type) {
    case REDIS_REPLY_NIL:
        /* Nothing... */
        break;
    case REDIS_REPLY_ERROR:
        out = sdscatlen(out,r->str,r->len);
        out = sdscatlen(out,"\n",1);
        break;
    case REDIS_REPLY_STATUS:
    case REDIS_REPLY_STRING:
    case REDIS_REPLY_VERB:
        if (r->type == REDIS_REPLY_STATUS && config.eval_ldb) {
            /* The Lua debugger replies with arrays of simple (status)
             * strings. We colorize the output for more fun if this
             * is a debugging session. */

            /* Detect the end of a debugging session. */
            if (strstr(r->str,"<endsession>") == r->str) {
                config.enable_ldb_on_eval = 0;
                config.eval_ldb = 0;
                config.eval_ldb_end = 1; /* Signal the caller session ended. */
                config.output = OUTPUT_STANDARD;
                cliRefreshPrompt();
            } else {
                out = sdsCatColorizedLdbReply(out,r->str,r->len);
            }
        } else {
            out = sdscatlen(out,r->str,r->len);
        }
        break;
    case REDIS_REPLY_BOOL:
        out = sdscat(out,r->integer ? "(true)" : "(false)");
    break;
    case REDIS_REPLY_INTEGER:
        out = sdscatprintf(out,"%lld",r->integer);
        break;
    case REDIS_REPLY_DOUBLE:
        out = sdscatprintf(out,"%s",r->str);
        break;
    case REDIS_REPLY_SET:
    case REDIS_REPLY_ARRAY:
    case REDIS_REPLY_PUSH:
        for (i = 0; i < r->elements; i++) {
            if (i > 0) out = sdscat(out,config.mb_delim);
            tmp = cliFormatReplyRaw(r->element[i]);
            out = sdscatlen(out,tmp,sdslen(tmp));
            sdsfree(tmp);
        }
        break;
    case REDIS_REPLY_MAP:
        for (i = 0; i < r->elements; i += 2) {
            if (i > 0) out = sdscat(out,config.mb_delim);
            tmp = cliFormatReplyRaw(r->element[i]);
            out = sdscatlen(out,tmp,sdslen(tmp));
            sdsfree(tmp);

            out = sdscatlen(out," ",1);
            tmp = cliFormatReplyRaw(r->element[i+1]);
            out = sdscatlen(out,tmp,sdslen(tmp));
            sdsfree(tmp);
        }
        break;
    default:
        fprintf(stderr,"Unknown reply type: %d\n", r->type);
        exit(1);
    }
    return out;
}

/* Generate reply strings in various output modes */
static sds cliFormatReply(redisReply *reply, int mode, int verbatim) {
    sds out;

    if (verbatim) {
        out = cliFormatReplyRaw(reply);
    }  else if (mode == OUTPUT_STANDARD) {
        out = cliFormatReplyTTY(reply, "");
    } 
    // else if (mode == OUTPUT_RAW) {
    //     out = cliFormatReplyRaw(reply);
    //     out = sdscatsds(out, config.cmd_delim);
    // } else if (mode == OUTPUT_CSV) {
    //     out = cliFormatReplyCSV(reply);
    //     out = sdscatlen(out, "\n", 1);
    // } else if (mode == OUTPUT_JSON || mode == OUTPUT_QUOTED_JSON) {
    //     out = cliFormatReplyJson(sdsempty(), reply, mode);
    //     out = sdscatlen(out, "\n", 1);
    // }
    //  else {
    //     fprintf(stderr, "Error:  Unknown output encoding %d\n", mode);
    //     exit(1);
    // }

    return out;
}


/* Output any spontaneous PUSH reply we receive */
static void cliPushHandler(void *privdata, void *reply) {
    UNUSED(privdata);
    sds out;

    if (config.output == OUTPUT_STANDARD && isInvalidateReply(reply)) {
        out = cliFormatInvalidateTTY(reply);
    } else {
        out = cliFormatReply(reply, config.output, 0);
    }

    fwrite(out, sdslen(out), 1, stdout);

    freeReplyObject(reply);
    sdsfree(out);
}

/* Connect to the server. It is possible to pass certain flags to the function:
 *      CC_FORCE: The connection is performed even if there is already
 *                a connected socket.
 *      CC_QUIET: Don't print errors if connection fails. */
static int cliConnect(int flags) {
    if (context == NULL || flags & CC_FORCE) {
        if (context != NULL) {
            redisFree(context);
            config.dbnum = 0;
            config.in_multi = 0;
            cliRefreshPrompt();
        }

        /* Do not use hostsocket when we got redirected in cluster mode */
        if (config.hostsocket == NULL ||
            (config.cluster_mode && config.cluster_reissue_command)) {
            context = redisConnect(config.conn_info.hostip,config.conn_info.hostport);
        } else {
            context = redisConnectUnix(config.hostsocket);
        }

        if (!context->err && config.tls) {
            const char *err = NULL;
            if (cliSecureConnection(context, config.sslconfig, &err) == REDIS_ERR && err) {
                fprintf(stderr, "Could not negotiate a TLS connection: %s\n", err);
                redisFree(context);
                context = NULL;
                return REDIS_ERR;
            }
        }

        if (context->err) {
            if (!(flags & CC_QUIET)) {
                fprintf(stderr,"Could not connect to Redis at ");
                if (config.hostsocket == NULL ||
                    (config.cluster_mode && config.cluster_reissue_command))
                {
                    fprintf(stderr, "%s:%d: %s\n",
                        config.conn_info.hostip,config.conn_info.hostport,context->errstr);
                } else {
                    fprintf(stderr,"%s: %s\n",
                        config.hostsocket,context->errstr);
                }
            }
            redisFree(context);
            context = NULL;
            return REDIS_ERR;
        }


        /* Set aggressive KEEP_ALIVE socket option in the Redis context socket
         * in order to prevent timeouts caused by the execution of long
         * commands. At the same time this improves the detection of real
         * errors. */
        anetKeepAlive(NULL, context->fd, REDIS_CLI_KEEPALIVE_INTERVAL);

        /* Do AUTH, select the right DB, switch to RESP3 if needed. */
        if (cliAuth(context, config.conn_info.user, config.conn_info.auth) != REDIS_OK)
            return REDIS_ERR;
        if (cliSelect() != REDIS_OK)
            return REDIS_ERR;
        if (cliSwitchProto() != REDIS_OK)
            return REDIS_ERR;
    }

    /* Set a PUSH handler if configured to do so. */
    if (config.push_output) {
        redisSetPushCallback(context, cliPushHandler);
    }

    return REDIS_OK;
}

static uint64_t dictSdsHash(const void *key) {
    return dictGenHashFunction((unsigned char*)key, sdslen((char*)key));
}

static int dictSdsKeyCompare(dict *d, const void *key1, const void *key2)
{
    int l1,l2;
    UNUSED(d);

    l1 = sdslen((sds)key1);
    l2 = sdslen((sds)key2);
    if (l1 != l2) return 0;
    return memcmp(key1, key2, l1) == 0;
}

static void dictSdsDestructor(dict *d, void *val)
{
    UNUSED(d);
    sdsfree(val);
}

void dictListDestructor(dict *d, void *val)
{
    UNUSED(d);
    listRelease((list*)val);
}

/* For backwards compatibility with pre-7.0 servers. Initializes command help. */
static void cliOldInitHelp(void) {
    int commandslen = sizeof(commandHelp)/sizeof(struct commandHelp);
    int groupslen = sizeof(commandGroups)/sizeof(char*);
    int i, len, pos = 0;
    helpEntry tmp;

    helpEntriesLen = len = commandslen+groupslen;
    helpEntries = zmalloc(sizeof(helpEntry)*len);

    for (i = 0; i < groupslen; i++) {
        tmp.argc = 1;
        tmp.argv = zmalloc(sizeof(sds));
        tmp.argv[0] = sdscatprintf(sdsempty(),"@%s",commandGroups[i]);
        tmp.full = tmp.argv[0];
        tmp.type = CLI_HELP_GROUP;
        tmp.org.name = NULL;
        tmp.org.params = NULL;
        tmp.org.summary = NULL;
        tmp.org.since = NULL;
        tmp.org.group = NULL;
        helpEntries[pos++] = tmp;
    }

    for (i = 0; i < commandslen; i++) {
        tmp.argv = sdssplitargs(commandHelp[i].name,&tmp.argc);
        tmp.full = sdsnew(commandHelp[i].name);
        tmp.type = CLI_HELP_COMMAND;
        tmp.org.name = commandHelp[i].name;
        tmp.org.params = commandHelp[i].params;
        tmp.org.summary = commandHelp[i].summary;
        tmp.org.since = commandHelp[i].since;
        tmp.org.group = commandGroups[commandHelp[i].group];
        helpEntries[pos++] = tmp;
    }
}

/* For backwards compatibility with pre-7.0 servers.
 * cliOldInitHelp() setups the helpEntries array with the command and group
 * names from the help.h file. However the Redis instance we are connecting
 * to may support more commands, so this function integrates the previous
 * entries with additional entries obtained using the COMMAND command
 * available in recent versions of Redis. */
static void cliOldIntegrateHelp(void) {
    if (cliConnect(CC_QUIET) == REDIS_ERR) return;

    redisReply *reply = redisCommand(context, "COMMAND");
    if(reply == NULL || reply->type != REDIS_REPLY_ARRAY) return;

    /* Scan the array reported by COMMAND and fill only the entries that
     * don't already match what we have. */
    for (size_t j = 0; j < reply->elements; j++) {
        redisReply *entry = reply->element[j];
        if (entry->type != REDIS_REPLY_ARRAY || entry->elements < 4 ||
            entry->element[0]->type != REDIS_REPLY_STRING ||
            entry->element[1]->type != REDIS_REPLY_INTEGER ||
            entry->element[3]->type != REDIS_REPLY_INTEGER) return;
        char *cmdname = entry->element[0]->str;
        int i;

        for (i = 0; i < helpEntriesLen; i++) {
            helpEntry *he = helpEntries+i;
            if (!strcasecmp(he->argv[0],cmdname))
                break;
        }
        if (i != helpEntriesLen) continue;

        helpEntriesLen++;
        helpEntries = zrealloc(helpEntries,sizeof(helpEntry)*helpEntriesLen);
        helpEntry *new = helpEntries+(helpEntriesLen-1);

        new->argc = 1;
        new->argv = zmalloc(sizeof(sds));
        new->argv[0] = sdsnew(cmdname);
        new->full = new->argv[0];
        new->type = CLI_HELP_COMMAND;
        sdstoupper(new->argv[0]);

        new->org.name = new->argv[0];
        new->org.params = sdsempty();
        int args = llabs(entry->element[1]->integer);
        args--; /* Remove the command name itself. */
        if (entry->element[3]->integer == 1) {
            new->org.params = sdscat(new->org.params,"key ");
            args--;
        }
        while(args-- > 0) new->org.params = sdscat(new->org.params,"arg ");
        if (entry->element[1]->integer < 0)
            new->org.params = sdscat(new->org.params,"...options...");
        new->org.summary = "Help not available";
        new->org.since = "Not known";
        new->org.group = commandGroups[0];
    }
    freeReplyObject(reply);
}


/* Returns the total number of commands and subcommands in the command docs table. */
static size_t cliCountCommands(redisReply* commandTable) {
    size_t numCommands = commandTable->elements / 2;

    /* The command docs table maps command names to a map of their specs. */    
    for (size_t i = 0; i < commandTable->elements; i += 2) {
        assert(commandTable->element[i]->type == REDIS_REPLY_STRING);  /* Command name. */
        assert(commandTable->element[i + 1]->type == REDIS_REPLY_MAP ||
               commandTable->element[i + 1]->type == REDIS_REPLY_ARRAY);
        redisReply *map = commandTable->element[i + 1];
        for (size_t j = 0; j < map->elements; j += 2) {
            assert(map->element[j]->type == REDIS_REPLY_STRING);
            char *key = map->element[j]->str;
            if (!strcmp(key, "subcommands")) {
                redisReply *subcommands = map->element[j + 1];
                assert(subcommands->type == REDIS_REPLY_MAP || subcommands->type == REDIS_REPLY_ARRAY);
                numCommands += subcommands->elements / 2;
            }
        }
    }
    return numCommands;
}

/* Fill in the fields of a help entry for the command/subcommand name. */
static void cliFillInCommandHelpEntry(helpEntry *help, char *cmdname, char *subcommandname) {
    help->argc = subcommandname ? 2 : 1;
    help->argv = zmalloc(sizeof(sds) * help->argc);
    help->argv[0] = sdsnew(cmdname);
    sdstoupper(help->argv[0]);
    if (subcommandname) {
        /* Subcommand name is two words separated by a pipe character. */
        help->argv[1] = sdsnew(strchr(subcommandname, '|') + 1);
        sdstoupper(help->argv[1]);
    }
    sds fullname = sdsnew(help->argv[0]);
    if (subcommandname) {
        fullname = sdscat(fullname, " ");
        fullname = sdscat(fullname, help->argv[1]);
    }
    help->full = fullname;
    help->type = CLI_HELP_COMMAND;

    help->org.name = help->full;
    help->org.params = sdsempty();
    help->org.since = NULL;
}

static sds cliAddArgument(sds params, redisReply *argMap);

/* Concatenate a list of arguments to the parameter string, separated by a separator string. */
static sds cliConcatArguments(sds params, redisReply *arguments, char *separator) {
    for (size_t j = 0; j < arguments->elements; j++) {
        params = cliAddArgument(params, arguments->element[j]);
        if (j != arguments->elements - 1) {
            params = sdscat(params, separator);
        }
    }
    return params;
}


/* Concatenate a string to an sds string, but if it's empty substitute double quote marks. */
static sds sdscat_orempty(sds params, char *value) {
    if (value[0] == '\0') {
        return sdscat(params, "\"\"");
    }
    return sdscat(params, value);
}


/* Add an argument to the parameter string. */
static sds cliAddArgument(sds params, redisReply *argMap) {
    char *name = NULL;
    char *type = NULL;
    int optional = 0;
    int multiple = 0;
    int multipleToken = 0;
    redisReply *arguments = NULL;
    sds tokenPart = sdsempty();
    sds repeatPart = sdsempty();

    /* First read the fields describing the argument. */
    if (argMap->type != REDIS_REPLY_MAP && argMap->type != REDIS_REPLY_ARRAY) {
        return params;
    }
    for (size_t i = 0; i < argMap->elements; i += 2) {
        assert(argMap->element[i]->type == REDIS_REPLY_STRING);
        char *key = argMap->element[i]->str;
        if (!strcmp(key, "name")) {
            assert(argMap->element[i + 1]->type == REDIS_REPLY_STRING);
            name = argMap->element[i + 1]->str;
        } else if (!strcmp(key, "token")) {
            assert(argMap->element[i + 1]->type == REDIS_REPLY_STRING);
            char *token = argMap->element[i + 1]->str;
            tokenPart = sdscat_orempty(tokenPart, token);
        } else if (!strcmp(key, "type")) {
            assert(argMap->element[i + 1]->type == REDIS_REPLY_STRING);
            type = argMap->element[i + 1]->str;
        } else if (!strcmp(key, "arguments")) {
            arguments = argMap->element[i + 1];
        } else if (!strcmp(key, "flags")) {
            redisReply *flags = argMap->element[i + 1];
            assert(flags->type == REDIS_REPLY_SET || flags->type == REDIS_REPLY_ARRAY);
            for (size_t j = 0; j < flags->elements; j++) {
                assert(flags->element[j]->type == REDIS_REPLY_STATUS);
                char *flag = flags->element[j]->str;
                if (!strcmp(flag, "optional")) {
                    optional = 1;
                } else if (!strcmp(flag, "multiple")) {
                    multiple = 1;
                } else if (!strcmp(flag, "multiple_token")) {
                    multipleToken = 1;
                }
            }
        }
    }

    /* Then build the "repeating part" of the argument string. */
    if (!strcmp(type, "key") ||
        !strcmp(type, "string") ||
        !strcmp(type, "integer") ||
        !strcmp(type, "double") ||
        !strcmp(type, "pattern") ||
        !strcmp(type, "unix-time") ||
        !strcmp(type, "token"))
    {
        repeatPart = sdscat_orempty(repeatPart, name);
    } else if (!strcmp(type, "oneof")) {
        repeatPart = cliConcatArguments(repeatPart, arguments, "|");
    } else if (!strcmp(type, "block")) {
        repeatPart = cliConcatArguments(repeatPart, arguments, " ");
    } else if (strcmp(type, "pure-token") != 0) {
        fprintf(stderr, "Unknown type '%s' set for argument '%s'\n", type, name);
    }

    /* Finally, build the parameter string. */
    if (tokenPart[0] != '\0' && strcmp(type, "pure-token") != 0) {
        tokenPart = sdscat(tokenPart, " ");
    }
    if (optional) {
        params = sdscat(params, "[");
    }
    params = sdscat(params, tokenPart);
    params = sdscat(params, repeatPart);
    if (multiple) {
        params = sdscat(params, " [");
        if (multipleToken) {
            params = sdscat(params, tokenPart);
        }
        params = sdscat(params, repeatPart);
        params = sdscat(params, " ...]");
    }
    if (optional) {
        params = sdscat(params, "]");
    }
    sdsfree(tokenPart);
    sdsfree(repeatPart);
    return params;
}

/* Initialize a command help entry for the command/subcommand described in 'specs'.
 * 'next' points to the next help entry to be filled in.
 * 'groups' is a set of command group names to be filled in.
 * Returns a pointer to the next available position in the help entries table.
 * If the command has subcommands, this is called recursively for the subcommands.
 */
static helpEntry *cliInitCommandHelpEntry(char *cmdname, char *subcommandname,
                                          helpEntry *next, redisReply *specs,
                                          dict *groups) {
    helpEntry *help = next++;
    cliFillInCommandHelpEntry(help, cmdname, subcommandname);

    assert(specs->type == REDIS_REPLY_MAP || specs->type == REDIS_REPLY_ARRAY);
    for (size_t j = 0; j < specs->elements; j += 2) {
        assert(specs->element[j]->type == REDIS_REPLY_STRING);
        char *key = specs->element[j]->str;
        if (!strcmp(key, "summary")) {
            redisReply *reply = specs->element[j + 1];
            assert(reply->type == REDIS_REPLY_STRING);
            help->org.summary = sdsnew(reply->str);
        } else if (!strcmp(key, "since")) {
            redisReply *reply = specs->element[j + 1];
            assert(reply->type == REDIS_REPLY_STRING);
            help->org.since = sdsnew(reply->str);
        } else if (!strcmp(key, "group")) {
            redisReply *reply = specs->element[j + 1];
            assert(reply->type == REDIS_REPLY_STRING);
            help->org.group = sdsnew(reply->str);
            sds group = sdsdup(help->org.group);
            if (dictAdd(groups, group, NULL) != DICT_OK) {
                sdsfree(group);
            }
        } else if (!strcmp(key, "arguments")) {
            redisReply *args = specs->element[j + 1];
            assert(args->type == REDIS_REPLY_ARRAY);
            help->org.params = cliConcatArguments(help->org.params, args, " ");
        } else if (!strcmp(key, "subcommands")) {
            redisReply *subcommands = specs->element[j + 1];
            assert(subcommands->type == REDIS_REPLY_MAP || subcommands->type == REDIS_REPLY_ARRAY);
            for (size_t i = 0; i < subcommands->elements; i += 2) {
                assert(subcommands->element[i]->type == REDIS_REPLY_STRING);
                char *subcommandname = subcommands->element[i]->str;
                redisReply *subcommand = subcommands->element[i + 1];
                assert(subcommand->type == REDIS_REPLY_MAP || subcommand->type == REDIS_REPLY_ARRAY);
                next = cliInitCommandHelpEntry(cmdname, subcommandname, next, subcommand, groups);
            }
        }
    }
    return next;
}

/* Initializes help entries for all commands in the COMMAND DOCS reply. */
void cliInitCommandHelpEntries(redisReply *commandTable, dict *groups) {
    helpEntry *next = helpEntries;
    for (size_t i = 0; i < commandTable->elements; i += 2) {
        assert(commandTable->element[i]->type == REDIS_REPLY_STRING);
        char *cmdname = commandTable->element[i]->str;

        assert(commandTable->element[i + 1]->type == REDIS_REPLY_MAP ||
               commandTable->element[i + 1]->type == REDIS_REPLY_ARRAY);
        redisReply *cmdspecs = commandTable->element[i + 1];
        next = cliInitCommandHelpEntry(cmdname, NULL, next, cmdspecs, groups);
    }
}

/* Initializes command help entries for command groups.
 * Called after the command help entries have already been filled in.
 * Extends the help table with new entries for the command groups.
 */
void cliInitGroupHelpEntries(dict *groups) {
    dictIterator *iter = dictGetIterator(groups);
    dictEntry *entry;
    helpEntry tmp;

    int numGroups = dictSize(groups);
    int pos = helpEntriesLen;
    helpEntriesLen += numGroups;
    helpEntries = zrealloc(helpEntries, sizeof(helpEntry)*helpEntriesLen);

    for (entry = dictNext(iter); entry != NULL; entry = dictNext(iter)) {
        tmp.argc = 1;
        tmp.argv = zmalloc(sizeof(sds));
        tmp.argv[0] = sdscatprintf(sdsempty(),"@%s",(char *)entry->key);
        tmp.full = tmp.argv[0];
        tmp.type = CLI_HELP_GROUP;
        tmp.org.name = NULL;
        tmp.org.params = NULL;
        tmp.org.summary = NULL;
        tmp.org.since = NULL;
        tmp.org.group = NULL;
        helpEntries[pos++] = tmp;
    }
    dictReleaseIterator(iter);
}


/* cliInitHelp() sets up the helpEntries array with the command and group
 * names and command descriptions obtained using the COMMAND DOCS command.
 */
static void cliInitHelp(void) {
    /* Dict type for a set of strings, used to collect names of command groups. */
    dictType groupsdt = {
        dictSdsHash,                /* hash function */
        NULL,                       /* key dup */
        NULL,                       /* val dup */
        dictSdsKeyCompare,          /* key compare */
        dictSdsDestructor,          /* key destructor */
        NULL,                       /* val destructor */
        NULL                        /* allow to expand */
    };
    redisReply *commandTable;
    dict *groups;

    if (cliConnect(CC_QUIET) == REDIS_ERR) {
        /* Can not connect to the server, but we still want to provide
         * help, generate it only from the old help.h data instead. */
        cliOldInitHelp(); //debug michael
        return;
    }
    commandTable = redisCommand(context, "COMMAND DOCS");
    if (commandTable == NULL || commandTable->type == REDIS_REPLY_ERROR) {
        /* New COMMAND DOCS subcommand not supported - generate help from old help.h data instead. */
        freeReplyObject(commandTable);
        cliOldInitHelp();
        cliOldIntegrateHelp();
        return;
    };
    if (commandTable->type != REDIS_REPLY_MAP && commandTable->type != REDIS_REPLY_ARRAY) return;

    /* Scan the array reported by COMMAND DOCS and fill in the entries */
    helpEntriesLen = cliCountCommands(commandTable);
    helpEntries = zmalloc(sizeof(helpEntry)*helpEntriesLen);

    groups = dictCreate(&groupsdt);
    cliInitCommandHelpEntries(commandTable, groups);
    cliInitGroupHelpEntries(groups);

    qsort(helpEntries, helpEntriesLen, sizeof(helpEntry), helpEntryCompare);
    freeReplyObject(commandTable);
    dictRelease(groups);
}

/* Return the name of the dotfile for the specified 'dotfilename'.
 * Normally it just concatenates user $HOME to the file specified
 * in 'dotfilename'. However if the environment variable 'envoverride'
 * is set, its value is taken as the path.
 *
 * The function returns NULL (if the file is /dev/null or cannot be
 * obtained for some error), or an SDS string that must be freed by
 * the user. */
static sds getDotfilePath(char *envoverride, char *dotfilename) {
    char *path = NULL;
    sds dotPath = NULL;

    /* Check the env for a dotfile override. */
    path = getenv(envoverride);
    if (path != NULL && *path != '\0') {
        if (!strcmp("/dev/null", path)) {
            return NULL;
        }

        /* If the env is set, return it. */
        dotPath = sdsnew(path);
    } else {
        char *home = getenv("HOME");
        if (home != NULL && *home != '\0') {
            /* If no override is set use $HOME/<dotfilename>. */
            dotPath = sdscatprintf(sdsempty(), "%s/%s", home, dotfilename);
        }
    }
    return dotPath;
}

/* Linenoise completion callback. */
static void completionCallback(const char *buf, linenoiseCompletions *lc) {
    size_t startpos = 0;
    int mask;
    int i;
    size_t matchlen;
    sds tmp;

    if (strncasecmp(buf,"help ",5) == 0) {
        startpos = 5;
        while (isspace(buf[startpos])) startpos++;
        mask = CLI_HELP_COMMAND | CLI_HELP_GROUP;
    } else {
        mask = CLI_HELP_COMMAND;
    }

    for (i = 0; i < helpEntriesLen; i++) {
        if (!(helpEntries[i].type & mask)) continue;

        matchlen = strlen(buf+startpos);
        if (strncasecmp(buf+startpos,helpEntries[i].full,matchlen) == 0) {
            tmp = sdsnewlen(buf,startpos);
            tmp = sdscat(tmp,helpEntries[i].full);
            linenoiseAddCompletion(lc,tmp);
            sdsfree(tmp);
        }
    }
}


/* Set the CLI preferences. This function is invoked when an interactive
 * ":command" is called, or when reading ~/.redisclirc file, in order to
 * set user preferences. */
void cliSetPreferences(char **argv, int argc, int interactive) {
    if (!strcasecmp(argv[0],":set") && argc >= 2) {
        if (!strcasecmp(argv[1],"hints")) pref.hints = 1;
        else if (!strcasecmp(argv[1],"nohints")) pref.hints = 0;
        else {
            printf("%sunknown redis-cli preference '%s'\n",
                interactive ? "" : ".redisclirc: ",
                argv[1]);
        }
    } else {
        printf("%sunknown redis-cli internal command '%s'\n",
            interactive ? "" : ".redisclirc: ",
            argv[0]);
    }
}

/* Load the ~/.redisclirc file if any. */
void cliLoadPreferences(void) {
    sds rcfile = getDotfilePath(REDIS_CLI_RCFILE_ENV,REDIS_CLI_RCFILE_DEFAULT);
    if (rcfile == NULL) return;
    FILE *fp = fopen(rcfile,"r");
    char buf[1024];

    if (fp) {
        while(fgets(buf,sizeof(buf),fp) != NULL) {
            sds *argv;
            int argc;

            argv = sdssplitargs(buf,&argc);
            if (argc > 0) cliSetPreferences(argv,argc,0);
            sdsfreesplitres(argv,argc);
        }
        fclose(fp);
    }
    sdsfree(rcfile);
}

/* Split the user provided command into multiple SDS arguments.
 * This function normally uses sdssplitargs() from sds.c which is able
 * to understand "quoted strings", escapes and so forth. However when
 * we are in Lua debugging mode and the "eval" command is used, we want
 * the remaining Lua script (after "e " or "eval ") to be passed verbatim
 * as a single big argument. */
static sds *cliSplitArgs(char *line, int *argc) {
    if (config.eval_ldb && (strstr(line,"eval ") == line ||
                            strstr(line,"e ") == line))
    {
        sds *argv = sds_malloc(sizeof(sds)*2);
        *argc = 2;
        int len = strlen(line);
        int elen = line[1] == ' ' ? 2 : 5; /* "e " or "eval "? */
        argv[0] = sdsnewlen(line,elen-1);
        argv[1] = sdsnewlen(line+elen,len-elen);
        return argv;
    } else {
        return sdssplitargs(line,argc);
    }
}


static void cliPrintContextError(void) {
    if (context == NULL) return;
    fprintf(stderr,"Error: %s\n",context->errstr);
}

static int cliReadReply(int output_raw_strings) {
    void *_reply;
    redisReply *reply;
    sds out = NULL;
    int output = 1;

    if (redisGetReply(context,&_reply) != REDIS_OK) {
        if (config.blocking_state_aborted) {
            config.blocking_state_aborted = 0;
            config.monitor_mode = 0;
            config.pubsub_mode = 0;
            return cliConnect(CC_FORCE);
        }

        if (config.shutdown) {
            redisFree(context);
            context = NULL;
            return REDIS_OK;
        }
        if (config.interactive) {
            /* Filter cases where we should reconnect */
            if (context->err == REDIS_ERR_IO &&
                (errno == ECONNRESET || errno == EPIPE))
                return REDIS_ERR;
            if (context->err == REDIS_ERR_EOF)
                return REDIS_ERR;
        }
        cliPrintContextError();
        exit(1);
        return REDIS_ERR; /* avoid compiler warning */
    }

    reply = (redisReply*)_reply;

    config.last_cmd_type = reply->type;

    /* Check if we need to connect to a different node and reissue the
     * request. */
    if (config.cluster_mode && reply->type == REDIS_REPLY_ERROR &&
        (!strncmp(reply->str,"MOVED ",6) || !strncmp(reply->str,"ASK ",4)))
    {
        char *p = reply->str, *s;
        int slot;

        output = 0;
        /* Comments show the position of the pointer as:
         *
         * [S] for pointer 's'
         * [P] for pointer 'p'
         */
        s = strchr(p,' ');      /* MOVED[S]3999 127.0.0.1:6381 */
        p = strchr(s+1,' ');    /* MOVED[S]3999[P]127.0.0.1:6381 */
        *p = '\0';
        slot = atoi(s+1);
        s = strrchr(p+1,':');    /* MOVED 3999[P]127.0.0.1[S]6381 */
        *s = '\0';
        sdsfree(config.conn_info.hostip);
        config.conn_info.hostip = sdsnew(p+1);
        config.conn_info.hostport = atoi(s+1);
        if (config.interactive)
            printf("-> Redirected to slot [%d] located at %s:%d\n",
                slot, config.conn_info.hostip, config.conn_info.hostport);
        config.cluster_reissue_command = 1;
        if (!strncmp(reply->str,"ASK ",4)) {
            config.cluster_send_asking = 1;
        }
        cliRefreshPrompt();
    } else if (!config.interactive && config.set_errcode && 
        reply->type == REDIS_REPLY_ERROR) 
    {
        fprintf(stderr,"%s\n",reply->str);
        exit(1);
        return REDIS_ERR; /* avoid compiler warning */
    }

    if (output) {
        out = cliFormatReply(reply, config.output, output_raw_strings);
        fwrite(out,sdslen(out),1,stdout);
        fflush(stdout);
        sdsfree(out);
    }
    freeReplyObject(reply);
    return REDIS_OK;
}


/* Some commands can include sensitive information and shouldn't be put in the
 * history file. Currently these commands are include:
 * - AUTH
 * - ACL SETUSER
 * - CONFIG SET masterauth/masteruser/requirepass
 * - HELLO with [AUTH username password]
 * - MIGRATE with [AUTH password] or [AUTH2 username password] */
static int isSensitiveCommand(int argc, char **argv) {
    if (!strcasecmp(argv[0],"auth")) {
        return 1;
    } else if (argc > 1 &&
        !strcasecmp(argv[0],"acl") &&
        !strcasecmp(argv[1],"setuser"))
    {
        return 1;
    } else if (argc > 2 &&
        !strcasecmp(argv[0],"config") &&
        !strcasecmp(argv[1],"set") && (
            !strcasecmp(argv[2],"masterauth") ||
            !strcasecmp(argv[2],"masteruser") ||
            !strcasecmp(argv[2],"requirepass")))
    {
        return 1;
    /* HELLO [protover [AUTH username password] [SETNAME clientname]] */
    } else if (argc > 4 && !strcasecmp(argv[0],"hello")) {
        for (int j = 2; j < argc; j++) {
            int moreargs = argc - 1 - j;
            if (!strcasecmp(argv[j],"AUTH") && moreargs >= 2) {
                return 1;
            } else if (!strcasecmp(argv[j],"SETNAME") && moreargs) {
                j++;
            } else {
                return 0;
            }
        }
    /* MIGRATE host port key|"" destination-db timeout [COPY] [REPLACE]
     * [AUTH password] [AUTH2 username password] [KEYS key [key ...]] */
    } else if (argc > 7 && !strcasecmp(argv[0], "migrate")) {
        for (int j = 6; j < argc; j++) {
            int moreargs = argc - 1 - j;
            if (!strcasecmp(argv[j],"auth") && moreargs) {
                return 1;
            } else if (!strcasecmp(argv[j],"auth2") && moreargs >= 2) {
                return 1;
            } else if (!strcasecmp(argv[j],"keys") && moreargs) {
                return 0;
            }
        }
    }
    return 0;
}

/* Output command help to stdout. */
static void cliOutputCommandHelp(struct commandDocs *help, int group) {
    printf("\r\n  \x1b[1m%s\x1b[0m \x1b[90m%s\x1b[0m\r\n", help->name, help->params);
    printf("  \x1b[33msummary:\x1b[0m %s\r\n", help->summary);
    if (help->since != NULL) {
        printf("  \x1b[33msince:\x1b[0m %s\r\n", help->since);
    }
    if (group) {
        printf("  \x1b[33mgroup:\x1b[0m %s\r\n", help->group);
    }
}

/* Print generic help. */
static void cliOutputGenericHelp(void) {
    sds version = cliVersion();
    printf(
        "redis-cli %s\n"
        "To get help about Redis commands type:\n"
        "      \"help @<group>\" to get a list of commands in <group>\n"
        "      \"help <command>\" for help on <command>\n"
        "      \"help <tab>\" to get a list of possible help topics\n"
        "      \"quit\" to exit\n"
        "\n"
        "To set redis-cli preferences:\n"
        "      \":set hints\" enable online hints\n"
        "      \":set nohints\" disable online hints\n"
        "Set your preferences in ~/.redisclirc\n",
        version
    );
    sdsfree(version);
}

/* Output all command help, filtering by group or command name. */
static void cliOutputHelp(int argc, char **argv) {
    int i, j;
    char *group = NULL;
    helpEntry *entry;
    struct commandDocs *help;

    if (argc == 0) {
        cliOutputGenericHelp();
        return;
    } else if (argc > 0 && argv[0][0] == '@') {
        group = argv[0]+1;
    }

    if (helpEntries == NULL) {
        /* Initialize the help using the results of the COMMAND command.
         * In case we are using redis-cli help XXX, we need to init it. */
        cliInitHelp();
    }

    assert(argc > 0);
    for (i = 0; i < helpEntriesLen; i++) {
        entry = &helpEntries[i];
        if (entry->type != CLI_HELP_COMMAND) continue;

        help = &entry->org;
        if (group == NULL) {
            /* Compare all arguments */
            if (argc <= entry->argc) {
                for (j = 0; j < argc; j++) {
                    if (strcasecmp(argv[j],entry->argv[j]) != 0) break;
                }
                if (j == argc) {
                    cliOutputCommandHelp(help,1);
                }
            }
        } else if (strcasecmp(group, help->group) == 0) {
            cliOutputCommandHelp(help,0);
        }
    }
    printf("\r\n");
}



static int cliSendCommand(int argc, char **argv, long repeat) {
    char *command = argv[0];
    size_t *argvlen;
    int j, output_raw;

    if (context == NULL) return REDIS_ERR;

    output_raw = 0;
    if (!strcasecmp(command,"info") ||
        !strcasecmp(command,"lolwut") ||
        (argc >= 2 && !strcasecmp(command,"debug") &&
                       !strcasecmp(argv[1],"htstats")) ||
        (argc >= 2 && !strcasecmp(command,"debug") &&
                       !strcasecmp(argv[1],"htstats-key")) ||
        (argc >= 2 && !strcasecmp(command,"debug") &&
                       !strcasecmp(argv[1],"client-eviction")) ||
        (argc >= 2 && !strcasecmp(command,"memory") &&
                      (!strcasecmp(argv[1],"malloc-stats") ||
                       !strcasecmp(argv[1],"doctor"))) ||
        (argc == 2 && !strcasecmp(command,"cluster") &&
                      (!strcasecmp(argv[1],"nodes") ||
                       !strcasecmp(argv[1],"info"))) ||
        (argc >= 2 && !strcasecmp(command,"client") &&
                       (!strcasecmp(argv[1],"list") ||
                        !strcasecmp(argv[1],"info"))) ||
        (argc == 3 && !strcasecmp(command,"latency") &&
                       !strcasecmp(argv[1],"graph")) ||
        (argc == 2 && !strcasecmp(command,"latency") &&
                       !strcasecmp(argv[1],"doctor")) ||
        /* Format PROXY INFO command for Redis Cluster Proxy:
         * https://github.com/artix75/redis-cluster-proxy */
        (argc >= 2 && !strcasecmp(command,"proxy") &&
                       !strcasecmp(argv[1],"info")))
    {
        output_raw = 1;
    }

    if (!strcasecmp(command,"shutdown")) config.shutdown = 1;
    if (!strcasecmp(command,"monitor")) config.monitor_mode = 1;
    if (!strcasecmp(command,"subscribe") ||
        !strcasecmp(command,"psubscribe") ||
        !strcasecmp(command,"ssubscribe")) config.pubsub_mode = 1;
    if (!strcasecmp(command,"sync") ||
        !strcasecmp(command,"psync")) config.slave_mode = 1;

    /* When the user manually calls SCRIPT DEBUG, setup the activation of
     * debugging mode on the next eval if needed. */
    if (argc == 3 && !strcasecmp(argv[0],"script") &&
                     !strcasecmp(argv[1],"debug"))
    {
        if (!strcasecmp(argv[2],"yes") || !strcasecmp(argv[2],"sync")) {
            config.enable_ldb_on_eval = 1;
        } else {
            config.enable_ldb_on_eval = 0;
        }
    }

    /* Actually activate LDB on EVAL if needed. */
    if (!strcasecmp(command,"eval") && config.enable_ldb_on_eval) {
        config.eval_ldb = 1;
        config.output = OUTPUT_RAW;
    }

    /* Setup argument length */
    argvlen = zmalloc(argc*sizeof(size_t));
    for (j = 0; j < argc; j++)
        argvlen[j] = sdslen(argv[j]);

    /* Negative repeat is allowed and causes infinite loop,
       works well with the interval option. */
    while(repeat < 0 || repeat-- > 0) {
        redisAppendCommandArgv(context,argc,(const char**)argv,argvlen);
        if (config.monitor_mode) {
            do {
                if (cliReadReply(output_raw) != REDIS_OK) {
                    cliPrintContextError();
                    exit(1);
                }
                fflush(stdout);

                /* This happens when the MONITOR command returns an error. */
                if (config.last_cmd_type == REDIS_REPLY_ERROR)
                    config.monitor_mode = 0;
            } while(config.monitor_mode);
            zfree(argvlen);
            return REDIS_OK;
        }

        if (config.pubsub_mode) {
            if (config.output != OUTPUT_RAW)
                printf("Reading messages... (press Ctrl-C to quit)\n");

            /* Unset our default PUSH handler so this works in RESP2/RESP3 */
            redisSetPushCallback(context, NULL);

            while (config.pubsub_mode) {
                if (cliReadReply(output_raw) != REDIS_OK) {
                    cliPrintContextError();
                    exit(1);
                }
                fflush(stdout); /* Make it grep friendly */
                if (!config.pubsub_mode || config.last_cmd_type == REDIS_REPLY_ERROR) {
                    if (config.push_output) {
                        redisSetPushCallback(context, cliPushHandler);
                    }
                    config.pubsub_mode = 0;
                }
            }
            continue;
        }

        if (config.slave_mode) {
            printf("Entering replica output mode...  (press Ctrl-C to quit)\n");
            // slaveMode(); //debug mode
            config.slave_mode = 0;
            zfree(argvlen);
            return REDIS_ERR;  /* Error = slaveMode lost connection to master */
        }

        if (cliReadReply(output_raw) != REDIS_OK) {
            zfree(argvlen);
            return REDIS_ERR;
        } else {
            /* Store database number when SELECT was successfully executed. */
            if (!strcasecmp(command,"select") && argc == 2 && 
                config.last_cmd_type != REDIS_REPLY_ERROR) 
            {
                config.conn_info.input_dbnum = config.dbnum = atoi(argv[1]);
                cliRefreshPrompt();
            } else if (!strcasecmp(command,"auth") && (argc == 2 || argc == 3)) {
                cliSelect();
            } else if (!strcasecmp(command,"multi") && argc == 1 &&
                config.last_cmd_type != REDIS_REPLY_ERROR) 
            {
                config.in_multi = 1;
                config.pre_multi_dbnum = config.dbnum;
                cliRefreshPrompt();
            } else if (!strcasecmp(command,"exec") && argc == 1 && config.in_multi) {
                config.in_multi = 0;
                if (config.last_cmd_type == REDIS_REPLY_ERROR ||
                    config.last_cmd_type == REDIS_REPLY_NIL)
                {
                    config.conn_info.input_dbnum = config.dbnum = config.pre_multi_dbnum;
                }
                cliRefreshPrompt();
            } else if (!strcasecmp(command,"discard") && argc == 1 && 
                config.last_cmd_type != REDIS_REPLY_ERROR) 
            {
                config.in_multi = 0;
                config.conn_info.input_dbnum = config.dbnum = config.pre_multi_dbnum;
                cliRefreshPrompt();
            } else if (!strcasecmp(command,"reset") && argc == 1 &&
                                     config.last_cmd_type != REDIS_REPLY_ERROR) {
                config.in_multi = 0;
                config.dbnum = 0;
                config.conn_info.input_dbnum = 0;
                config.resp3 = 0;
                cliRefreshPrompt();
            }
        }
        if (config.cluster_reissue_command){
            /* If we need to reissue the command, break to prevent a
               further 'repeat' number of dud interactions */
            break;
        }
        if (config.interval) usleep(config.interval);
        fflush(stdout); /* Make it grep friendly */
    }

    zfree(argvlen);
    return REDIS_OK;
}

/* In cluster, if server replies ASK, we will redirect to a different node.
 * Before sending the real command, we need to send ASKING command first. */
static int cliSendAsking() {
    redisReply *reply;

    config.cluster_send_asking = 0;
    if (context == NULL) {
        return REDIS_ERR;
    }
    reply = redisCommand(context,"ASKING");
    if (reply == NULL) {
        fprintf(stderr, "\nI/O error\n");
        return REDIS_ERR;
    }
    int result = REDIS_OK;
    if (reply->type == REDIS_REPLY_ERROR) {
        result = REDIS_ERR;
        fprintf(stderr,"ASKING failed: %s\n",reply->str);
    }
    freeReplyObject(reply);
    return result;
}

static int issueCommandRepeat(int argc, char **argv, long repeat) {
    /* In Lua debugging mode, we want to pass the "help" to Redis to get
     * it's own HELP message, rather than handle it by the CLI, see ldbRepl.
     *
     * For the normal Redis HELP, we can process it without a connection. */
    if (!config.eval_ldb &&
        (!strcasecmp(argv[0],"help") || !strcasecmp(argv[0],"?")))
    {
        cliOutputHelp(--argc, ++argv);
        return REDIS_OK;
    }

    while (1) {
        if (config.cluster_reissue_command || context == NULL ||
            context->err == REDIS_ERR_IO || context->err == REDIS_ERR_EOF)
        {
            if (cliConnect(CC_FORCE) != REDIS_OK) {
                cliPrintContextError();
                config.cluster_reissue_command = 0;
                return REDIS_ERR;
            }
        }
        config.cluster_reissue_command = 0;
        if (config.cluster_send_asking) {
            if (cliSendAsking() != REDIS_OK) {
                cliPrintContextError();
                return REDIS_ERR;
            }
        }
        if (cliSendCommand(argc,argv,repeat) != REDIS_OK) {
            cliPrintContextError();
            redisFree(context);
            context = NULL;
            return REDIS_ERR;
        }

        /* Issue the command again if we got redirected in cluster mode */
        if (config.cluster_mode && config.cluster_reissue_command) {
            continue;
        }
        break;
    }
    return REDIS_OK;
}

static void repl(void) {
    sds historyfile = NULL;
    int history = 0;
    char *line;
    int argc;
    sds *argv;

    /* There is no need to initialize redis HELP when we are in lua debugger mode.
     * It has its own HELP and commands (COMMAND or COMMAND DOCS will fail and got nothing).
     * We will initialize the redis HELP after the Lua debugging session ended.*/
    if (!config.eval_ldb) {
        /* Initialize the help using the results of the COMMAND command. */
        cliInitHelp();
    }

    config.interactive = 1;
    linenoiseSetMultiLine(1); //debug michael
    linenoiseSetCompletionCallback(completionCallback);//debug michael
    // linenoiseSetHintsCallback(hintsCallback);//debug michael
    // linenoiseSetFreeHintsCallback(freeHintsCallback);//debug michael

    /* Only use history and load the rc file when stdin is a tty. */
    if (isatty(fileno(stdin))) {
        historyfile = getDotfilePath(REDIS_CLI_HISTFILE_ENV,REDIS_CLI_HISTFILE_DEFAULT);
        //keep in-memory history always regardless if history file can be determined
        history = 1;
        if (historyfile != NULL) {
            linenoiseHistoryLoad(historyfile);
        }
        cliLoadPreferences();
    }

    cliRefreshPrompt();
    while((line = linenoise(context ? config.prompt : "not connected> ")) != NULL) {
        if (line[0] != '\0') {
            long repeat = 1;
            int skipargs = 0;
            char *endptr = NULL;

            argv = cliSplitArgs(line,&argc);
            if (argv == NULL) {
                printf("Invalid argument(s)\n");
                fflush(stdout);
                if (history) linenoiseHistoryAdd(line);
                if (historyfile) linenoiseHistorySave(historyfile);
                linenoiseFree(line);
                continue;
            } else if (argc == 0) {
                sdsfreesplitres(argv,argc);
                linenoiseFree(line);
                continue;
            }

            /* check if we have a repeat command option and
             * need to skip the first arg */
            errno = 0;
            repeat = strtol(argv[0], &endptr, 10);
            if (argc > 1 && *endptr == '\0') {
                if (errno == ERANGE || errno == EINVAL || repeat <= 0) {
                    fputs("Invalid redis-cli repeat command option value.\n", stdout);
                    sdsfreesplitres(argv, argc);
                    linenoiseFree(line);
                    continue;
                }
                skipargs = 1;
            } else {
                repeat = 1;
            }

            if (!isSensitiveCommand(argc - skipargs, argv + skipargs)) {
                if (history) linenoiseHistoryAdd(line);
                if (historyfile) linenoiseHistorySave(historyfile);
            }

            if (strcasecmp(argv[0],"quit") == 0 ||
                strcasecmp(argv[0],"exit") == 0)
            {
                exit(0);
            } else if (argv[0][0] == ':') {
                cliSetPreferences(argv,argc,1);
                sdsfreesplitres(argv,argc);
                linenoiseFree(line);
                continue;
            } else if (strcasecmp(argv[0],"restart") == 0) {
                if (config.eval) {
                    config.eval_ldb = 1;
                    config.output = OUTPUT_RAW;
                    sdsfreesplitres(argv,argc);
                    linenoiseFree(line);
                    return; /* Return to evalMode to restart the session. */
                } else {
                    printf("Use 'restart' only in Lua debugging mode.\n");
                    fflush(stdout);
                }
            } else if (argc == 3 && !strcasecmp(argv[0],"connect")) {
                sdsfree(config.conn_info.hostip);
                config.conn_info.hostip = sdsnew(argv[1]);
                config.conn_info.hostport = atoi(argv[2]);
                cliRefreshPrompt();
                cliConnect(CC_FORCE);
            } else if (argc == 1 && !strcasecmp(argv[0],"clear")) {
                linenoiseClearScreen();
            } else {
                long long start_time = mstime(), elapsed;

                issueCommandRepeat(argc-skipargs, argv+skipargs, repeat);

                /* If our debugging session ended, show the EVAL final
                    * reply. */
                if (config.eval_ldb_end) {
                    config.eval_ldb_end = 0;
                    cliReadReply(0);
                    printf("\n(Lua debugging session ended%s)\n\n",
                        config.eval_ldb_sync ? "" :
                        " -- dataset changes rolled back");
                    cliInitHelp();
                }

                elapsed = mstime()-start_time;
                if (elapsed >= 500 &&
                    config.output == OUTPUT_STANDARD)
                {
                    printf("(%.2fs)\n",(double)elapsed/1000);
                }
            }
            /* Free the argument vector */
            sdsfreesplitres(argv,argc);
        }
        /* linenoise() returns malloc-ed lines like readline() */
        linenoiseFree(line);
    }
    exit(0);
}

int main(int argc, char **argv) {
     printf("start client %d",argc);
      int firstarg;
    struct timeval tv;
    memset(&config.sslconfig, 0, sizeof(config.sslconfig));
    pref.hints = 1;

    firstarg = parseOptions(argc,argv);
    argc -= firstarg;
    argv += firstarg;
    parseEnv();
    gettimeofday(&tv, NULL);
    init_genrand64(((long long) tv.tv_sec * 1000000 + tv.tv_usec) ^ getpid());
      /* Start interactive mode when no command is provided */
    if (argc == 0 && !config.eval) {
        /* Ignore SIGPIPE in interactive mode to force a reconnect */
        signal(SIGPIPE, SIG_IGN);
        signal(SIGINT, sigIntHandler);

        /* Note that in repl mode we don't abort on connection error.
         * A new attempt will be performed for every command send. */
        cliConnect(0);
        repl();
    }


}