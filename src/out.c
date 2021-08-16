#include <glib.h>
#include <glib/gprintf.h>
#include <glib/gstdio.h>
#include <locale.h>
#include <sys/file.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "util.h"
#include "control.h"

static GIOChannel *_out_channel = NULL;
static gboolean _use_locks = FALSE;
static gchar *_log_file = NULL;
static glong _log_file_initial_timestamp = 0;

glong get_output_channel_age() {
    g_assert(_log_file_initial_timestamp > 0);
    return get_current_timestamp() - _log_file_initial_timestamp;
}

void destroy_output_channel() {
    if (_out_channel != NULL) {
        g_io_channel_shutdown(_out_channel, TRUE, NULL);
        g_io_channel_unref(_out_channel);
        _out_channel = NULL;
    }
    g_free(_log_file);
}

void init_output_channel(const gchar *log_file, gboolean use_locks, gboolean force_control_file, const gchar *chmod_str, const gchar *chown_str, const gchar *chgrp_str) {
    _log_file = g_strdup(log_file);
    _use_locks = use_locks;
    create_empty(_log_file);
    _log_file_initial_timestamp = -1;
    gchar *content = NULL;
    if (force_control_file == FALSE) {
        content = get_control_file_content(_log_file);
        if (content != NULL) {
            _log_file_initial_timestamp = atol(content);
            g_free(content);
        }
    }
    if (_log_file_initial_timestamp <= 0) {
        _log_file_initial_timestamp = get_current_timestamp();
        content = g_strdup_printf("%li\n", (long int) _log_file_initial_timestamp);
        init_control_file(_log_file, content);
        g_free(content);
    }
    GError *error = NULL;
    while (TRUE) {
        _out_channel = g_io_channel_new_file(_log_file, "a", &error);
        if (chmod_str != NULL) {
            mode_t chmod_mode_t = strtol(chmod_str, NULL, 8);
            chmod(_log_file, chmod_mode_t);
        }
        uid_t uid = -1;
        gid_t gid = -1;
        if (chown_str != NULL) {
            uid = user_id_from_name(chown_str);
        }
        if (chgrp_str != NULL) {
            gid = group_id_from_name(chgrp_str);
        }
        if ((uid > 0) || (gid > 0)) {
            chown(_log_file, uid, gid);
        }
        if (error != NULL) {
            g_warning("error during open output channel: %s => waiting 1s and try again...", error->message);
            g_error_free(error);
            sleep(1);
            continue;
        }
        g_io_channel_set_encoding(_out_channel, NULL, NULL);
        g_io_channel_set_buffered(_out_channel, FALSE);
        break;
    }
}

gboolean test_output_channel_rotated() {
    g_assert(_out_channel != NULL);
    int fd = g_io_channel_unix_get_fd(_out_channel);
    glong fd_inode = get_fd_inode(fd);
    glong log_file_inode = get_file_inode(_log_file);
    return (fd_inode != log_file_inode);
}

gboolean write_output_channel(GString *buffer) {
    g_assert(_out_channel != NULL);
    GIOStatus write_status;
    GError *error = NULL;
    gsize written;
    while (TRUE) {
        if (_use_locks) {
            int res = flock(g_io_channel_unix_get_fd(_out_channel), LOCK_EX);
            if (res < 0) {
                continue;
            }
        }
        write_status = g_io_channel_write_chars(_out_channel, buffer->str,
                buffer->len, &written, &error);
        if (_use_locks) {
            while (TRUE) {
                int res2 = flock(g_io_channel_unix_get_fd(_out_channel), LOCK_UN);
                if (res2 < 0) {
                    if (errno == EINTR) {
                        // try again
                        continue;
                    }
                }
                break;
            }
        }
        if (write_status == G_IO_STATUS_NORMAL) {
            break;
        } else if (write_status == G_IO_STATUS_AGAIN) {
            continue;
        } else {
            return FALSE;
        }
    }
    return TRUE;
}
