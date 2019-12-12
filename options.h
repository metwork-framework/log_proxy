#ifndef OPTIONS_H_
#define OPTIONS_H_

#include <glib.h>

static gchar *log_file = NULL;
static glong rotation_size = 104857600;
static glong rotation_time = 86400;
static gchar *rotation_suffix = ".%Y%m%d%H%M%S";
static gboolean rm_fifo_at_exit = FALSE;
static gchar *fifo = NULL;
static gint rotated_files = 5;
static gboolean use_locks = FALSE;

static GOptionEntry entries[] = {
    { "rotation-size", 's', 0, G_OPTION_ARG_INT, &rotation_size, "maximum size (in bytes) for a log file before rotation (0 => no maximum, default: 104857600 (100MB))", NULL },
    { "rotation-time", 't', 0, G_OPTION_ARG_INT, &rotation_time, "maximum lifetime (in seconds) for a log file before rotation (0 => no maximum, default: 86400 (24H))", NULL },
    { "rotation-suffix", 'S', 0, G_OPTION_ARG_STRING, &rotation_suffix, "strftime based suffix to append to rotated log files (default: .%%Y%%m%%d%%H%%M%%S", NULL },
    { "rotated-files", 'n', 0, G_OPTION_ARG_INT, &rotated_files, "maximum number of rotated files to keep including main one (0 => no cleaning, default: 5)", NULL },
    { "use-locks", 'm', 0, G_OPTION_ARG_NONE, &use_locks, "use locks to append to main log file (useful if several process writes to the same file)", NULL },
    { "fifo", 'f', 0, G_OPTION_ARG_STRING, &fifo, "if set, read lines on this fifo instead of stdin", NULL },
    { "rm-fifo-at-exit", 'r', 0, G_OPTION_ARG_NONE, &rm_fifo_at_exit, "if set, drop fifo at then end of the program (you have to use --fifo option of course)", NULL }
};

#endif /* OPTIONS_H_ */
