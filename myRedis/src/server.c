#include "server.h"
#include "config.h"
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "Main/mt19937-64.h"
// #include "functions.h"


/* Our shared "common" objects */

// struct sharedObjectsStruct shared;

// uint64_t dictSdsHash(const void *key) {
//     return dictGenHashFunction((unsigned char*)key, sdslen((char*)key));
// }

// uint64_t dictSdsCaseHash(const void *key) {
//     return dictGenCaseHashFunction((unsigned char*)key, sdslen((char*)key));
// }

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
}