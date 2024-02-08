#include <glib.h>
#include <glib/gstdio.h>
#include <glib/gprintf.h>
#include <locale.h>
#include <sys/file.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include "out.h"
#include "util.h"
#include "control.h"
#include "options.h"

struct sigaction sigact;
static gboolean first_iteration = TRUE;
static GMutex *mutex = NULL;
static GAsyncQueue *queue = NULL;
static volatile gboolean stop_signal = FALSE;
static GIOChannel *in = NULL;

gint _list_compare(gconstpointer a, gconstpointer b) {
    gchar *ca = (gchar *) a;
    gchar *cb = (gchar *) b;
    return g_strcmp0(cb, ca);
}

#define UNUSED(x) (void)(x)

void clean_too_old_files() {
    gchar *dirpath = g_path_get_dirname(log_file);
    GDir *dir = g_dir_open(dirpath, 0, NULL);
    if (dir == NULL) {
        g_warning("can't read dir: %s", dirpath);
        g_free(dirpath);
        return;
    }
    gchar *basename = g_path_get_basename(log_file);
    gchar *control = g_strdup_printf(".%s.control", basename);
    GList *list = NULL;
    while (TRUE) {
        const gchar *name = g_dir_read_name(dir);
        if (name == NULL) {
            break;
        }
        if (g_strcmp0(name, control) == 0) {
            continue;
        }
        if (g_strcmp0(name, basename) == 0) {
            continue;
        }
        if (g_str_has_prefix(name, basename)) {
            gchar *filepath = g_strdup_printf("%s/%s", dirpath, name);
            list = g_list_insert_sorted(list, filepath,
                    (GCompareFunc) _list_compare);
        }
    }
    if ((gint) g_list_length(list) > rotated_files) {
        GList *list2 = g_list_nth(list, rotated_files - 1);
        GList *l;
        for (l = list2; l != NULL; l = l->next) {
            int res = g_unlink((const gchar*) l->data);
            if (res < 0) {
                g_warning("can't unlink: %s", (gchar*) l->data);
            }
        }
    }
    g_list_free_full(list, g_free);
    g_free(control);
    g_free(dirpath);
    g_free(basename);
    g_dir_close(dir);
}

gboolean rotate() {
    gboolean result = FALSE;
    gchar *rotated_file = compute_strftime_suffix(log_file, rotation_suffix);
    if (rotated_file == NULL) {
        return FALSE;
    }
    if (g_file_test(rotated_file, G_FILE_TEST_EXISTS) == TRUE) {
        gchar *new_rotated_file = g_strdup_printf("%s.%s", rotated_file, get_unique_hexa_identifier());
        g_free(rotated_file);
        rotated_file = new_rotated_file;
    }
    int res = g_rename(log_file, rotated_file);
    if (res == 0) {
        result = TRUE;
    } else {
        g_warning("can't rotate %s => %s (%i)", log_file, rotated_file, errno);
    }
    g_free(rotated_file);
    return result;
}

void signal_handler(int signum) {
    if ((signum == SIGINT) || (signum == SIGTERM)) {
        stop_signal = TRUE;
    }
}

static void every_second() {
    int fd = lock_control_file(log_file);
    if (fd >= 0) {
        if (first_iteration) {
            // A little bit of cleaning for first iteration
            first_iteration = FALSE;
            clean_too_old_files();
        }
        if (test_output_channel_rotated() == TRUE) {
            // another program rotated our log file
            // => let's reinit the output channel
            destroy_output_channel();
            init_output_channel(log_file, use_locks, TRUE, chmod_str, chown_str, chgrp_str, timestamp_prefix);
            unlock_control_file(fd);
            return;
        }
        glong size = get_file_size(log_file);
        if (size < 0) {
            unlock_control_file(fd);
            return;
        }
        gboolean must_rotate = FALSE;
        if (rotation_size > 0) {
            if (size > rotation_size) {
                must_rotate = TRUE;
            }
        }
        if (rotation_time > 0) {
            if (get_output_channel_age() > rotation_time) {
                must_rotate = TRUE;
            }
        }
        if (must_rotate) {
            gboolean rotate_res = rotate();
            if (rotated_files > 0) {
                clean_too_old_files();
            }
            if (rotate_res == TRUE) {
                destroy_output_channel();
                init_output_channel(log_file, use_locks, TRUE, chmod_str, chown_str, chgrp_str, timestamp_prefix);
            }
        }
        unlock_control_file(fd);
    }
}

gpointer stop_thread(gpointer data) {
    // we do this here and not in signal_handler to avoid malloc in signal
    // handlers
    UNUSED(data);
    while (stop_signal == FALSE) {
        sleep(1);
    }
    g_async_queue_push(queue, GINT_TO_POINTER(2));
    return NULL;
}

