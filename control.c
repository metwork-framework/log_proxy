#include <glib.h>
#include <glib/gstdio.h>
#include <fcntl.h>
#include <sys/file.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "control.h"

gchar *_get_control_file_path(const gchar *path) {
    return g_strdup_printf("%s.control", path);
}

/**
 * Init the control file with the given content.
 *
 * If the file already exists or if errors, FALSE is returned.
 *
 * @param path log file path.
 * @param content the content to put in the control file.
 * @return TRUE if ok, FALSE if the control file is already here.
 */
gboolean init_control_file(const gchar *path, const gchar *content) {
    gboolean res = FALSE;
    gchar *cfile = _get_control_file_path(path);
    int fd = open(cfile, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd < 0) {
        g_free(cfile);
        return FALSE;
    }
    size_t content_size = strlen(content);
    ssize_t write_size = write(fd, content, content_size);
    if (write_size > 0) {
        if ((size_t) write_size == content_size) {
            res = TRUE;
        }
    }
    g_free(cfile);
    close(fd);
    return res;
}

/**
 * Lock the control file.
 *
 * If the returned value is >=0, we have an exclusive lock.
 *
 * @param path log file path.
 * @return a file descriptor (< 0 in case of errors)
 */
int lock_control_file(const gchar *path) {
    int fd = -1;
    while (TRUE) {
        gchar *cfile = _get_control_file_path(path);
        fd = open(cfile, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        g_free(cfile);
        if (fd < 0) {
            if (errno == EINTR) {
                // try another time
                continue;
            }
            return -1;
        } else {
            break;
        }
    }
    while (TRUE) {
        int res = flock(fd, LOCK_EX);
        if (res < 0) {
            if (errno == EINTR) {
                // try another time
                continue;
            }
            return -1;
        } else {
            break;
        }
    }
    return fd;
}

/**
 * Unlock the control file.
 *
 * @param fd the file descriptor got with lock_control_file.
 */
void unlock_control_file(int fd) {
    while (TRUE) {
        int res = close(fd);
        if (res < 0) {
            if (errno == EINTR) {
                // try another time
                continue;
            }
        }
        break;
    }
}

/**
 * Get the control file content
 *
 * If there are some errors, NULL is returned.
 *
 * @param path log file path.
 * @return newly allocated string with the content (free with g_free).
 */
gchar *get_control_file_content(const gchar *path) {
    gchar *cfile = _get_control_file_path(path);
    gchar *contents = NULL;
    gboolean res = g_file_get_contents(cfile, &contents, NULL, NULL);
    g_free(cfile);
    if (res == TRUE) {
        return contents;
    } else {
        return NULL;
    }
}
