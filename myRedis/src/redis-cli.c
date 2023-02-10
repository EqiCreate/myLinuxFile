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
// #include "linenoise.h"
// #include "help.h" /* Used for backwards-compatibility with pre-7.0 servers that don't support COMMAND DOCS. */
#include "anet.h"
#include "ae.h"
#include "connection.h"
#include "cli_common.h"
#include "Main/mt19937-64.h"

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
//     clusterManagerCommand cluster_manager_command;
    int no_auth_warning;
    int resp2;
    int resp3; /* value of 1: specified explicitly, value of 2: implicit like --json option */
    int in_multi;
    int pre_multi_dbnum;
} config;

int main(int argc, char **argv) {
     printf("client %d",argc);
      int firstarg;
    struct timeval tv;
    memset(&config.sslconfig, 0, sizeof(config.sslconfig));

}