gpointer management_thread(gpointer data) {
    gpointer qdata;
    GTimeVal tval;
    gint stop_flag = 0;
    UNUSED(data);
    while (TRUE) {
        g_get_current_time(&tval);
        g_time_val_add(&tval, 1000000);
        qdata = g_async_queue_timed_pop(queue, &tval);
        if (qdata != NULL) {
            stop_flag = GPOINTER_TO_INT(qdata);
        }
        g_mutex_lock(mutex);
        every_second();
        g_mutex_unlock(mutex);
        if (qdata != NULL) {
            break;
        }
    }
    if (stop_flag == 2) {
        // in this case (sigterm), we prefer to keep the mutex
        g_mutex_lock(mutex);
    }
    destroy_output_channel();
    if (rm_fifo_at_exit == TRUE) {
        if (fifo != NULL) {
            g_unlink(fifo);
        }
    }
    if (stop_flag == 2) {
        // we exit here for sigterm as main thread is blocked in reading
        exit(0);
    }
    return NULL;
}

void init_or_reinit_output_channel(const gchar *lg_file, gboolean us_locks) {
    int lock_fd = lock_control_file(lg_file);
    if (lock_fd < 0) {
        g_critical("can't lock control file for log_file=%s => exiting", lg_file);
        exit(2);
    }
    destroy_output_channel();
    init_output_channel(lg_file, us_locks, FALSE, chmod_str, chown_str, chgrp_str, timestamp_prefix);
    unlock_control_file(lock_fd);
}

int main(int argc, char *argv[])
{
    GOptionContext *context;
    setlocale(LC_ALL, "");
    context = g_option_context_new("LOGFILE  - log proxy");
    g_option_context_add_main_entries(context, entries, NULL);
    gchar *description = "Optional environment variables to override defaults: \n    LOGPROXY_ROTATION_SIZE\n    LOGPROXY_ROTATION_TIME\n    LOGPROXY_ROTATION_SUFFIX\n    LOGPROXY_LOG_DIRECTORY\n    LOGPROXY_ROTATED_FILES\n    LOGPROXY_TIMESTAMPS\n\nExample for rotation-size option:\n- If log_proxy is run with the option --rotation-size on the command line, rotation-size will take the provided value\n- If the option --rotation-size is not provided on command line :\n  - If the environment variable LOGPROXY_ROTATION_SIZE is set, rotation-size will take this value\n  - If the environment variable LOGPROXY_ROTATION_SIZE is not set, rotation-size will take the default value 104857600\n";
    g_option_context_set_description(context, description);
    if (!g_option_context_parse(context, &argc, &argv, NULL)) {
        g_print("%s", g_option_context_get_help(context, TRUE, NULL));
        exit(1);
    }
    if (argc < 2) {
        g_print("%s", g_option_context_get_help(context, TRUE, NULL));
        exit(1);
    }
    g_thread_init(NULL);
    mutex = g_mutex_new();
    queue = g_async_queue_new();
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    set_default_values_from_env();
    log_file = compute_file_path(log_directory, argv[1]);
    // Create log directory if not existing
    gchar *log_dir = g_path_get_dirname(log_file);
    if ( ! g_file_test(log_dir, G_FILE_TEST_IS_DIR) ) {
        if ( g_mkdir_with_parents(log_dir, 0755) == -1 ) {
            g_critical("Can't create directory %s => exit", log_dir);
            return 1;
        }
    }
    g_free(log_dir);
    if (fifo == NULL) {
        // We read from stdin
        in = g_io_channel_unix_new(fileno(stdin));
    } else {
        GError *error = NULL;
        in = g_io_channel_new_file(fifo, "r", &error);
        if (in == NULL) {
            g_critical("Can't open %s => exit", fifo);
            return 1;
        }
    }
    g_io_channel_set_encoding(in, NULL, NULL);
    GIOStatus in_status = G_IO_STATUS_NORMAL;
    GString *in_buffer = g_string_new(NULL);
    init_or_reinit_output_channel(log_file, use_locks);
    g_thread_create(stop_thread, NULL, FALSE, NULL);
    GThread* management = g_thread_create(management_thread, NULL, TRUE, NULL);
    while ((in_status != G_IO_STATUS_EOF) && (in_status != G_IO_STATUS_ERROR)) {
        in_status = g_io_channel_read_line_string(in, in_buffer, NULL, NULL);
        if (in_status == G_IO_STATUS_NORMAL) {
            g_mutex_lock(mutex);
            while (TRUE) {
                gboolean write_status = write_output_channel(in_buffer);
                if (write_status == FALSE) {
                    g_warning("error during write on: %s", log_file);
                    init_or_reinit_output_channel(log_file, use_locks);
                    continue;
                }
                break;
            }
            g_mutex_unlock(mutex);
        }
    }
    // if we are here, the "in" input channel is closed
    signal(SIGINT, SIG_DFL);
    signal(SIGTERM, SIG_DFL);
    g_async_queue_push(queue, GINT_TO_POINTER(1));
    g_thread_join(management);
    g_io_channel_shutdown(in, FALSE, NULL);
    g_io_channel_unref(in);
    g_string_free(in_buffer, TRUE);
    g_option_context_free(context);
    g_free(log_file);
    g_mutex_free(mutex);
    return 0;
}
