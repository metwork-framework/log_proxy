#ifndef OPTIONS_H_
#define OPTIONS_H_

#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/resource.h>

static gchar *log_file = NULL;
static glong rotation_size = -1;
static glong rotation_time = -1;
static gchar *rotation_suffix = NULL;
static gchar *timestamp_prefix = NULL;
static gchar *log_directory = NULL;
static gchar *chmod_str = NULL;
static gchar *chown_str = NULL;
static gchar *chgrp_str = NULL;
static gint rotated_files = -1;
static gboolean rm_fifo_at_exit = FALSE;
static gchar *fifo = NULL;
static gboolean use_locks = FALSE;

void set_default_values_from_env()
{
    const gchar *env_val;
    struct rlimit rl;
    getrlimit(RLIMIT_FSIZE, &rl);
    if ( rotation_size == -1 ) {
        env_val = g_getenv("LOGPROXY_ROTATION_SIZE");
        if ( env_val != NULL ) {
            rotation_size = atoi(env_val);
        }
        else {
            rotation_size = 104857600;
        }
    }
    if ( (long long int) rl.rlim_max != -1 ) {
        long long int val_max = (long long int) rl.rlim_max * 90 / 100;
        if ( rotation_size > (int) val_max ) {
            rotation_size = (int) val_max;
        }
    }

    if ( rotation_time == -1 ) {
        env_val = g_getenv("LOGPROXY_ROTATION_TIME");
        if ( env_val != NULL ) {
            rotation_time = atoi(env_val);
        }
        else {
            rotation_time = 86400;
        }
    }

    if ( rotation_suffix ==  NULL ) {
        env_val = g_getenv("LOGPROXY_ROTATION_SUFFIX");
        if ( env_val != NULL ) {
            rotation_suffix = (gchar *)env_val;
        }
        else {
            rotation_suffix = ".%Y%m%d%H%M%S";
        }
    }

    if ( rotated_files == -1 ) {
        env_val = g_getenv("LOGPROXY_ROTATED_FILES");
        if ( env_val != NULL ) {
            rotated_files = atoi(env_val);
        }
        else {
            rotated_files = 5;
        }
    }

    if ( timestamp_prefix ==  NULL ) {
        env_val = g_getenv("LOGPROXY_TIMESTAMPS");
        if ( env_val != NULL ) {
            timestamp_prefix = (gchar *)env_val;
        }
    }
    if ( timestamp_prefix !=  NULL && strlen(timestamp_prefix) == 0 ) {
        timestamp_prefix = NULL;
    }

    if ( log_directory ==  NULL ) {
        env_val = g_getenv("LOGPROXY_LOG_DIRECTORY");
        if ( env_val != NULL ) {
            log_directory = (gchar *)env_val;
        }
        else {
            log_directory = g_get_current_dir();
        }
    }

    if ( chmod_str ==  NULL ) {
        env_val = g_getenv("LOGPROXY_CHMOD");
        if ( env_val != NULL ) {
            chmod_str = (gchar *)env_val;
        }
    }
}

static GOptionEntry entries[] = {
    { "rotation-size", 's', 0, G_OPTION_ARG_INT64, &rotation_size, "maximum size (in bytes) for a log file before rotation (0 => no maximum, default: content of environment variable LOGPROXY_ROTATION_SIZE or 104857600 (100MB))", NULL },
    { "rotation-time", 't', 0, G_OPTION_ARG_INT64, &rotation_time, "maximum lifetime (in seconds) for a log file before rotation (0 => no maximum, default: content of environment variable LOGPROXY_ROTATION_TIME or 86400 (24H))", NULL },
    { "rotation-suffix", 'S', 0, G_OPTION_ARG_STRING, &rotation_suffix, "strftime based suffix to append to rotated log files (default: content of environment variable LOGPROXY_ROTATION_SUFFIX or .%%Y%%m%%d%%H%%M%%S)", NULL },
    { "log-directory", 'd', 0, G_OPTION_ARG_STRING, &log_directory, "directory to store log files (default: content of environment variable LOGPROXY_LOG_DIRECTORY or current directory), directory is created if missing", NULL },
    { "rotated-files", 'n', 0, G_OPTION_ARG_INT, &rotated_files, "maximum number of rotated files to keep including main one (0 => no cleaning, default: content of environment variable LOGPROXY_ROTATED_FILES or 5)", NULL },
    { "timestamps", 'T', 0, G_OPTION_ARG_STRING, &timestamp_prefix, "strftime prefix to prepend to every output line (default: content of environment variable LOGPROXY_TIMESTAMPS or none)", NULL },
    { "chmod", 'c', 0, G_OPTION_ARG_STRING, &chmod_str, "if set, chmod the logfile to this value, '0700' for example (default: content of environment variable LOGPROXY_CHMOD or NULL)", NULL },
    { "chown", 'o', 0, G_OPTION_ARG_STRING, &chown_str, "if set, try (if you don't have sufficient privileges, it will fail silently) to change the owner of the logfile to the given user value", NULL },
    { "chgrp", 'g', 0, G_OPTION_ARG_STRING, &chgrp_str, "if set, try (if you don't have sufficient privileges, it will fail silently) to change the group of the logfile to the given group value", NULL },
    { "use-locks", 'm', 0, G_OPTION_ARG_NONE, &use_locks, "use locks to append to main log file (useful if several process writes to the same file)", NULL },
    { "fifo", 'f', 0, G_OPTION_ARG_STRING, &fifo, "if set, read lines on this fifo instead of stdin", NULL },
    { "rm-fifo-at-exit", 'r', 0, G_OPTION_ARG_NONE, &rm_fifo_at_exit, "if set, drop fifo at then end of the program (you have to use --fifo option of course)", NULL },
    { NULL }
};

#endif /* OPTIONS_H_ */
